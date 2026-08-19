// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/TowerDefensePlayerController.h"

#include "AbilitySystemComponent.h"
#include "Components/PlayerAbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Widgets/HireMenuElement.h"

ATowerDefensePlayerController::ATowerDefensePlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* ATowerDefensePlayerController::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ATowerDefensePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

FGenericTeamId ATowerDefensePlayerController::GetGenericTeamId() const
{
    return TeamId;
}

void ATowerDefensePlayerController::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

void ATowerDefensePlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ATowerDefensePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}
