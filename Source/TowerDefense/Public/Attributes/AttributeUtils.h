#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#define GAMEPLAYATTRIBUTE_CONST_VALUE_SETTER(PropertyName)                                \
    FORCEINLINE void Set##PropertyName(float NewVal) const                                \
    {                                                                                     \
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();         \
        if (ensure(AbilityComp))                                                          \
        {                                                                                 \
            AbilityComp->SetNumericAttributeBase(Get##PropertyName##Attribute(), NewVal); \
        };                                                                                \
    }

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_CONST_VALUE_SETTER(PropertyName)         \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
