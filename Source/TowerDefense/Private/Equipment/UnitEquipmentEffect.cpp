// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Equipment/UnitEquipmentEffect.h"

#include "Attributes/AICharacterAttributeSet.h"
#include "UObject/UnrealType.h"

UUnitEquipmentEffect::UUnitEquipmentEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    for (TFieldIterator<FStructProperty> It(UAICharacterAttributeSet::StaticClass()); It; ++It)
    {
        if (It->Struct != FGameplayAttributeData::StaticStruct() || FGameplayAttribute(*It) == UAICharacterAttributeSet::GetHitPointsAttribute())
        {
            continue;
        }
        FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();
        Modifier.Attribute = FGameplayAttribute(*It);
        Modifier.ModifierOp = EGameplayModOp::Additive;
        FSetByCallerFloat Magnitude;
        Magnitude.DataName = It->GetFName();
        Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
    }
}
