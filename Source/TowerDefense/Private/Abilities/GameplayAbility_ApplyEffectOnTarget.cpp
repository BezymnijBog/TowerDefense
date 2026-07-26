// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Abilities/GameplayAbility_ApplyEffectOnTarget.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UGameplayAbility_ApplyEffectOnTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        return;
    }

    WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EffectTag);
    WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnEventReceived);
    WaitEventTask->Activate();
}

void UGameplayAbility_ApplyEffectOnTarget::OnEventReceived(FGameplayEventData Payload)
{
    UAbilitySystemComponent* const InstigatorAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Instigator);
    UAbilitySystemComponent* const TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Target);

    if (IsValid(InstigatorAbilitySystem) && IsValid(TargetAbilitySystem) && IsValid(EffectClass))
    {
        InstigatorAbilitySystem->ApplyGameplayEffectToTarget(EffectClass->GetDefaultObject<UGameplayEffect>(), TargetAbilitySystem);
    }
    WaitEventTask->EndTask();
}