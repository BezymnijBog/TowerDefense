// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackSlotTarget.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackSlotTarget : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IAttackSlotTarget : public IInterface
{
    GENERATED_BODY()

public:
    virtual TArray<FVector> GetSlotPoints() const;

private:
    virtual void InitializeSlots() = 0;
};
