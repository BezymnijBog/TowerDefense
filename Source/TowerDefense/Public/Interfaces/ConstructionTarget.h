// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConstructionTarget.generated.h"

UINTERFACE()
class UConstructionTarget : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IConstructionTarget : public IInterface
{
    GENERATED_BODY()

public:
    virtual bool CanStartConstruction() const = 0;
    virtual void StartConstruction() = 0;
};
