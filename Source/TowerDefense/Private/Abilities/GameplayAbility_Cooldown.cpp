// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility_Cooldown.h"

#include "Attributes/TurretAttributeSet.h"

float UGameplayAbility_Cooldown::GetWaitTime(const UTurretAttributeSet* AttributeSet) const
{
    return AttributeSet->GetShotCooldown();
}