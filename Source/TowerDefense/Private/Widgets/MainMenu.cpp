// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/MainMenu.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameStartSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();
    bStartingGame = false;
    NewGameButton->SetIsEnabled(true);
    ExitButton->SetIsEnabled(true);
    if (UGameStartSubsystem* GameStart = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(this); IsValid(GameStart))
    {
        GameStart->OnGameStartFailed.RemoveAll(this);
        GameStart->OnGameStartFailed.AddUObject(this, &ThisClass::OnGameStartFailed);
    }
    NewGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnNewGameClicked);
    ExitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitClicked);
}

void UMainMenu::NativeDestruct()
{
    if (UGameStartSubsystem* GameStart = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(this); IsValid(GameStart))
    {
        GameStart->OnGameStartFailed.RemoveAll(this);
    }
    NewGameButton->OnClicked.RemoveDynamic(this, &ThisClass::OnNewGameClicked);
    ExitButton->OnClicked.RemoveDynamic(this, &ThisClass::OnExitClicked);
    Super::NativeDestruct();
}

void UMainMenu::OnAddedToViewport()
{
    if (APlayerController* Controller = GetOwningPlayer(); IsValid(Controller))
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(NewGameButton->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        Controller->SetInputMode(InputMode);
        Controller->SetShowMouseCursor(true);
    }
}

void UMainMenu::OnNewGameClicked()
{
    if (bStartingGame)
    {
        return;
    }
    UGameStartSubsystem* GameStart = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(this);
    URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this);
    if (IsValid(GameStart) && IsValid(Run) && GameStart->StartGame())
    {
        Run->FailRun();
        bStartingGame = true;
        NewGameButton->SetIsEnabled(false);
        ExitButton->SetIsEnabled(false);
    }
}

void UMainMenu::OnGameStartFailed()
{
    bStartingGame = false;
    NewGameButton->SetIsEnabled(true);
    ExitButton->SetIsEnabled(true);
    OnAddedToViewport();
}

void UMainMenu::OnExitClicked()
{
    if (APlayerController* Controller = GetOwningPlayer(); IsValid(Controller))
    {
        UKismetSystemLibrary::QuitGame(this, Controller, EQuitPreference::Quit, false);
    }
}
