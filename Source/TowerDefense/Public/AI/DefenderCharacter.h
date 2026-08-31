// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Interfaces/CellPlacedInterface.h"
#include "DefenderCharacter.generated.h"

class UAttackSlotComponent;

UCLASS()
class TOWERDEFENSE_API ADefenderCharacter : public ATowerDefenseAICharacter, public ICellPlacedInterface
{
    GENERATED_BODY()

public:
    virtual void OnDeath() override;

    virtual const TArray<FIntVector2>& GetAdjacentCells() const override;
    virtual const TArray<FIntVector2>& GetOccupiedCells() const override;
    virtual FIntVector2 GetSize() const override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleInstanceOnly, Category=Defender)
    TArray<FIntVector2> OccupiedCells;

    UPROPERTY(VisibleInstanceOnly, Category=Defender)
    TArray<FIntVector2> AdjacentCells;
};
