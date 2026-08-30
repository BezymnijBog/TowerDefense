// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/BaseBuilding.h"

#include "AbilitySystemComponent.h"
#include "AI/AttackSlot.h"
#include "Attributes/BuildingAttributeSet.h"
#include "Components/AttackSlotComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Utils/WorldGridFunctionLibrary.h"
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

    SlotsComponent = CreateDefaultSubobject<UAttackSlotComponent>(TEXT("AttackSlots"));
}

void ABaseBuilding::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SlotsComponent->SetBaseTransform(Transform);
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

TArray<FAttackSlot> ABaseBuilding::GetSlotPoints() const
{
    return SlotsComponent->GetSlots();
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

TArray<FIntVector2> ABaseBuilding::GetAdjacentCells() const
{
    return UWorldGridFunctionLibrary::GetAdjacentCells(this, GetComponentsBoundingBox());
}

TArray<FIntVector2> ABaseBuilding::GetOccupiedCells() const
{
    return UWorldGridFunctionLibrary::GetOccupiedCells(this, GetComponentsBoundingBox());
}

FIntVector2 ABaseBuilding::GetSize() const
{
    return CellsSize;
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
    const FVector VerticalOffset = FVector::UpVector * AttackSlotHeight;
    const FVector Extent = HitBox->GetScaledBoxExtent() + FVector::OneVector * AttackSlotRadius;

    const int32 SlotsAlongX = FMath::FloorToInt(Extent.X / AttackSlotRadius);
    const int32 SlotsAlongY = FMath::FloorToInt(Extent.Y / AttackSlotRadius);

    TArray<FVector> AttackSlots;
    AttackSlots.Reserve(2 * SlotsAlongX + 2 * SlotsAlongY);

    const double SlotsIntervalX = 2.0 * Extent.X / SlotsAlongX;
    const double SlotsIntervalY = 2.0 * Extent.Y / SlotsAlongY;

    for (int32 Idx = 0; Idx < SlotsAlongX; ++Idx)
    {
        AttackSlots.Emplace(FVector::RightVector * Extent.Y + FVector::ForwardVector * (Extent.X - Idx * SlotsIntervalX) + VerticalOffset);
        AttackSlots.Emplace(-FVector::RightVector * Extent.Y - FVector::ForwardVector * (Extent.X - Idx * SlotsIntervalX) + VerticalOffset);
    }

    for (int32 Idx = 0; Idx < SlotsAlongY; ++Idx)
    {
        AttackSlots.Emplace(-FVector::RightVector * (Extent.Y - Idx * SlotsIntervalY) + FVector::ForwardVector * Extent.X + VerticalOffset);
        AttackSlots.Emplace(FVector::RightVector * (Extent.Y - Idx * SlotsIntervalY) - FVector::ForwardVector * Extent.X + VerticalOffset);
    }

    SlotsComponent->InitializeSlotsLocal(AttackSlots);
}

void ABaseBuilding::OnDeathTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
    Destroy();
}
