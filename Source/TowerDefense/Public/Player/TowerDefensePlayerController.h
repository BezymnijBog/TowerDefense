// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
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

    UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
};
