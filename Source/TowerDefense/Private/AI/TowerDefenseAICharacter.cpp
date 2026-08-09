// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/TowerDefenseAICharacter.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/WayComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Utils/BaseUtils.h"
#include "Widgets/ProgressBarWidget.h"

ATowerDefenseAICharacter::ATowerDefenseAICharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
    WidgetComponent->SetupAttachment(GetMesh());

    WayComponent = CreateDefaultSubobject<UWayComponent>(TEXT("WayComponent"));
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
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

bool ATowerDefenseAICharacter::IsDead() const
{
    return bIsDead;
}

void ATowerDefenseAICharacter::OnDeath()
{
    bIsDead = true;
    GiveReward();
    GetMesh()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCapsuleComponent()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCharacterMovement()->DisableMovement();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::OnDeathTimerElapsed, DeathDestroyInterval);
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

void ATowerDefenseAICharacter::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
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

void ATowerDefenseAICharacter::BeginPlay()
{
    Super::BeginPlay();

    AttributeSet = AbilitySystemComponent->GetSet<UAICharacterAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetHealthAttribute())
        .AddUObject(this, &ThisClass::OnHealthChanged);

    for (const auto& [Ability, bActivateAtStartup] : DefaultAbilities)
    {
        FGameplayAbilitySpecHandle AbilitySpec = AbilitySystemComponent->GiveAbility(Ability);
        if (bActivateAtStartup)
        {
            AbilitySystemComponent->TryActivateAbility(MoveTemp(AbilitySpec));
        }
    }

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        HealthWidget->SetPercent(1.f);
    }
}

void ATowerDefenseAICharacter::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
    Destroy();
}

void ATowerDefenseAICharacter::GiveReward()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (It->IsValid() && FGenericTeamId::GetAttitude(this, It->Get()) == ETeamAttitude::Hostile)
        {
            AbilitySystem::SendGameplayEventToInstigator(this, It->Get(), Action_Money_Receive, AttributeSet->GetReward());
        }
    }
}
