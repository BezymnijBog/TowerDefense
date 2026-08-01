// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WayInterface.generated.h"

class UWayComponent;

UINTERFACE(MinimalAPI)
class UWayInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IWayInterface
{
    GENERATED_BODY()

public:
    virtual UWayComponent* GetWayComponent() const = 0;
};
