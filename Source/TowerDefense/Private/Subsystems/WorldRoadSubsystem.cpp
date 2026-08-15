// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/WorldRoadSubsystem.h"

#include "Algo/AnyOf.h"

void UWorldRoadSubsystem::RegisterActor(AWayActor* WayActor)
{
    RegisteredActors.Emplace(WayActor);
}

void UWorldRoadSubsystem::UnRegisterActor(const AWayActor* WayActor)
{
    RegisteredActors.Remove(WayActor);
}

bool UWorldRoadSubsystem::CanSpawnUnit(const FVector& SpawnPoint) const
{
    return Algo::AnyOf(RegisteredActors, std::bind(&AWayActor::CanSpawnUnit, std::placeholders::_1, SpawnPoint));
}
