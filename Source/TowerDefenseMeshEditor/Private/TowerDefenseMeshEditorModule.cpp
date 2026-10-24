// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "TowerDefenseMeshEditorModule.h"

#include "Framework/Docking/TabManager.h"
#include "SUnitMeshEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

namespace
{
const FName MeshEditorTab(TEXT("TowerDefenseMeshEditor"));
}

void FTowerDefenseMeshEditorModule::StartupModule()
{
    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(MeshEditorTab, FOnSpawnTab::CreateRaw(this, &FTowerDefenseMeshEditorModule::SpawnTab))
        .SetDisplayName(FText::FromString(TEXT("Unit Mesh Editor")))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTowerDefenseMeshEditorModule::RegisterMenus));
}

void FTowerDefenseMeshEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    if (const TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->FindExistingLiveTab(MeshEditorTab))
    {
        Tab->SetCanCloseTab(SDockTab::FCanCloseTab());
        Tab->RequestCloseTab();
    }
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MeshEditorTab);
}

TSharedRef<SDockTab> FTowerDefenseMeshEditorModule::SpawnTab(const FSpawnTabArgs& Arguments)
{
    const TSharedRef<SUnitMeshEditor> Editor = SNew(SUnitMeshEditor);
    TSharedRef<SDockTab> Tab = SNew(SDockTab).TabRole(ETabRole::NomadTab)[Editor];
    Tab->SetCanCloseTab(SDockTab::FCanCloseTab::CreateSP(Editor, &SUnitMeshEditor::CanClose));
    return Tab;
}

void FTowerDefenseMeshEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped Owner(this);
    UToolMenus* Menus = UToolMenus::Get();
    Menus->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"))
        ->FindOrAddSection(TEXT("TowerDefense"), FText::FromString(TEXT("Tower Defense")))
        .AddMenuEntry(TEXT("OpenUnitMeshEditor"),
                      FText::FromString(TEXT("Unit Mesh Editor")),
                      FText::FromString(TEXT("Assemble skeletal unit meshes and equipment")),
                      FSlateIcon(),
                      FUIAction(FExecuteAction::CreateLambda([] { FGlobalTabmanager::Get()->TryInvokeTab(MeshEditorTab); })));
}

IMPLEMENT_MODULE(FTowerDefenseMeshEditorModule, TowerDefenseMeshEditor)
