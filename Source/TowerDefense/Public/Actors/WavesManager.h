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

    void StartWave();
    void PrepareNextWave();
    void AddSpawner(AWayActor* Spawner);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = WavesManager)
    TMap<EWaveType, UWaveSetTemplateDataAsset*> WaveDataAssets;

    UPROPERTY(VisibleInstanceOnly, Category = WavesManager)
    TSet<TWeakObjectPtr<AWayActor>> WaveSpawners;

    UPROPERTY(VisibleInstanceOnly, Category = WavesManager)
    FWaveDefinition NextWaveDefinition;

    UPROPERTY(EditAnywhere, Category = WavesManager)
    EWaveType NextWaveType;
};
