// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Widgets/MainMenu.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/PackageName.h"
#include "Settings/TowerDefenseSettings.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();
    bStartingGame = false;
    NewGameButton->SetIsEnabled(true);
    NewGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnNewGameClicked);
    ExitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitClicked);
}

void UMainMenu::NativeDestruct()
{
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
    const TSoftObjectPtr<UWorld>& GameMap = UTowerDefenseSettings::Get().GameMap;
    if (GameMap.IsNull() || !FPackageName::DoesPackageExist(GameMap.ToSoftObjectPath().GetLongPackageName()))
    {
        UE_LOG(LogTemp, Error, TEXT("Main menu: configure a valid GameMap in TowerDefenseSettings"));
        return;
    }
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->FailRun();
        Run->StartNewRun();
        bStartingGame = true;
        NewGameButton->SetIsEnabled(false);
        UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameMap);
    }
}

void UMainMenu::OnExitClicked()
{
    if (APlayerController* Controller = GetOwningPlayer(); IsValid(Controller))
    {
        UKismetSystemLibrary::QuitGame(this, Controller, EQuitPreference::Quit, false);
    }
}
