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
    FGameplayEffectAttributeCaptureDefinition DamageDef;
    FGameplayEffectAttributeCaptureDefinition DexteritySourceDef;
    FGameplayEffectAttributeCaptureDefinition AttackRelativeSourceDef;

    FGameplayEffectAttributeCaptureDefinition BodyArmorDef;
    FGameplayEffectAttributeCaptureDefinition HeadArmorDef;
    FGameplayEffectAttributeCaptureDefinition DexterityTargetDef;
    FGameplayEffectAttributeCaptureDefinition AttackRelativeTargetDef;
    FGameplayEffectAttributeCaptureDefinition BlockRelativeDef;
    FGameplayEffectAttributeCaptureDefinition DefenseBonusDef;
};
