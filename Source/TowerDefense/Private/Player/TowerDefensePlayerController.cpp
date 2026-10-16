// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/TowerDefensePlayerController.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Actors/PlayerHUD.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Components/PlayerAbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Subsystems/WorldRoadSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"
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
    if (UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); IsValid(Roads))
    {
        Roads->WaveCleared.AddUObject(this, &ThisClass::OnWaveCleared);
    }
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnCombatWon.AddUObject(this, &ThisClass::OnCombatWon);
    }
}

void ATowerDefensePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnCombatWon.RemoveAll(this);
    }
    if (UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); IsValid(Roads))
    {
        Roads->WaveCleared.RemoveAll(this);
    }
    Super::EndPlay(EndPlayReason);
}

void ATowerDefensePlayerController::OnCombatWon(int32 MoneyReward)
{
    if (HasAuthority() && IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->ApplyModToAttribute(UPlayerAttributeSet::GetMoneyAttribute(), EGameplayModOp::Additive, MoneyReward);
    }
}

void ATowerDefensePlayerController::StartWave()
{
    if (const UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); !IsValid(Roads) || !Roads->CanStartWave())
    {
        return;
    }
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
        const UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this);
        HudWidget->ToggleWaveWidgetVisibility(IsValid(Roads) && Roads->CanStartWave());
    }
}
