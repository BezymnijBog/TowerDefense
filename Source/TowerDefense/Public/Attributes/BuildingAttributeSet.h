// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BuildingAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UBuildingAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBuildingAttributeSet, Health)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBuildingAttributeSet, MaxHealth)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth)

    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBuildingAttributeSet, ConstructionCost)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(ConstructionCost)

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Construction", meta = (ClampMin = "0"))
    FGameplayAttributeData ConstructionCost = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData MaxHealth;
};
