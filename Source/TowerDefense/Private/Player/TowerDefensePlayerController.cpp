// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/TowerDefensePlayerController.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Actors/PlayerHUD.h"
#include "Components/PlayerAbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Subsystems/WorldRoadSubsystem.h"
#include "Widgets/HireMenuElement.h"
#include "Widgets/PlayerHudWidget.h"

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

FGenericTeamId ATowerDefensePlayerController::GetGenericTeamId() const
{
    return TeamId;
}

void ATowerDefensePlayerController::SetGenericTeamId(const FGenericTeamId& TeamID)
{
    TeamId = TeamID;
}

void ATowerDefensePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(MovementMappingContext, 0);
        Subsystem->AddMappingContext(CommonMappingContext, 0);
    }

    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(StartWaveAction, ETriggerEvent::Triggered, this, &ThisClass::StartWave);
    }
}

void ATowerDefensePlayerController::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetSubsystem<UWorldRoadSubsystem>()->WaveCleared.AddUObject(this, &ThisClass::OnWaveCleared);
}

void ATowerDefensePlayerController::StartWave()
{
    if (!AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Action_Wave_Start)))
    {
        return;
    }

    if (const APlayerHUD* const PlayerHUD = GetHUD<APlayerHUD>(); IsValid(PlayerHUD))
    {
        const UPlayerHudWidget* const HudWidget = PlayerHUD->GetHUDWidget();
        check(IsValid(HudWidget));
        HudWidget->ToggleWaveWidgetVisibility(false);
    }
}

void ATowerDefensePlayerController::OnWaveCleared()
{
    if (const APlayerHUD* const PlayerHUD = GetHUD<APlayerHUD>(); IsValid(PlayerHUD))
    {
        const UPlayerHudWidget* const HudWidget = PlayerHUD->GetHUDWidget();
        check(IsValid(HudWidget));
        HudWidget->ToggleWaveWidgetVisibility(true);
    }
}
