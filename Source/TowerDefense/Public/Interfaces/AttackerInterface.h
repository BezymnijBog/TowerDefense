// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackerInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IAttackerInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual float GetAttackRange() const = 0;
};
