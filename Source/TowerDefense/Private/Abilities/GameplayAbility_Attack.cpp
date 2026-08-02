// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_Attack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AICharacterAttributeSet.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Utils/BaseUtils.h"

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

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, *GetName(), MontageToPlay, PlayRate, SectionName, false);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
    MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
    MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
    MontageTask->Activate();

    WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Action_Damage_Take);
    WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
    WaitEventTask->Activate();
}

void UGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility,
                                         bool bWasCancelled)
{
    AbilitySystem::EndTask(MontageTask);
    AbilitySystem::EndTask(WaitEventTask);
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_Attack::OnMontageCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_Attack::OnMontageFinished()
{
    if (FinishCondition == EFinishCondition::OnMontageEnd)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UGameplayAbility_Attack::OnEventReceived(FGameplayEventData Payload)
{
    UAbilitySystemComponent* const SourceComp = UInterfaceFunctionLibrary::GetAbilitySystemComponent(Payload.Instigator.Get());
    UAbilitySystemComponent* const TargetComp = UInterfaceFunctionLibrary::GetAbilitySystemComponent(Payload.Target.Get());
    if (!IsValid(SourceComp) || !IsValid(TargetComp))
    {
        return;
    }

    SourceComp->ApplyGameplayEffectToTarget(DamageEffect->GetDefaultObject<UGameplayEffect>(), TargetComp);
    if (FinishCondition == EFinishCondition::OnEventReceived)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}
