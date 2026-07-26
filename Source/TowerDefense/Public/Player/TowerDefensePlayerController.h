// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "Templates/SubclassOf.h"
#include "Utils/BaseUtils.h"
#include "TowerDefensePlayerController.generated.h"

class ATurret;
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;
class UPlayerAttributeSet;

UCLASS()
class TOWERDEFENSE_API ATowerDefensePlayerController : public APlayerController, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATowerDefensePlayerController();

    void SetBuildingMode(FBuildingInfo BuildingInfo);

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

#pragma region Input
    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold = 0.3f;

    UPROPERTY(EditAnywhere, Category = Input)
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> SetLocationClickAction;

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> SetLocationTouchAction;
#pragma endregion

#pragma region GameplayAbilitySystem
    UPROPERTY(VisibleDefaultsOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, Category = "Abilities")
    TObjectPtr<const UPlayerAttributeSet> PlayerAttributes;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilityClasses;

    UPROPERTY(VisibleInstanceOnly, Category = "Abilities")
    TArray<FGameplayAbilitySpecHandle> GivenAbilities;
#pragma endregion

#pragma region Building
    UPROPERTY(VisibleAnywhere, Category = Building)
    TObjectPtr<ATurret> PreviewActor = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = Building)
    TObjectPtr<UMaterialInterface> BuildAllowedMaterial;

    UPROPERTY(EditDefaultsOnly, Category = Building)
    TObjectPtr<UMaterialInterface> BuildRejectedMaterial;

    UPROPERTY(VisibleInstanceOnly, Category = Building)
    FBuildingInfo BuildingInfo{};

    UPROPERTY(VisibleInstanceOnly, Category = Building)
    bool bIsBuildingMode = false;
#pragma endregion

#pragma region InputHandlers
    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();
    void OnTouchTriggered();
    void OnTouchReleased();
#pragma endregion

private:
    void InitializeAbilitySystem();
    void OnMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
    void UpdateBuildingPreview() const;

    FVector CachedDestination = FVector::ZeroVector;

    bool bIsTouch = false;
    float FollowTime = 0.f;
};
