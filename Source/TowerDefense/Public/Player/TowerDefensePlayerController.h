// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Templates/SubclassOf.h"
#include "Utils/BaseUtils.h"
#include "TowerDefensePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPlayerAbilitySystemComponent;

UCLASS()
class TOWERDEFENSE_API ATowerDefensePlayerController : public APlayerController, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ATowerDefensePlayerController();

    void SetBuildingMode(FHireInfo BuildingInfo);

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void Tick(float DeltaSeconds) override;

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPlayerAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(EditAnywhere, Category = "Team")
    FGenericTeamId TeamId;

#pragma region Hire
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
    FHireInfo BuildingInfo{};

    UPROPERTY(VisibleInstanceOnly, Category = Building)
    bool bIsBuildingMode = false;
#pragma endregion

private:
    void BuildPreviewActor();
    void UpdateBuildingPreview() const;

    FVector CachedDestination = FVector::ZeroVector;

    bool bIsTouch = false;
    float FollowTime = 0.f;
};
