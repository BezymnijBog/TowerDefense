// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AICharacterAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UAICharacterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Health);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, MaxHealth);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Damage);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Damage);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Armor);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Armor);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Cooldown);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Cooldown);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Armor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Cooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData CriticalChance;
};
