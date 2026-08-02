// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/Calculations/UnitsDamageCalculation.h"

#include "Attributes/AICharacterAttributeSet.h"

bool IsChanceSucceeded(float Chance)
{
    return FMath::FRand() <= Chance;
}

int32 TimesChanceSucceeded(float Chance)
{
    int32 Result = 0;
    while (IsChanceSucceeded(Chance))
    {
        ++Result;
        --Chance;
    }
    return Result;
}

UUnitsDamageCalculation::UUnitsDamageCalculation()
{
    DamageDef.AttributeToCapture = UAICharacterAttributeSet::GetDamageAttribute();
    DamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    CriticalChanceDef.AttributeToCapture = UAICharacterAttributeSet::GetCriticalChanceAttribute();
    CriticalChanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    ArmorDef.AttributeToCapture = UAICharacterAttributeSet::GetArmorAttribute();
    ArmorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    BlockChanceDef.AttributeToCapture = UAICharacterAttributeSet::GetBlockChanceAttribute();
    BlockChanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    RelevantAttributesToCapture.Add(DamageDef);
    RelevantAttributesToCapture.Add(CriticalChanceDef);
    RelevantAttributesToCapture.Add(ArmorDef);
    RelevantAttributesToCapture.Add(BlockChanceDef);
}

float UUnitsDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    FAggregatorEvaluateParameters Parameters;
    Parameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    Parameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    const auto GetAttributeMagnitudeWithDefault = [this, &Spec, &Parameters](const FGameplayEffectAttributeCaptureDefinition& Definition, float Default)
    {
        float Value;
        return GetCapturedAttributeMagnitude(Definition, Spec, Parameters, Value) ? Value : Default;
    };

    if (const float BlockChance = GetAttributeMagnitudeWithDefault(BlockChanceDef, 0.f); IsChanceSucceeded(BlockChance))
    {
        return 0.f; // damage blocked
    }

    const float CriticalChance = GetAttributeMagnitudeWithDefault(CriticalChanceDef, 0.f);
    const float BaseDamage = FMath::Abs(GetAttributeMagnitudeWithDefault(DamageDef, 0.f));
    const float Armor = FMath::Abs(GetAttributeMagnitudeWithDefault(ArmorDef, 0.f));
    return -FMath::Max(BaseDamage * GetDeviationModifier() * (TimesChanceSucceeded(CriticalChance) + 1) - Armor, 0.f);
}

float UUnitsDamageCalculation::GetDeviationModifier() const
{
    return FMath::FRandRange(1.f - BaseDamageDeviation, 1.f + BaseDamageDeviation);
}
