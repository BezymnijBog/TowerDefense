// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_StartWave.h"

#include "Subsystems/WorldRoadSubsystem.h"
#include "Utils/BaseUtils.h"

void UGameplayAbility_StartWave::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UWorldRoadSubsystem* const WaveSubsystem = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this);
    if (!IsValid(WaveSubsystem))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    ThisHandle = WaveSubsystem->WaveCleared.AddUObject(this, &ThisClass::OnWaveCleared);
    if (!WaveSubsystem->StartWave())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGameplayAbility_StartWave::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (UWorldRoadSubsystem* WaveSubsystem = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); IsValid(WaveSubsystem))
    {
        WaveSubsystem->WaveCleared.Remove(ThisHandle);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_StartWave::OnWaveCleared()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
