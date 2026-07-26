// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "EnemyAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UEnemyAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UEnemyAttributeSet, Health);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UEnemyAttributeSet, MaxHealth);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UEnemyAttributeSet, MoneyReward);
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MoneyReward);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData MaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData MoneyReward;
};
