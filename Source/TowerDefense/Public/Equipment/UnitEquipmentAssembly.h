// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Equipment/UnitEquipmentDefinition.h"
#include "UnitEquipmentAssembly.generated.h"

// Runtime equipment data retained on the assembled mesh, including in cooked builds.
UCLASS()
class TOWERDEFENSE_API UUnitEquipmentAssembly : public UAssetUserData
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category = "Equipment")
    FUnitEquipmentLoadout Loadout;
};
