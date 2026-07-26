// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "TowerAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UTowerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTowerAttributeSet, Health)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UTowerAttributeSet, MaxHealth)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth)

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData MaxHealth;
};
