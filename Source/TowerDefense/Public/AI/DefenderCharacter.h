// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Interfaces/AttackSlotTarget.h"
#include "DefenderCharacter.generated.h"

class UAttackSlotComponent;

UCLASS()
class TOWERDEFENSE_API ADefenderCharacter : public ATowerDefenseAICharacter, public IAttackSlotTarget
{
    GENERATED_BODY()

public:
    explicit ADefenderCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual TArray<FAttackSlot> GetSlotPoints() const override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<UAttackSlotComponent> SlotsComponent;

private:
    virtual void InitializeSlots() override;
};
