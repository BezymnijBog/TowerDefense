// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AICharacterAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UAICharacterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Health);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Health);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, MaxHealth);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Damage);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Damage);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Damage);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, CriticalChance);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(CriticalChance);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(CriticalChance);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Armor);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Armor);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Armor);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, BlockChance);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(BlockChance);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(BlockChance);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAICharacterAttributeSet, Cooldown);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Cooldown);
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Cooldown);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData CriticalChance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Armor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData BlockChance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Cooldown;
};
