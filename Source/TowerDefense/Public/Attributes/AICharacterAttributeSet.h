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
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, Strength);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, Dexterity);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, Health);

    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, Damage);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, HitPoints);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, ExtraHitPoints);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, DeathThresholds);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, HeadArmor);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, BodyArmor);

    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, AttackRelative);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, BlockRelative);
    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, DefenseBonus);

    ATTRIBUTE_ACCESSORS_BASIC(UAICharacterAttributeSet, Reward);

protected:
    // Main attributes
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Strength = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Dexterity = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health = 10.f;

    // Secondary attributes
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Damage = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData HitPoints = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData ExtraHitPoints = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (ClampMin = "1", ClampMax = "5"))
    FGameplayAttributeData DeathThresholds = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData HeadArmor = 2.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData BodyArmor = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Cooldown = 2.f;

    // Skills
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData AttackRelative = -4.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData BlockRelative = -4.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData DefenseBonus = 0.f;

    // Reward after unit's death
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Reward = 0.f;
};
