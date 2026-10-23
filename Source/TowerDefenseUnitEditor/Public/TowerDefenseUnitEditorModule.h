// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FTowerDefenseUnitEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args);
    void RegisterMenus();
};
