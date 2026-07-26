// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_ApplyEffectOnTarget.h"
#include "GameplayAbility_Shot.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_Shot : public UGameplayAbility_ApplyEffectOnTarget
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

protected:
    virtual void OnEventReceived(FGameplayEventData Payload) override;
};
