// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Abilities/GameplayAbility_StartWave.h"

#include "Subsystems/WorldRoadSubsystem.h"

void UGameplayAbility_StartWave::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UWorldRoadSubsystem* const WaveSubsystem = GetWorld()->GetSubsystem<UWorldRoadSubsystem>();
    WaveSubsystem->WaveCleared.AddUObject(this, &ThisClass::OnWaveCleared);
    WaveSubsystem->StartWave();
}

void UGameplayAbility_StartWave::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    GetWorld()->GetSubsystem<UWorldRoadSubsystem>()->WaveCleared.Remove(ThisHandle);
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_StartWave::OnWaveCleared()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
