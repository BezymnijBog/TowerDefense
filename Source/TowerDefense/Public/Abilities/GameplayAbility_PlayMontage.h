// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_PlayMontage.generated.h"

class UAbilityTask_PlayMontageAndWait;

UENUM(BlueprintType)
enum class EFinishCondition : uint8
{
    OnMontageEnd = 0,
    OnEventReceived
};

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_PlayMontage : public UGameplayAbility
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
    TObjectPtr<UAnimMontage> MontageToPlay;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    float PlayRate = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    FName SectionName;

    UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClassesWhileAnimating;

    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    UFUNCTION()
    void OnMontageCancelled();

    UFUNCTION()
    void OnMontageFinished();
};
