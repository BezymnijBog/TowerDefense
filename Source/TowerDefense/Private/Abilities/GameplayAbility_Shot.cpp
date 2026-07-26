// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility_Shot.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Attributes/TurretAttributeSet.h"
#include "TowerDefense.h"

void UGameplayAbility_Shot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        return;
    }
}

void UGameplayAbility_Shot::OnEventReceived(FGameplayEventData Payload)
{
    Super::OnEventReceived(MoveTemp(Payload));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}