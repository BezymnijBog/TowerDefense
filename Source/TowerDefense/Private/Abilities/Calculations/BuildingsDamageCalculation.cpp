// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/Calculations/BuildingsDamageCalculation.h"

#include "Abilities/TowerDefenceTags.h"
#include "Attributes/AICharacterAttributeSet.h"

UBuildingsDamageCalculation::UBuildingsDamageCalculation()
{
    DamageDef.AttributeToCapture = UAICharacterAttributeSet::GetDamageAttribute();
    DamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    RelevantAttributesToCapture.Add(DamageDef);
}

float UBuildingsDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    FAggregatorEvaluateParameters Parameters;
    Parameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    Parameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    const float HeavyDamageFactor = Parameters.SourceTags->HasTag(Action_Damage_Heavy) ? 3.f : 1.f;
    if (float Damage; GetCapturedAttributeMagnitude(DamageDef, Spec, Parameters, Damage))
    {
        return -FMath::Abs(Damage * HeavyDamageFactor);
    }
    return 0.f;
}
