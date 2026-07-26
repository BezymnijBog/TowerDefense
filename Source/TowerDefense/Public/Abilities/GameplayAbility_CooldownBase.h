// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_CooldownBase.generated.h"

class UAbilityTask_WaitDelay;
class UTurretAttributeSet;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_CooldownBase : public UGameplayAbility
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
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TWeakObjectPtr<UAbilitySystemComponent> ParentComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UAbilityTask_WaitDelay> WaitTask;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
    FGameplayTag PreCooldownTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
    FGameplayTag PostCooldownTag;

private:
    UFUNCTION()
    void Finish();

    void InitWaitTask(float WaitTime);

    virtual float GetWaitTime(const UTurretAttributeSet* AttributeSet) const;
};
