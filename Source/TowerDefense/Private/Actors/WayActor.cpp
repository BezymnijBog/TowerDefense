// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/WayActor.h"

#include "AI/TowerDefenseAICharacter.h"
#include "Algo/RandomShuffle.h"
#include "Components/SplineComponent.h"
#include "Components/UnitSpawnComponent.h"
#include "Components/WayComponent.h"
#include "Subsystems/WorldRoadSubsystem.h"

AWayActor::AWayActor()
{
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    SetRootComponent(SplineComponent);
}

void AWayActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    const int32 SideLanes = static_cast<int32>(MaxSpawnDistance / SpawnWidthInterval);
    if (SideLanes != SpawnComponents.Num())
    {
        ClearSpawnComponents();
        CreateSpawnComponents(SideLanes);
    }
    PositionSpawnComponents(Transform);
}

USplineComponent* AWayActor::GetWay() const
{
    return SplineComponent;
}

bool AWayActor::CanSpawnUnit(const FVector& SpawnLocation) const
{
    return (SplineComponent->FindLocationClosestToWorldLocation(SpawnLocation, ESplineCoordinateSpace::World) - SpawnLocation).SquaredLength()
        <= MaxSpawnDistance * MaxSpawnDistance;
}

void AWayActor::SetWaveDefinition(FWaveDefinition NewDefinition)
{
    WaveDefinition = MoveTemp(NewDefinition);
    Algo::RandomShuffle(WaveDefinition.SpawnGroup);
    if (FMath::IsNearlyZero(WaveDefinition.WaveStartDelay))
    {
        SetupWave();
    }
    else
    {
        GetWorldTimerManager().SetTimer(WaveTimer, this, &ThisClass::SetupWave, WaveDefinition.WaveStartDelay);
    }
}

void AWayActor::BeginPlay()
{
    Super::BeginPlay();
    if (UWorldRoadSubsystem* const RoadSubsystem = GetWorld()->GetSubsystem<UWorldRoadSubsystem>(); IsValid(RoadSubsystem))
    {
        RoadSubsystem->RegisterActor(this);
    }
    for (UUnitSpawnComponent* SpawnComponent : SpawnComponents)
    {
        SpawnComponent->UnitStartSpawn.BindUObject(this, &ThisClass::OnUnitStartedSpawn, SpawnComponent);
    }
}

void AWayActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorldRoadSubsystem* const RoadSubsystem = GetWorld()->GetSubsystem<UWorldRoadSubsystem>(); IsValid(RoadSubsystem))
    {
        RoadSubsystem->UnRegisterActor(this);
    }
    Super::EndPlay(EndPlayReason);
}

bool AWayActor::HaveAllUnitsSpawned() const
{
    return WaveDefinition.SpawnGroup.IsEmpty();
}

void AWayActor::SetupWave()
{
    const float SpawnInterval = WaveDefinition.WaveDuration * SpawnComponents.Num() / WaveDefinition.SpawnGroup.Num();
    for (UUnitSpawnComponent* SpawnComponent : SpawnComponents)
    {
        SpawnComponent->SetSpawnInterval(SpawnInterval);
        if (!WaveDefinition.SpawnGroup.IsEmpty())
        {
            SpawnComponent->EnqueueSpawn(WaveDefinition.SpawnGroup.Pop());
        }
    }
}

void AWayActor::OnUnitStartedSpawn(ATowerDefenseAICharacter* SpawnedUnit, UUnitSpawnComponent* SpawnedComponent)
{
    if (IsValid(SpawnedUnit))
    {
        SpawnedUnits.Emplace(SpawnedUnit);
        SpawnedUnit->DeathDelegate.AddUObject(this, &ThisClass::OnSpawnedUnitDead, SpawnedUnit);
        SpawnedUnit->GetWayComponent()->SetWayActor(this);
        SpawnedUnit->FinishSpawning(SpawnedComponent->GetComponentTransform());
    }

    if (!WaveDefinition.SpawnGroup.IsEmpty())
    {
        SpawnedComponent->EnqueueSpawn(WaveDefinition.SpawnGroup.Pop());
    }
}

void AWayActor::OnSpawnedUnitDead(ATowerDefenseAICharacter* DeadUnit)
{
    if (!IsValid(DeadUnit))
    {
        return;
    }

    SpawnedUnits.Remove(DeadUnit);
    if (SpawnedUnits.IsEmpty() && HaveAllUnitsSpawned())
    {
        std::ignore = WaveIsOver.ExecuteIfBound();
    }
}

void AWayActor::CreateSpawnComponents(int32 NumOfComponents)
{
    SpawnComponents.Reserve(NumOfComponents);
    for (int32 Idx = 0; Idx < NumOfComponents; ++Idx)
    {
        UUnitSpawnComponent* const NewComponent = NewObject<UUnitSpawnComponent>(this);
        NewComponent->SetCapsuleHalfHeight(SpawnerVolumeHalfHeight);
        NewComponent->SetCapsuleRadius(SpawnerVolumeRadius);
        NewComponent->SetupAttachment(SplineComponent);
        NewComponent->RegisterComponent();
        SpawnComponents.Emplace(NewComponent);
    }
}

void AWayActor::ClearSpawnComponents()
{
    for (UUnitSpawnComponent* const Component : SpawnComponents)
    {
        Component->DestroyComponent();
    }
    SpawnComponents.Empty();
}

void AWayActor::PositionSpawnComponents(const FTransform& Transform)
{
    const int32 ComponentsNum = SpawnComponents.Num();
    const FVector CentralLane = Transform.GetLocation();
    const FVector Up = Transform.GetRotation().GetAxisZ();
    const FVector LaneOffsetDir = Transform.GetRotation().GetAxisY();
    const FVector GlobalOffset = (ComponentsNum - 1) * LaneOffsetDir * SpawnWidthInterval * 0.5;
    for (int32 Idx = 0; Idx < ComponentsNum; ++Idx)
    {
        SpawnComponents[Idx]->SetWorldLocation(CentralLane + LaneOffsetDir * Idx * SpawnWidthInterval - GlobalOffset + Up * SpawnerVolumeHalfHeight);
    }
}
