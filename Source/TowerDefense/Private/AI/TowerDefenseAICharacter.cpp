// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/TowerDefenseAICharacter.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Algo/AnyOf.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/UnitAbilitySystemComponent.h"
#include "Components/WayComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Settings/TowerDefenseWorldSettings.h"
#include "Utils/BaseUtils.h"
#include "Utils/WorldGridFunctionLibrary.h"
#include "Widgets/ProgressBarWidget.h"

ATowerDefenseAICharacter::ATowerDefenseAICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
    WidgetComponent->SetupAttachment(GetMesh());

    WayComponent = CreateDefaultSubobject<UWayComponent>(TEXT("WayComponent"));
    AbilitySystemComponent = CreateDefaultSubobject<UUnitAbilitySystemComponent>(TEXT("AbilitySystem"));
    GetCapsuleComponent()->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Overlap);
    GetMesh()->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Ignore);
    WidgetComponent->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Ignore);
}

void ATowerDefenseAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    // Apply after Blueprint component defaults, including additional visual components.
    TInlineComponentArray<UPrimitiveComponent*> Components(this);
    for (UPrimitiveComponent* Component : Components)
    {
        Component->SetCollisionResponseToChannel(Collision::UnitHitTarget, Component == GetCapsuleComponent() ? ECR_Overlap : ECR_Ignore);
    }
}

FGenericTeamId ATowerDefenseAICharacter::GetGenericTeamId() const
{
    return TeamId;
}

void ATowerDefenseAICharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

UAbilitySystemComponent* ATowerDefenseAICharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UWayComponent* ATowerDefenseAICharacter::GetWayComponent() const
{
    return WayComponent;
}

float ATowerDefenseAICharacter::GetAttackRange() const
{
    return AttackRange;
}

EUnitAttackPattern ATowerDefenseAICharacter::GetAttackPattern() const
{
    return AttackPattern;
}

int32 ATowerDefenseAICharacter::GetAttackRadiusInCells() const
{
    return AttackRadiusInCells;
}

bool ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern Pattern, const FIntVector2& Delta, int32 Radius)
{
    const int32 X = FMath::Abs(Delta.X);
    const int32 Y = FMath::Abs(Delta.Y);
    if (X == 0 && Y == 0)
    {
        return false;
    }
    switch (Pattern)
    {
        case EUnitAttackPattern::TwoHanded:
        case EUnitAttackPattern::Ranged:
            return X * X + Y * Y <= Radius * Radius;
        default:
            return FMath::Max(X, Y) <= Radius;
    }
}

bool ATowerDefenseAICharacter::CanAttackFrom(const FVector& Position, const AActor* Target) const
{
    const UWorld* World = GetWorld();
    if (!Teams::CanAttackEachOther(this, Target) || !IsValid(World) || !IsTargetInRange(Position, Target))
    {
        return false;
    }

    // Test physical obstacles, not reservations: an ally may occupy the intervening cell.
    const FVector Start(Position.X, Position.Y, GetActorLocation().Z);
    const FVector End = Target->GetActorLocation();
    FCollisionQueryParams Query(SCENE_QUERY_STAT(UnitAttack), false, this);
    Query.AddIgnoredActor(Target);
    TArray<FHitResult> Hits;
    // Overlap-only traces return false but still populate Hits with combatants.
    World->LineTraceMultiByChannel(Hits, Start, End, Collision::UnitHitTarget, Query);
    for (const FHitResult& Hit : Hits)
    {
        // A blocking obstacle terminates the trace and cannot be passed through.
        if (Hit.bBlockingHit)
        {
            return false;
        }
        const AActor* Obstacle = Hit.GetActor();
        if (!IsValid(Obstacle) || Obstacle->IsA<AController>() || UInterfaceFunctionLibrary::IsActorDead(Obstacle))
        {
            continue;
        }
        const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(this, Obstacle);
        if (Attitude == ETeamAttitude::Friendly || (Attitude == ETeamAttitude::Hostile && GetAttackPattern() == EUnitAttackPattern::Ranged))
        {
            continue;
        }

        if (Attitude == ETeamAttitude::Hostile)
        {
            return false;
        }
    }
    return true;
}

bool ATowerDefenseAICharacter::CanAttack(const AActor* Target) const
{
    return CanAttackFrom(GetActorLocation(), Target);
}

bool ATowerDefenseAICharacter::IsDead() const
{
    return bIsDead;
}

void ATowerDefenseAICharacter::OnDeath()
{
    if (bIsDead)
    {
        return;
    }

    const auto DisableComponent = [](UPrimitiveComponent* Component)
    {
        Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Component->SetCollisionProfileName(Collision::NoCollisionProfile);
        Component->SetCanEverAffectNavigation(false);
    };

    bIsDead = true;
    GiveReward();
    DisableComponent(GetMesh());
    DisableComponent(GetCapsuleComponent());
    GetCharacterMovement()->DisableMovement();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::OnDeathTimerElapsed, DeathDestroyInterval);
    SetCanAffectNavigationGeneration(false, true);
    AbilitySystemComponent->CancelAllAbilities();
    if (HealthWidget.IsValid())
    {
        HealthWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
    UInterfaceFunctionLibrary::CallOnDeath(GetController());
    DeathDelegate.Broadcast();
}

FDeathDelegate& ATowerDefenseAICharacter::GetDeathDelegate()
{
    return DeathDelegate;
}

void ATowerDefenseAICharacter::BeginPlay()
{
    Super::BeginPlay();

    AbilitySystemComponent->UnitDead.BindUObject(this, &ThisClass::OnDeath);

    // Damage classification must persist after the attack montage ends (arrows may still be flying).
    if (AttackPattern == EUnitAttackPattern::Ranged)
    {
        AbilitySystemComponent->AddLooseGameplayTag(Weapon_Projectile);
    }
    else if (AttackPattern == EUnitAttackPattern::TwoHanded)
    {
        AbilitySystemComponent->AddLooseGameplayTag(Modifier_Damage_TwoHanded);
    }

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        AbilitySystemComponent->HitPointPercentChanged.BindUObject(HealthWidget.Get(), &UProgressBarWidget::SetPercent);
    }
}

void ATowerDefenseAICharacter::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
    Destroy();
}

void ATowerDefenseAICharacter::GiveReward()
{
    const float Reward = AbilitySystemComponent->GetSetChecked<UAICharacterAttributeSet>()->GetReward();
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (It->IsValid() && FGenericTeamId::GetAttitude(this, It->Get()) == ETeamAttitude::Hostile)
        {
            AbilitySystem::SendGameplayEventToInstigator(this, It->Get(), Action_Money_Change, Reward);
        }
    }
}

bool ATowerDefenseAICharacter::IsTargetInRange(const FVector& Position, const AActor* Target) const
{
    const ATowerDefenseWorldSettings* Settings = UWorldGridFunctionLibrary::GetWorldSettings(this);
    if (!IsValid(Settings))
    {
        return false;
    }

    TArray<FIntVector2> TargetCells = UInterfaceFunctionLibrary::GetOccupiedCells(Target);
    if (TargetCells.IsEmpty())
    {
        TargetCells.Add(Settings->WorldToGridLocation(Target->GetActorLocation()));
    }
    const FIntVector2 Origin = Settings->WorldToGridLocation(Position);
    const int32 Radius = GetAttackRadiusInCells();
    return Algo::AnyOf(TargetCells, [this, &Origin, &Radius](const FIntVector2& Cell) { return IsCellInAttackRange(AttackPattern, Cell - Origin, Radius); });
}
