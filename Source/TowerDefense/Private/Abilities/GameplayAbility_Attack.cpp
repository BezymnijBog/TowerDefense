// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_Attack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, *GetName(), MontageToPlay, PlayRate, SectionName, false);
    Task->OnCancelled.AddDynamic(this, &ThisClass::OnTaskCancelled);
    Task->OnInterrupted.AddDynamic(this, &ThisClass::OnTaskCancelled);
    Task->OnCompleted.AddDynamic(this, &ThisClass::OnTaskFinished);
    Task->OnBlendOut.AddDynamic(this, &ThisClass::OnTaskFinished);
    Task->Activate();
}

void UGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility,
                                         bool bWasCancelled)
{
    if (IsValid(Task))
    {
        Task->EndTask();
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_Attack::OnTaskCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_Attack::OnTaskFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
