// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/BaseBuilding.h"

#include "AbilitySystemComponent.h"
#include "Attributes/BuildingAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/ProgressBarWidget.h"

ABaseBuilding::ABaseBuilding()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(Mesh);

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    WidgetComponent->SetupAttachment(Mesh);

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
}

FGenericTeamId ABaseBuilding::GetGenericTeamId() const
{
    return TeamId;
}

void ABaseBuilding::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

UAbilitySystemComponent* ABaseBuilding::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

bool ABaseBuilding::IsDead() const
{
    return bIsDead;
}

void ABaseBuilding::OnDeath()
{
    bIsDead = true;
    Mesh->SetStaticMesh(nullptr);
    if (UUserWidget* const Widget = WidgetComponent->GetWidget(); IsValid(Widget))
    {
        Widget->SetVisibility(ESlateVisibility::Collapsed);
    }
    GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &ThisClass::OnDeathTimerElapsed, TimeBeforeDestroy);
}

FDeathDelegate& ABaseBuilding::GetDeathDelegate()
{
    return DeathDelegate;
}

void ABaseBuilding::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
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

void ABaseBuilding::BeginPlay()
{
    Super::BeginPlay();

    AttributeSet = AbilitySystemComponent->GetSet<UBuildingAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBuildingAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(1.f);
    }
}

void ABaseBuilding::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
    Destroy();
}
