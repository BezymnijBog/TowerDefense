// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Actors/WayActor.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldRoadSubsystem.generated.h"

class AWavesManager;

DECLARE_MULTICAST_DELEGATE(FWaveCleared);

UCLASS()
class TOWERDEFENSE_API UWorldRoadSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void RegisterActor(AWayActor* WayActor);
    void UnRegisterActor(AWayActor* WayActor);
    bool CanSpawnUnit(const FVector& SpawnPoint) const;
    void StartWave();
    bool IsWaveInProgress() const;

    FWaveCleared WaveCleared;

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void OnSpawnerWaveFinished(AWayActor* WayActor);

    UPROPERTY()
    TSet<AWayActor*> RegisteredActors;

    UPROPERTY()
    TMap<AWayActor*, bool> FinishedWaves;

    UPROPERTY()
    TWeakObjectPtr<AWavesManager> WavesManager;

    bool bIsWaveInProgress = false;
};
