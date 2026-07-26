// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility_Reload.h"

#include "Attributes/TurretAttributeSet.h"
#include "TowerDefense.h"

float UGameplayAbility_Reload::GetWaitTime(const UTurretAttributeSet* AttributeSet) const
{
    return AttributeSet->GetReloadTime();
}
void UGameplayAbility_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UE_LOG(LogTowerDefense, Display, TEXT("Reload started..."));
}

void UGameplayAbility_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility,
                                         bool bWasCancelled)
{
    const TStrongObjectPtr<UAbilitySystemComponent> AbilitySystem = ParentComponent.Pin();
    if (const UTurretAttributeSet* const AttributeSet = AbilitySystem->GetSet<UTurretAttributeSet>(); IsValid(AttributeSet) && !bWasCancelled)
    {
        AttributeSet->SetAmmo(AttributeSet->GetMaxAmmo());
        UE_LOG(LogTowerDefense, Display, TEXT("Reload finished, ammo: %f/%f"), AttributeSet->GetAmmo(), AttributeSet->GetMaxAmmo());
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
