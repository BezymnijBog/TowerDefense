// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Attack.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_Attack : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    TObjectPtr<UAnimMontage> MontageToPlay;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    float PlayRate = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    FName SectionName;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClassesWhileAnimating;

    UPROPERTY(VisibleDefaultsOnly, Category = Deprecated)
    TArray<TObjectPtr<const UGameplayEffect>> GameplayEffectsWhileAnimating;

    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> Task;

    UFUNCTION()
    void OnTaskCancelled();

    UFUNCTION()
    void OnTaskFinished();
};
