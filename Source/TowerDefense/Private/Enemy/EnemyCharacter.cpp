// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Enemy/EnemyCharacter.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/EnemyAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TowerDefense.h"
#include "Widgets/ProgressBarWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
    WidgetComponent->SetupAttachment(GetMesh());

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
}

FGenericTeamId AEnemyCharacter::GetGenericTeamId() const
{
    return TeamId;
}

void AEnemyCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UKismetSystemLibrary::PrintString(
        this, FString::Printf(TEXT("%s HP is %f"), *GetName(), AttributeSet->GetHealth()), true, false, FColor::Red, DeltaSeconds, *GetName());
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    EnemyController = NewController && NewController->IsA<AEnemyController>() ? StaticCast<AEnemyController*>(NewController) : nullptr;
}

void AEnemyCharacter::OnDeath()
{
    GetMesh()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCapsuleComponent()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCharacterMovement()->DisableMovement();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::OnDeathTimerElapsed, DeathDestroyInterval);
    AbilitySystemComponent->CancelAbilityHandle(DealDamageAbility);
    HealthWidget->SetVisibility(ESlateVisibility::Collapsed);

    OnDeathDelegate.Broadcast(this);

    for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AbilitySystem::SendGameplayEventToTarget(this, It->Get(), Action_Money_Receive);
    }
}

void AEnemyCharacter::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
    if (HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(OnAttributeChangeData.NewValue / AttributeSet->GetMaxHealth());
    }

    if (OnAttributeChangeData.NewValue <= 0.f)
    {
        OnDeath();
    }
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    AttributeSet = AbilitySystemComponent->GetSet<UEnemyAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEnemyAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

    DealDamageAbility = AbilitySystemComponent->GiveAbility(DealDamageAbilityClass);
    if (!ensure(AbilitySystemComponent->TryActivateAbility(DealDamageAbility)))
    {
        UE_LOG(LogTowerDefense, Warning, TEXT("Failed to activate ability for %s"), *GetName());
    }

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        HealthWidget->SetPercent(1.f);
    }
}

void AEnemyCharacter::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
    Destroy();
}
