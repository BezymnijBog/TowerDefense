// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/BaseBuilding.h"

#include "AbilitySystemComponent.h"
#include "Attributes/BuildingAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/BuildingConstructionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Subsystems/WorldGridSubsystem.h"
#include "Utils/BaseUtils.h"
#include "Utils/WorldGridFunctionLibrary.h"
#include "Widgets/BuildingUpgradeWidget.h"
#include "Widgets/ProgressBarWidget.h"

ABaseBuilding::ABaseBuilding()
{
    TeamId = Teams::TeamPlayer;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Construction = CreateDefaultSubobject<UBuildingConstructionComponent>(TEXT("Construction"));

    UpgradeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("UpgradeWidget"));
    UpgradeWidget->SetupAttachment(Mesh);
    UpgradeWidget->SetWidgetSpace(EWidgetSpace::Screen);
    UpgradeWidget->SetDrawAtDesiredSize(true);
    UpgradeWidget->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
    UpgradeWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(Mesh);

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    WidgetComponent->SetupAttachment(Mesh);

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    HitBox->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Overlap);
    Mesh->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Ignore);
    WidgetComponent->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Ignore);
}

void ABaseBuilding::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    TInlineComponentArray<UPrimitiveComponent*> Components(this);
    for (UPrimitiveComponent* Component : Components)
    {
        Component->SetCollisionResponseToChannel(Collision::UnitHitTarget, Component == HitBox ? ECR_Overlap : ECR_Ignore);
    }
    bool bQuery, bPhysics, bProbe;
    CollisionEnabledToFlags(HitBox->GetCollisionEnabled(), bQuery, bPhysics, bProbe);
    if (!bQuery)
    {
        HitBox->SetCollisionEnabled(CollisionEnabledFromFlags(true, bPhysics, bProbe));
    }
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
    if (bIsDead)
    {
        return;
    }
    ApplyConstructionState(false, Construction->GetDestroyedMesh());
    OnBuildingDestroyed();
    Construction->CancelConstruction();
    DeathDelegate.Broadcast();
}

FDeathDelegate& ABaseBuilding::GetDeathDelegate()
{
    return DeathDelegate;
}

const TArray<FIntVector2>& ABaseBuilding::GetAdjacentCells() const
{
    return AdjacentCells;
}

const TArray<FIntVector2>& ABaseBuilding::GetOccupiedCells() const
{
    return OccupiedCells;
}

FIntVector2 ABaseBuilding::GetSize() const
{
    return CellsSize;
}

void ABaseBuilding::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
    if (HealthWidget.IsValid() && IsValid(AttributeSet))
    {
        const float MaxHealth = AttributeSet->GetMaxHealth();
        HealthWidget->SetPercent(MaxHealth > 0.f ? FMath::Clamp(OnAttributeChangeData.NewValue / MaxHealth, 0.f, 1.f) : 0.f);
    }

    if (OnAttributeChangeData.NewValue <= 0.f)
    {
        OnDeath();
    }
}

void ABaseBuilding::BeginPlay()
{
    Super::BeginPlay();

    Construction->ApplyBuildingState.BindUObject(this, &ThisClass::ApplyConstructionState);
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetStaticMesh(Construction->GetStartingMesh());

    OccupiedCells = UWorldGridFunctionLibrary::GetOccupiedCells(this, GetComponentsBoundingBox());
    AdjacentCells = UWorldGridFunctionLibrary::GetAdjacentCells(this, GetComponentsBoundingBox());

    if (UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this); IsValid(Grid))
    {
        Grid->RegisterActor(this);
    }

    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    AttributeSet = AbilitySystemComponent->GetSet<UBuildingAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBuildingAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

    if (UBuildingUpgradeWidget* Widget = Cast<UBuildingUpgradeWidget>(UpgradeWidget->GetWidget()); IsValid(Widget))
    {
        Widget->ConstructionTarget.BindWeakLambda(this, [this] { return this; });
        Widget->ConstructionCost.BindWeakLambda(this, [this] { return AttributeSet->GetConstructionCost(); });
    }

    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(1.f);
    }
}

void ABaseBuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this); IsValid(Grid))
    {
        Grid->UnRegisterActor(this);
    }
    Construction->ApplyBuildingState.Unbind();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBuildingAttributeSet::GetHealthAttribute()).RemoveAll(this);
    Super::EndPlay(EndPlayReason);
}

void ABaseBuilding::ApplyConstructionState(bool bActive, UStaticMesh* StageMesh)
{
    const bool bWasDead = bIsDead;
    bIsDead = !bActive;
    Mesh->SetStaticMesh(StageMesh);
    // Ruins keep their footprint and physical collision until the actor leaves the world.
    WidgetComponent->SetVisibility(bActive);
    if (UUserWidget* Widget = WidgetComponent->GetWidget(); IsValid(Widget))
    {
        Widget->SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
    }
    if (!bActive)
    {
        StimuliSource->UnregisterFromPerceptionSystem();
        return;
    }
    if (bWasDead)
    {
        if (IsValid(AttributeSet))
        {
            AbilitySystemComponent->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), AttributeSet->GetMaxHealth());
        }
        StimuliSource->RegisterWithPerceptionSystem();
    }
}

void ABaseBuilding::StartConstruction()
{
    Construction->StartConstruction();
}

bool ABaseBuilding::CanStartConstruction() const
{
    return Construction->CanStartConstruction();
}
