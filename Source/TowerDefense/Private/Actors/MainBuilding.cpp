// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/MainBuilding.h"

#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

void AMainBuilding::OnBuildingDestroyed()
{
    Super::OnBuildingDestroyed();
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->FailRun();
    }
}
