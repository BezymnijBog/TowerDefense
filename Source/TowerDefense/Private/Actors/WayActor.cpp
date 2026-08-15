// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/WayActor.h"

#include "Components/SplineComponent.h"
#include "Subsystems/WorldRoadSubsystem.h"

AWayActor::AWayActor()
{
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    SetRootComponent(SplineComponent);
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

void AWayActor::BeginPlay()
{
    Super::BeginPlay();
    if (UWorldRoadSubsystem* const RoadSubsystem = GetWorld()->GetSubsystem<UWorldRoadSubsystem>(); IsValid(RoadSubsystem))
    {
        RoadSubsystem->RegisterActor(this);
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
