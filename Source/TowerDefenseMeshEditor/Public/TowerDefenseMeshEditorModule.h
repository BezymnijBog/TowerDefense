// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FTowerDefenseMeshEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();
    TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Arguments);
};
