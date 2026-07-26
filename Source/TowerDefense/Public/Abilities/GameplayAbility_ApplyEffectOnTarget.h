// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_ApplyEffectOnTarget.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_ApplyEffectOnTarget : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

protected:
    UFUNCTION()
    virtual void OnEventReceived(FGameplayEventData Payload);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ability|Tasks")
    TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEventTask;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    TSubclassOf<UGameplayEffect> EffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Effects")
    FGameplayTag EffectTag;
};
