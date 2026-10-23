// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FTowerDefenseTests : public FDefaultModuleImpl
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
