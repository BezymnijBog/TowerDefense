// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Utils/AbilityDescription.h"


FGameplayAbilitySpec FDefaultAbilityDescription::ToSpec() const
{
    return FGameplayAbilitySpec(Ability, Level, InputID);
}
