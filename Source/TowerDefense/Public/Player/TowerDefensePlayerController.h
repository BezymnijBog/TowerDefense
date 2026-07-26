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

//class ADefenderBase;
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
    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> BuildClickAction;

    //UPROPERTY(VisibleAnywhere, Category = Building)
    //TObjectPtr<ADefenderBase> PreviewActor = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = Building)
    TObjectPtr<UMaterialInterface> BuildAllowedMaterial;

    UPROPERTY(EditDefaultsOnly, Category = Building)
    TObjectPtr<UMaterialInterface> BuildRejectedMaterial;

    UPROPERTY(VisibleInstanceOnly, Category = Building)
    FBuildingInfo BuildingInfo{};

    UPROPERTY(VisibleInstanceOnly, Category = Building)
    bool bIsBuildingMode = false;
#pragma endregion

private:
    void BuildPreviewActor();
    void InitializeAbilitySystem();
    void OnMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
    void UpdateBuildingPreview() const;

    FVector CachedDestination = FVector::ZeroVector;

    bool bIsTouch = false;
    float FollowTime = 0.f;
};
