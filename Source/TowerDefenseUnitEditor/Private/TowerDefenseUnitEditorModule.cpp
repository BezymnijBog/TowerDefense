// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "TowerDefenseUnitEditorModule.h"

#include "Framework/Docking/TabManager.h"
#include "SUnitEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

namespace
{
const FName UnitEditorTab(TEXT("TowerDefenseUnitEditor"));
}

void FTowerDefenseUnitEditorModule::StartupModule()
{
    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(UnitEditorTab, FOnSpawnTab::CreateRaw(this, &FTowerDefenseUnitEditorModule::SpawnTab))
        .SetDisplayName(FText::FromString(TEXT("Unit Editor")))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTowerDefenseUnitEditorModule::RegisterMenus));
}

void FTowerDefenseUnitEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    if (const TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->FindExistingLiveTab(UnitEditorTab); Tab)
    {
        Tab->SetCanCloseTab(SDockTab::FCanCloseTab());
        Tab->RequestCloseTab();
    }
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UnitEditorTab);
}

TSharedRef<SDockTab> FTowerDefenseUnitEditorModule::SpawnTab(const FSpawnTabArgs&)
{
    const TSharedRef<SUnitEditor> Editor = SNew(SUnitEditor);
    TSharedRef<SDockTab> Tab = SNew(SDockTab).TabRole(ETabRole::NomadTab)[Editor];
    Tab->SetCanCloseTab(SDockTab::FCanCloseTab::CreateSP(Editor, &SUnitEditor::CanClose));
    return Tab;
}

void FTowerDefenseUnitEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped Owner(this);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
    Menu->FindOrAddSection(TEXT("TowerDefense"), FText::FromString(TEXT("Tower Defense")))
        .AddMenuEntry(TEXT("OpenUnitEditor"),
                      FText::FromString(TEXT("Unit Editor")),
                      FText::FromString(TEXT("Create and edit Tower Defense units")),
                      FSlateIcon(),
                      FUIAction(FExecuteAction::CreateLambda([] { FGlobalTabmanager::Get()->TryInvokeTab(UnitEditorTab); })));
}

IMPLEMENT_MODULE(FTowerDefenseUnitEditorModule, TowerDefenseUnitEditor)
