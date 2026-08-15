// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InputActionValue.h"
#include "GameplayAbility_HireUnit.generated.h"

class ATowerDefenseAICharacter;
class AUnitPreview;
class UAbilityTask_TraceCursor;
class UAbilityTask_WaitGameplayEvent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class TOWERDEFENSE_API UGameplayAbility_HireUnit : public UGameplayAbility
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
    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UInputMappingContext> HireInputContext;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UInputAction> ConfirmHire;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UInputAction> CancelHire;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UInputAction> RotatePreview;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TSubclassOf<UGameplayEffect> HireCostEffect;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    FVector GridSize = FVector::OneVector * UE_M_TO_CM;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    FVector GridOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TSubclassOf<AUnitPreview> PreviewActorClass;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UMaterialInterface> AllowSpawnMaterial;

    UPROPERTY(EditDefaultsOnly, Category = GameplayAbilityInput)
    TObjectPtr<UMaterialInterface> ForbidSpawnMaterial;

private:
    UFUNCTION()
    void UpdateHireUnit(FGameplayEventData Payload);
    void OnHireConfirmed();
    void OnHireCancelled();
    void RotatePreviewMesh(const FInputActionValue& RotationValue);
    void OnPositionChanged(const FHitResult& HitResult);
    void ActivateInputContext(const APlayerController* Controller);
    void DeactivateInputContext(const APlayerController* Controller) const;
    void UpdatePreviewMaterial();
    bool HasEnoughResources() const;

    UPROPERTY()
    TSubclassOf<ATowerDefenseAICharacter> HireClass;

    UPROPERTY()
    TObjectPtr<AUnitPreview> PreviewActor;

    UPROPERTY()
    TObjectPtr<UAbilityTask_TraceCursor> CursorTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitUpdateTask;

    FVector MeshRelativeLocation = FVector::ZeroVector;
    float HireCost = 0;
    bool bCanSpawn = true;
};
