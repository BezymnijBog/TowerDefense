// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "EquipmentEditorModel.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "FileHelpers.h"
#include "Misc/PackageName.h"
#include "ScopedTransaction.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "EquipmentEditor"

UEquipmentEditorModel::UEquipmentEditorModel()
{
    OutputFolder.Path = TEXT("/Game/TowerDefense/Core/Miscellaneous/Equipment");
}

void UEquipmentEditorModel::InitializeNew()
{
    Draft = NewObject<UUnitEquipmentDefinition>(this, NAME_None, RF_Transactional);
}

bool UEquipmentEditorModel::Load(UUnitEquipmentDefinition* Asset, FText& OutStatus)
{
    if (!IsValid(Asset))
    {
        OutStatus = LOCTEXT("ChooseSource", "Выберите ассет экипировки в поле Source Asset.");
        return false;
    }
    Draft = DuplicateObject<UUnitEquipmentDefinition>(Asset, this);
    Draft->ClearFlags(RF_Public | RF_Standalone);
    Draft->SetFlags(RF_Transactional);
    SourceAsset = Asset;
    LoadedAsset = Asset;
    AssetName = Asset->GetName();
    OutputFolder.Path = FPackageName::GetLongPackagePath(Asset->GetOutermost()->GetName());
    bPendingNewSave = false;
    OutStatus = LOCTEXT("Loaded", "Ассет загружен. Сохраните изменения или задайте другое имя для новой копии.");
    return true;
}

bool UEquipmentEditorModel::GetNewAssetPath(FString& OutPath, FText& OutStatus) const
{
    OutPath = OutputFolder.Path / AssetName;
    if (AssetName.IsEmpty() || AssetName.Contains(TEXT("/")) || AssetName.Contains(TEXT("."))
        || !(OutputFolder.Path == TEXT("/Game") || OutputFolder.Path.StartsWith(TEXT("/Game/")))
        || !FPackageName::IsValidLongPackageName(OutPath, false, &OutStatus))
    {
        OutStatus = LOCTEXT("InvalidPath", "Укажите корректное имя ассета без расширения и папку внутри /Game.");
        return false;
    }
    return true;
}

UUnitEquipmentDefinition* UEquipmentEditorModel::GetSaveTarget(bool bCreateNew, FText& OutStatus)
{
    if (!bCreateNew)
    {
        if (!IsValid(LoadedAsset))
        {
            OutStatus = LOCTEXT("NoLoadedAsset", "Сначала загрузите существующий ассет.");
        }
        return LoadedAsset;
    }
    FString Path;
    if (!GetNewAssetPath(Path, OutStatus))
    {
        return nullptr;
    }
    if (bPendingNewSave && LoadedAsset && LoadedAsset->GetOutermost()->GetName() == Path)
    {
        return LoadedAsset;
    }
    if (FPackageName::DoesPackageExist(Path) || FindPackage(nullptr, *Path))
    {
        OutStatus = LOCTEXT("AlreadyExists", "По указанному пути уже есть ассет. Выберите другое имя или загрузите его для редактирования.");
        return nullptr;
    }
    UUnitEquipmentDefinition* Asset = DuplicateObject<UUnitEquipmentDefinition>(Draft, CreatePackage(*Path), *AssetName);
    Asset->ClearFlags(RF_Transient);
    Asset->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
    FAssetRegistryModule::AssetCreated(Asset);
    LoadedAsset = Asset;
    SourceAsset = Asset;
    bPendingNewSave = true;
    return Asset;
}

bool UEquipmentEditorModel::Save(bool bCreateNew, FText& OutStatus)
{
    if (!GEditor || GEditor->PlayWorld)
    {
        OutStatus = LOCTEXT("StopPIE", "Остановите Play / Simulate перед сохранением ассетов.");
        return false;
    }
    if (!Draft || !Draft->Validate(Draft->Slot, OutStatus))
    {
        return false;
    }
    const FScopedTransaction Transaction(LOCTEXT("EditEquipment", "Изменить экипировку юнита"));
    UUnitEquipmentDefinition* Asset = GetSaveTarget(bCreateNew, OutStatus);
    if (!Asset)
    {
        return false;
    }
    Asset->Modify();
    Asset->PreEditChange(nullptr);
    UEngine::FCopyPropertiesForUnrelatedObjectsParams CopyParams;
    // Copy default values too, so clearing a bonus, tag or socket updates existing assets.
    CopyParams.bDoDelta = false;
    UEngine::CopyPropertiesForUnrelatedObjects(Draft, Asset, CopyParams);
    Asset->PostEditChange();
    std::ignore = Asset->MarkPackageDirty();
    const TArray<UPackage*> Packages{ Asset->GetOutermost() };
    if (FEditorFileUtils::PromptForCheckoutAndSave(Packages, false, false) != FEditorFileUtils::PR_Success)
    {
        OutStatus = LOCTEXT("SaveFailed", "Изменения применены в памяти, но сохранение отменено или завершилось ошибкой. Повторите сохранение.");
        return false;
    }
    bPendingNewSave = false;
    OutStatus = FText::Format(LOCTEXT("Saved", "Сохранён ассет: {0}"), FText::FromString(Asset->GetPathName()));
    return true;
}

#undef LOCTEXT_NAMESPACE
