// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_CooldownBase.h"
#include "GameplayAbility_Cooldown.generated.h"

class UAbilityTask_WaitDelay;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_Cooldown : public UGameplayAbility_CooldownBase
{
    GENERATED_BODY()

    virtual float GetWaitTime(const UTurretAttributeSet* AttributeSet) const override;
};
