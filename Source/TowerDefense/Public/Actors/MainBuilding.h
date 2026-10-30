// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseBuilding.h"
#include "MainBuilding.generated.h"

UCLASS()
class TOWERDEFENSE_API AMainBuilding : public ABaseBuilding
{
    GENERATED_BODY()

protected:
    virtual void OnBuildingDestroyed() override;
};
