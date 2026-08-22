// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/BaseBuilding.h"

#include "AbilitySystemComponent.h"
#include "Algo/ForEach.h"
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

TArray<FVector> ABaseBuilding::GetSlotPoints() const
{
    return AttackSlots;
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
    InitializeSlots();

    AttributeSet = AbilitySystemComponent->GetSet<UBuildingAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBuildingAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(1.f);
    }
}

void ABaseBuilding::InitializeSlots()
{
    const FVector Origin = GetActorLocation();
    const FVector Forward = GetActorForwardVector();
    const FVector Right = GetActorRightVector();
    const FVector VerticalOffset = GetActorUpVector() * AttackSlotHeight;
    const FVector Extent = HitBox->GetScaledBoxExtent() + FVector::OneVector * AttackSlotRadius;

    const int32 SlotsAlongX = FMath::FloorToInt(Extent.X / AttackSlotRadius);
    const int32 SlotsAlongY = FMath::FloorToInt(Extent.Y / AttackSlotRadius);

    const double SlotsIntervalX = 2.0 * Extent.X / SlotsAlongX;
    const double SlotsIntervalY = 2.0 * Extent.Y / SlotsAlongY;

    for (int32 Idx = 0; Idx < SlotsAlongX; ++Idx)
    {
        AttackSlots.Emplace(Origin + Right * Extent.Y + Forward * (Extent.X - Idx * SlotsIntervalX) + VerticalOffset);
        AttackSlots.Emplace(Origin - Right * Extent.Y - Forward * (Extent.X - Idx * SlotsIntervalX) + VerticalOffset);
    }

    for (int32 Idx = 0; Idx < SlotsAlongY; ++Idx)
    {
        AttackSlots.Emplace(Origin - Right * (Extent.Y - Idx * SlotsIntervalY) + Forward * Extent.X + VerticalOffset);
        AttackSlots.Emplace(Origin + Right * (Extent.Y - Idx * SlotsIntervalY) - Forward * Extent.X + VerticalOffset);
    }

    Algo::ForEach(AttackSlots, [this](const FVector& Vec) { DrawDebugSphere(GetWorld(), Vec, AttackSlotRadius, 16, FColor::Red, false, 5); });
}

void ABaseBuilding::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
    Destroy();
}
