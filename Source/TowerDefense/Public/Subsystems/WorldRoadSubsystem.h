// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Actors/WayActor.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldRoadSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UWorldRoadSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void RegisterActor(AWayActor* WayActor);
    void UnRegisterActor(const AWayActor* WayActor);
    bool CanSpawnUnit(const FVector& SpawnPoint) const;

private:
    UPROPERTY()
    TSet<AWayActor*> RegisteredActors;
};
