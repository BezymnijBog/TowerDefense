// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_WaitForEvent.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Utils/BaseUtils.h"

void UGameplayAbility_WaitForEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
    WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
    WaitEventTask->ReadyForActivation();
}

void UGameplayAbility_WaitForEvent::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility,
                                               bool bWasCancelled)
{
    AbilitySystem::EndTask(WaitEventTask);
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_WaitForEvent::OnEventReceived(FGameplayEventData Payload)
{
    UAbilitySystemComponent* const SourceComp = UInterfaceFunctionLibrary::GetAbilitySystemComponent(Payload.Instigator.Get());
    UAbilitySystemComponent* const TargetComp = UInterfaceFunctionLibrary::GetAbilitySystemComponent(Payload.Target.Get());
    if (!IsValid(SourceComp) || !IsValid(TargetComp))
    {
        return;
    }

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectOnEvent);
    SpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, EventTag, Payload.EventMagnitude);
    SourceComp->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetComp);
    if (bFinishOnEventReceived)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}
