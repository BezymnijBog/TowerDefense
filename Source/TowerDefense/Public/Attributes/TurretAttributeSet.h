// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeUtils.h"
#include "TurretAttributeSet.generated.h"

UCLASS()
class TOWERDEFENSE_API UTurretAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UTurretAttributeSet, Damage);
    ATTRIBUTE_ACCESSORS(UTurretAttributeSet, ShotCooldown);
    ATTRIBUTE_ACCESSORS(UTurretAttributeSet, Ammo);
    ATTRIBUTE_ACCESSORS(UTurretAttributeSet, MaxAmmo);
    ATTRIBUTE_ACCESSORS(UTurretAttributeSet, ReloadTime);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData ShotCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData Ammo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData MaxAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData ReloadTime;
};
