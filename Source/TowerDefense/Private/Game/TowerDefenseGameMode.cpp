// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Game/TowerDefenseGameMode.h"

APawn* ATowerDefenseGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    // Unlike the default implementation, preserve the camera pawn's starting pitch.
    return SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetActorTransform());
}
