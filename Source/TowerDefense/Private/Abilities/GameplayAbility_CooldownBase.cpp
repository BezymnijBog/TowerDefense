// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_CooldownBase.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Attributes/TurretAttributeSet.h"

void UGameplayAbility_CooldownBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return;
    }
    const TStrongObjectPtr<UAbilitySystemComponent> AbilitySystem = ActorInfo->AbilitySystemComponent.Pin();
    AbilitySystem->RemoveLooseGameplayTag(PreCooldownTag);
    if (const UTurretAttributeSet* const TurretAttributes = AbilitySystem->GetSet<UTurretAttributeSet>(); IsValid(TurretAttributes))
    {
        ParentComponent = ActorInfo->AbilitySystemComponent;
        InitWaitTask(GetWaitTime(TurretAttributes));
    }
}

void UGameplayAbility_CooldownBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility,
                                               bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        const TStrongObjectPtr<UAbilitySystemComponent> AbilitySystem = ActorInfo->AbilitySystemComponent.Pin();
        AbilitySystem->AddLooseGameplayTag(PostCooldownTag);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_CooldownBase::Finish()
{
    WaitTask->EndTask();
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_CooldownBase::InitWaitTask(float WaitTime)
{
    WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, WaitTime);
    WaitTask->OnFinish.AddUniqueDynamic(this, &ThisClass::Finish);
    WaitTask->Activate();
}

float UGameplayAbility_CooldownBase::GetWaitTime(const UTurretAttributeSet* AttributeSet) const
{
    checkf(false, TEXT("Use of unimplemented UGameplayAbility_CooldownBase::GetWaitTime"));
    return 0.f;
}
