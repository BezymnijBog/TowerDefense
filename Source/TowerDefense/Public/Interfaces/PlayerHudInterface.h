// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerHudInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlayerHudInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IPlayerHudInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual void OnAddedToViewport() = 0;
};
