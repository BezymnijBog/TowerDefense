// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "TowerDefenseUnitEditorModule.h"

#include "Framework/Docking/TabManager.h"
#include "EquipmentDefinitionDetails.h"
#include "Equipment/UnitEquipmentDefinition.h"
#include "PropertyEditorModule.h"
#include "SEquipmentEditor.h"
#include "SUnitEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

namespace
{
const FName UnitEditorTab(TEXT("TowerDefenseUnitEditor"));
const FName EquipmentEditorTab(TEXT("TowerDefenseEquipmentEditor"));
} // namespace

void FTowerDefenseUnitEditorModule::StartupModule()
{
    FPropertyEditorModule& Properties = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    Properties.RegisterCustomClassLayout(UUnitEquipmentDefinition::StaticClass()->GetFName(),
                                         FOnGetDetailCustomizationInstance::CreateStatic(&FEquipmentDefinitionDetails::MakeInstance));
    Properties.NotifyCustomizationModuleChanged();
    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(UnitEditorTab, FOnSpawnTab::CreateRaw(this, &FTowerDefenseUnitEditorModule::SpawnTab))
        .SetDisplayName(FText::FromString(TEXT("Unit Editor")))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(EquipmentEditorTab, FOnSpawnTab::CreateRaw(this, &FTowerDefenseUnitEditorModule::SpawnEquipmentTab))
        .SetDisplayName(FText::FromString(TEXT("Equipment Editor")))
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
    if (const TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->FindExistingLiveTab(EquipmentEditorTab); Tab)
    {
        Tab->SetCanCloseTab(SDockTab::FCanCloseTab());
        Tab->RequestCloseTab();
    }
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EquipmentEditorTab);
    if (FPropertyEditorModule* Properties = FModuleManager::GetModulePtr<FPropertyEditorModule>(TEXT("PropertyEditor")))
    {
        Properties->UnregisterCustomClassLayout(UUnitEquipmentDefinition::StaticClass()->GetFName());
        Properties->NotifyCustomizationModuleChanged();
    }
}

TSharedRef<SDockTab> FTowerDefenseUnitEditorModule::SpawnEquipmentTab(const FSpawnTabArgs&)
{
    const TSharedRef<SEquipmentEditor> Editor = SNew(SEquipmentEditor);
    TSharedRef<SDockTab> Tab = SNew(SDockTab).TabRole(ETabRole::NomadTab)[Editor];
    Tab->SetCanCloseTab(SDockTab::FCanCloseTab::CreateSP(Editor, &SEquipmentEditor::CanClose));
    return Tab;
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
    Menu->FindOrAddSection(TEXT("TowerDefense"))
        .AddMenuEntry(TEXT("OpenEquipmentEditor"),
                      FText::FromString(TEXT("Equipment Editor")),
                      FText::FromString(TEXT("Create and edit unit equipment assets")),
                      FSlateIcon(),
                      FUIAction(FExecuteAction::CreateLambda([] { FGlobalTabmanager::Get()->TryInvokeTab(EquipmentEditorTab); })));
}

IMPLEMENT_MODULE(FTowerDefenseUnitEditorModule, TowerDefenseUnitEditor)
