// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/Calculations/UnitsDamageCalculation.h"

#include "Abilities/TowerDefenceTags.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Utils/BaseUtils.h"

namespace
{
float RollDamage(int32 DamageBonus)
{
    // Every +7 becomes +2d damage
    const int32 ExtraDoubleDice = DamageBonus / 7;
    DamageBonus -= ExtraDoubleDice * 7;
    // +4 after all +7 becomes +1d damage
    const int32 ExtraDice = DamageBonus / 4;
    DamageBonus -= ExtraDice * 4;
    return static_cast<float>(RollUtils::RollDice(1 + 2 * ExtraDoubleDice + ExtraDice) + DamageBonus);
}

int32 DefenseFromSkill(int32 SkillLevel)
{
    // integer division, we should discard halves
    return 3 + SkillLevel / 2;
}
} // namespace

UUnitsDamageCalculation::UUnitsDamageCalculation()
{
    DamageDef.AttributeToCapture = UAICharacterAttributeSet::GetDamageAttribute();
    DamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    DexteritySourceDef.AttributeToCapture = UAICharacterAttributeSet::GetDexterityAttribute();
    DexteritySourceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    AttackRelativeSourceDef.AttributeToCapture = UAICharacterAttributeSet::GetAttackRelativeAttribute();
    AttackRelativeSourceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

    BodyArmorDef.AttributeToCapture = UAICharacterAttributeSet::GetBodyArmorAttribute();
    BodyArmorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    HeadArmorDef.AttributeToCapture = UAICharacterAttributeSet::GetHeadArmorAttribute();
    HeadArmorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    DexterityTargetDef.AttributeToCapture = UAICharacterAttributeSet::GetDexterityAttribute();
    DexterityTargetDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    AttackRelativeTargetDef.AttributeToCapture = UAICharacterAttributeSet::GetAttackRelativeAttribute();
    AttackRelativeTargetDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    BlockRelativeDef.AttributeToCapture = UAICharacterAttributeSet::GetBlockRelativeAttribute();
    BlockRelativeDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    DefenseBonusDef.AttributeToCapture = UAICharacterAttributeSet::GetDefenseBonusAttribute();
    DefenseBonusDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

    RelevantAttributesToCapture = { DamageDef,          DexteritySourceDef,      AttackRelativeSourceDef, BodyArmorDef,   HeadArmorDef,
                                    DexterityTargetDef, AttackRelativeTargetDef, BlockRelativeDef,        DefenseBonusDef };
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

    //const bool bTwoHandedAttack = Parameters.SourceTags && Parameters.SourceTags->HasTag(Modifier_Damage_TwoHanded);
    //const bool bProjectileAttack = Parameters.SourceTags && Parameters.SourceTags->HasTag(Modifier_Damage_Pojectile);

    const int32 SourceDexterity = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(DexteritySourceDef, 0.f));
    const int32 SourceSkillRel = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(AttackRelativeSourceDef, -4.f));
    const int32 SourceDamage = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(DamageDef, 0.f));
    int32 Margin = 0;
    const RollUtils::ERollResult RollResult = RollUtils::CheckSuccessMargin(Margin, SourceDexterity + SourceSkillRel);
    if (RollResult == RollUtils::Failure || RollResult == RollUtils::CriticalFailure)
    {
        return 0.f;
    }

    const float Armor = FMath::Abs(GetAttributeMagnitudeWithDefault(BodyArmorDef, 0.f));

    if (RollResult == RollUtils::CriticalSuccess)
    {
        return -FMath::Max(2.f * RollDamage(SourceDamage) - Armor, 0.f);
    }

    const int32 TargetDexterity = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(DexterityTargetDef, 0.f));
    const int32 TargetSkillRel = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(AttackRelativeTargetDef, -4.f));
    const int32 TargetDefenseBonus = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(DefenseBonusDef, 0.f));
    const int32 ParryDifficulty = DefenseFromSkill(TargetDexterity + TargetSkillRel) + TargetDefenseBonus;
    int32 DefenseDifficulty = ParryDifficulty;
    if (Parameters.TargetTags && Parameters.TargetTags->HasTag(Modifier_Damage_Shield))
    {
        const int32 BlockSkillRel = FMath::FloorToInt(GetAttributeMagnitudeWithDefault(BlockRelativeDef, -4.f));
        const int32 BlockDifficulty = DefenseFromSkill(TargetDexterity + BlockSkillRel) + TargetDefenseBonus;
        DefenseDifficulty = FMath::Max(ParryDifficulty, BlockDifficulty);
    }
    return -FMath::Max(RollUtils::IsCheckSuccessful(DefenseDifficulty) ? 0.f : RollDamage(SourceDamage) - Armor, 0.f);
}
