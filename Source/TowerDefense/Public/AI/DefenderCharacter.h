// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Interfaces/AttackSlotTarget.h"
#include "DefenderCharacter.generated.h"

UCLASS()
class TOWERDEFENSE_API ADefenderCharacter : public ATowerDefenseAICharacter, public IAttackSlotTarget
{
    GENERATED_BODY()

public:
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual TArray<FVector> GetSlotPoints() const override;

protected:
    virtual void BeginPlay() override;

private:
    virtual void InitializeSlots() override;

    UPROPERTY(VisibleInstanceOnly, Category = AttackSlots)
    FQuat SlotsOrientation;

    UPROPERTY(VisibleInstanceOnly, Category = AttackSlots)
    TArray<FVector> SlotPoints;
};
