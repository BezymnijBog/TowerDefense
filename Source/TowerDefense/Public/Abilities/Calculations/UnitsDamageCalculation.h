// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "UnitsDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UUnitsDamageCalculation : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UUnitsDamageCalculation();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    UPROPERTY(EditAnywhere, Category = DamageCalculation, meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
    float BaseDamageDeviation = 0.f;

    FGameplayEffectAttributeCaptureDefinition DamageDef;
    FGameplayEffectAttributeCaptureDefinition CriticalChanceDef;
    FGameplayEffectAttributeCaptureDefinition ArmorDef;
    FGameplayEffectAttributeCaptureDefinition BlockChanceDef;

    float GetDeviationModifier() const;
};
