// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/TowerDefenseAICharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TowerDefense.h"
#include "Utils/BaseUtils.h"
#include "Widgets/ProgressBarWidget.h"

ATowerDefenseAICharacter::ATowerDefenseAICharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
    WidgetComponent->SetupAttachment(GetMesh());

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

void ATowerDefenseAICharacter::OnDeath()
{
    GetMesh()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCapsuleComponent()->SetCollisionProfileName(Collision::NoCollisionProfile);
    GetCharacterMovement()->DisableMovement();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::OnDeathTimerElapsed, DeathDestroyInterval);
    AbilitySystemComponent->CancelAbilityHandle(DealDamageAbility);
    HealthWidget->SetVisibility(ESlateVisibility::Collapsed);
    OnDeathDelegate.Broadcast(this);
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
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAICharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

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

void ATowerDefenseAICharacter::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
    Destroy();
}
