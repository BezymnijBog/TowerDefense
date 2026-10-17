// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru


#include "Game/MainMenuGameMode.h"
#include "Actors/PlayerHUD.h"
#include "Player/MainMenuController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    DefaultPawnClass = nullptr;
    PlayerControllerClass = AMainMenuController::StaticClass();
    HUDClass = APlayerHUD::StaticClass();
}

