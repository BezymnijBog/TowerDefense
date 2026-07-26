// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_CooldownBase.h"
#include "GameplayAbility_Reload.generated.h"

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_Reload : public UGameplayAbility_CooldownBase
{
    GENERATED_BODY()

    virtual float GetWaitTime(const UTurretAttributeSet* AttributeSet) const override;

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;
};
