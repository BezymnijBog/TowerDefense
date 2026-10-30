// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_ConstructBuilding.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/InterfaceFunctionLibrary.h"

void UGameplayAbility_ConstructBuilding::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                         const FGameplayAbilityActorInfo* ActorInfo,
                                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                                         const FGameplayEventData* TriggerEventData)
{
    if (!TriggerEventData || !CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const AActor* Target = TriggerEventData->Target.Get();
    const UAbilitySystemComponent* TargetComponent = UInterfaceFunctionLibrary::GetAbilitySystemComponent(Target);
    if (!UInterfaceFunctionLibrary::CanStartConstruction(Target) || !IsValid(TargetComponent))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    SetCostByCaller(ActorInfo->AbilitySystemComponent.Get(), -FMath::Abs(TriggerEventData->EventMagnitude));
    UInterfaceFunctionLibrary::StartConstruction(TargetComponent->GetAvatarActor());
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGameplayAbility_ConstructBuilding::SetCostByCaller(UAbilitySystemComponent* Caller, float Cost) const
{
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGameplayEffectClass);
    SpecHandle = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Action_Money_Change, Cost);
    Caller->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}
