// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/WaveSpawnUtils.h"
#include "WavesManager.generated.h"

class AWayActor;
enum class EWaveType;
class UWaveSetTemplateDataAsset;

UCLASS()
class TOWERDEFENSE_API AWavesManager : public AActor
{
    GENERATED_BODY()

public:
    AWavesManager();

    bool StartWave();
    bool PrepareWave(EWaveType WaveType, float Difficulty = 1.f);
    bool CanStartWave() const;
    bool IsWaveActive() const { return bWaveActive; }
    void AddSpawner(AWayActor* Spawner);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void OnWaveCleared();
    void ResolveVictory();
    void OnDefendedBuildingLost();
    bool bWavePrepared = false;
    bool bWaveActive = false;

    // Assign the settlement centre placed on this map.
    UPROPERTY(EditInstanceOnly, Category = WavesManager)
    TObjectPtr<class ABaseBuilding> DefendedBuilding;

    UPROPERTY(EditDefaultsOnly, Category = WavesManager)
    TMap<EWaveType, UWaveSetTemplateDataAsset*> WaveDataAssets;

    UPROPERTY(VisibleInstanceOnly, Category = WavesManager)
    TSet<TWeakObjectPtr<AWayActor>> WaveSpawners;

    UPROPERTY(VisibleInstanceOnly, Category = WavesManager)
    FWaveDefinition NextWaveDefinition;

    UPROPERTY(EditAnywhere, Category = WavesManager)
    EWaveType NextWaveType;
};
