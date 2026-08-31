// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Interfaces/AttackSlotTarget.h"
#include "Interfaces/CellPlacedInterface.h"
#include "DefenderCharacter.generated.h"

class UAttackSlotComponent;

UCLASS()
class TOWERDEFENSE_API ADefenderCharacter : public ATowerDefenseAICharacter, public IAttackSlotTarget, public ICellPlacedInterface
{
    GENERATED_BODY()

public:
    explicit ADefenderCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual TArray<FAttackSlot> GetSlotPoints() const override;

    virtual TArray<FIntVector2> GetAdjacentCells() const override;
    virtual TArray<FIntVector2> GetOccupiedCells() const override;
    virtual FIntVector2 GetSize() const override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<UAttackSlotComponent> SlotsComponent;

private:
    virtual void InitializeSlots() override;
};
