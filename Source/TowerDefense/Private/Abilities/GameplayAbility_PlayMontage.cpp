// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_PlayMontage.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Utils/BaseUtils.h"

void UGameplayAbility_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, *GetName(), MontageToPlay, PlayRate, SectionName, false);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
    MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
    MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
    MontageTask->Activate();
}

void UGameplayAbility_PlayMontage::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              bool bReplicateEndAbility,
                                              bool bWasCancelled)
{
    AbilitySystem::EndTask(MontageTask);
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_PlayMontage::OnMontageCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_PlayMontage::OnMontageFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
