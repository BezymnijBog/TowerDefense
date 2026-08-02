// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "BuildingsDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UBuildingsDamageCalculation : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UBuildingsDamageCalculation();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    FGameplayEffectAttributeCaptureDefinition DamageDef;
};
