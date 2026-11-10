// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UnitEquipmentEffect.generated.h"

// One persistent effect per unit; attribute names identify additive SetByCaller bonuses.
UCLASS()
class TOWERDEFENSE_API UUnitEquipmentEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UUnitEquipmentEffect();
};
