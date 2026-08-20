// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "UnitSpawnComponent.generated.h"

class ATowerDefenseAICharacter;

DECLARE_DELEGATE_OneParam(FUnitSpawned, ATowerDefenseAICharacter*);

UCLASS()
class TOWERDEFENSE_API UUnitSpawnComponent : public UCapsuleComponent
{
    GENERATED_BODY()

public:
    void EnqueueSpawn(TSubclassOf<ATowerDefenseAICharacter> NewClass);
    void SetSpawnInterval(float NewInterval);
    bool CanSpawnUnit() const;

    FUnitSpawned UnitStartSpawn;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = UnitsSpawn)
    float TimeRandomDeviation = 0.25;

    UPROPERTY(VisibleInstanceOnly, Category = UnitsSpawn)
    TSubclassOf<ATowerDefenseAICharacter> UnitClass;

private:
    void SpawnUnit();
    void OnTimerElapsed();
    void TrySpawnUnit();

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

    FTimerHandle Handle;
    TWeakObjectPtr<AActor> LastSpawnedActor;
    float SpawnInterval = 1.f;
    bool bIsOccupied = false;
};
