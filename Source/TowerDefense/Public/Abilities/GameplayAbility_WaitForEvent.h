// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_WaitForEvent.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_WaitForEvent : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    TSubclassOf<UGameplayEffect> EffectOnEvent;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    FGameplayTag EventTag;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    bool bFinishOnEventReceived = false;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEventTask;

    UFUNCTION()
    void OnEventReceived(FGameplayEventData Payload);
};
