// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DeathInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDeathInterface : public UInterface
{
    GENERATED_BODY()
};

DECLARE_MULTICAST_DELEGATE(FDeathDelegate);

class TOWERDEFENSE_API IDeathInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual bool IsDead() const = 0;
    virtual void OnDeath() = 0;
    virtual FDeathDelegate& GetDeathDelegate() = 0;
};
