// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/WaveSpawnUtils.h"
#include "WayActor.generated.h"

class UUnitSpawnComponent;
class USplineComponent;

DECLARE_DELEGATE(FWaveIsOverDelegate);

UCLASS()
class TOWERDEFENSE_API AWayActor : public AActor
{
    GENERATED_BODY()

public:
    AWayActor();

    virtual void OnConstruction(const FTransform& Transform) override;

    USplineComponent* GetWay() const;

    bool CanSpawnUnit(const FVector& SpawnLocation) const;

    void SetWaveDefinition(FWaveDefinition NewDefinition);

    FWaveIsOverDelegate WaveIsOver;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<USplineComponent> SplineComponent;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TArray<UUnitSpawnComponent*> SpawnComponents;

    UPROPERTY(EditAnywhere, Category = WayActor)
    float SpawnerVolumeHalfHeight = 88.f;

    UPROPERTY(EditAnywhere, Category = WayActor)
    float SpawnerVolumeRadius = 34.f;

    UPROPERTY(EditAnywhere, Category = WayActor)
    double MaxSpawnDistance = 100.;

    UPROPERTY(EditAnywhere, Category = WayActor)
    double SpawnWidthInterval = 100.;

    UPROPERTY(EditAnywhere, Category = WayActor)
    FWaveDefinition WaveDefinition;

    UPROPERTY(VisibleInstanceOnly, Category = WayActor)
    TSet<TWeakObjectPtr<ATowerDefenseAICharacter>> SpawnedUnits;

private:
    bool HaveAllUnitsSpawned() const;
    void SetupWave();
    void OnUnitStartedSpawn(ATowerDefenseAICharacter* SpawnedUnit, UUnitSpawnComponent* SpawnedComponent);
    void OnSpawnedUnitDead(ATowerDefenseAICharacter* DeadUnit);

    void CreateSpawnComponents(int32 NumOfComponents);
    void ClearSpawnComponents();
    void PositionSpawnComponents(const FTransform& Transform);

    FTimerHandle WaveTimer;
    TArray<FTimerHandle> SpawnTimers;
    int32 CurrentLaneIdx = 0;
};
