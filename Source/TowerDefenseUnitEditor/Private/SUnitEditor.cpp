// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "SUnitEditor.h"

#include "AI/TowerDefenseAICharacter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "FileHelpers.h"
#include "IDetailsView.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
bool PackageExists(const FString& Path)
{
    return FPackageName::DoesPackageExist(Path) || FindPackage(nullptr, *Path) != nullptr;
}
} // namespace

void SUnitEditor::Construct(const FArguments&)
{
    Model.Reset(NewObject<UUnitEditorModel>(GetTransientPackage(), NAME_None, RF_Transactional));
    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bAllowSearch = true;
    DetailsArgs.bHideSelectionTip = true;
    DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    Details = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor")).CreateDetailView(DetailsArgs);
    Details->SetObject(Model.Get());
    Details->OnFinishedChangingProperties().AddSP(this, &SUnitEditor::OnChanged);
    ChildSlot[SNew(SBorder).Padding(
        10)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
                  [SNew(SHorizontalBox)
                   + SHorizontalBox::Slot().AutoWidth().Padding(
                       3)[SNew(SButton).Text(FText::FromString(TEXT("Open selected unit"))).OnClicked(this, &SUnitEditor::OpenUnit)]
                   + SHorizontalBox::Slot().AutoWidth().Padding(
                       3)[SNew(SButton).Text(FText::FromString(TEXT("New blank"))).OnClicked(this, &SUnitEditor::NewBlank)]
                   + SHorizontalBox::Slot().AutoWidth().Padding(
                       3)[SNew(SButton).Text(FText::FromString(TEXT("New from template"))).OnClicked(this, &SUnitEditor::NewFromTemplate)]
                   + SHorizontalBox::Slot().AutoWidth().Padding(
                       3)[SNew(SButton)
                              .Text_Lambda([this] { return FText::FromString(Model->LoadedUnit ? TEXT("Save unit") : TEXT("Create unit")); })
                              .OnClicked(this, &SUnitEditor::SaveUnit)]
                   + SHorizontalBox::Slot().AutoWidth().Padding(
                       3)[SNew(SButton).Text(FText::FromString(TEXT("Browse assets"))).OnClicked(this, &SUnitEditor::BrowseAssets)]]
            + SVerticalBox::Slot().AutoHeight().Padding(3, 0, 3, 8)
                  [SNew(STextBlock)
                       .AutoWrapText(true)
                       .Text(FText::FromString(TEXT(
                           "Select Unit / template Blueprint, then Open or New from template. Save updates the loaded Blueprint and its linked table; a shared table affects every unit that uses it. Centimetres and cells are independent gameplay fields.")))]
            + SVerticalBox::Slot().FillHeight(1.f)[Details.ToSharedRef()]
            + SVerticalBox::Slot().AutoHeight().Padding(3, 8)[SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] { return GetDestinationText(); })]
            + SVerticalBox::Slot().AutoHeight().Padding(3)[SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] { return Status; })]]];
}

bool SUnitEditor::CanClose() const
{
    return !bFormChanged || FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("Discard unsaved unit form changes?"))) == EAppReturnType::Yes;
}

void SUnitEditor::OnChanged(const FPropertyChangedEvent& Event)
{
    if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnitEditorModel, SourceUnit))
    {
        return;
    }
    bFormChanged = true;
    Status = FText::FromString(TEXT("Unsaved form changes."));
}

void SUnitEditor::SetModel(UUnitEditorModel* NewModel)
{
    Model.Reset(NewModel);
    Details->SetObject(NewModel);
    bFormChanged = false;
}

void SUnitEditor::GetDataForSave(UBlueprint*& Blueprint, UDataTable*& DataTable)
{
    FString BlueprintPath;
    FString DataTablePath;
    const bool bCreateBlueprint = !Model->LoadedUnit;
    const bool bCreateDataTable = bCreateBlueprint || !Model->AttributeTable;
    if (bCreateDataTable && !UUnitEditorModel::MakeAssetPaths(Model->UnitName, Model->Team, BlueprintPath, DataTablePath, Status))
    {
        return;
    }

    if ((bCreateBlueprint && PackageExists(BlueprintPath)) || (bCreateDataTable && PackageExists(DataTablePath)))
    {
        Status = FText::FromString(TEXT("A destination asset already exists. Change UnitName or open the existing unit."));
        return;
    }

    const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Tower Defense unit")));
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
    Blueprint = bCreateBlueprint ? CreateBlueprint(BlueprintPath, AssetTools) : Model->LoadedUnit.Get();
    DataTable = bCreateDataTable ? CreateDataTable(DataTablePath, AssetTools) : Model->AttributeTable.Get();
}

UBlueprint* SUnitEditor::CreateBlueprint(const FString& BlueprintPath, IAssetTools& AssetTools) const
{
    if (Model->CreationTemplate)
    {
        return Cast<UBlueprint>(AssetTools.DuplicateAsset(
            FPackageName::GetLongPackageAssetName(BlueprintPath), FPackageName::GetLongPackagePath(BlueprintPath), Model->CreationTemplate));
    }

    UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(Model->CreationParentClass,
                                                                    CreatePackage(*BlueprintPath),
                                                                    FName(*FPackageName::GetLongPackageAssetName(BlueprintPath)),
                                                                    BPTYPE_Normal,
                                                                    UBlueprint::StaticClass(),
                                                                    UBlueprintGeneratedClass::StaticClass());
    if (IsValid(Blueprint))
    {
        FAssetRegistryModule::AssetCreated(Blueprint);
    }
    return Blueprint;
}

UDataTable* SUnitEditor::CreateDataTable(const FString& DataTablePath, IAssetTools& AssetTools) const
{
    if (Model->AttributeTable)
    {
        return Cast<UDataTable>(AssetTools.DuplicateAsset(
            FPackageName::GetLongPackageAssetName(DataTablePath), FPackageName::GetLongPackagePath(DataTablePath), Model->AttributeTable));
    }

    UDataTable* Table = NewObject<UDataTable>(
        CreatePackage(*DataTablePath), *FPackageName::GetLongPackageAssetName(DataTablePath), RF_Public | RF_Standalone | RF_Transactional);
    if (IsValid(Table))
    {
        Table->RowStruct = FAttributeMetaData::StaticStruct();
        FAssetRegistryModule::AssetCreated(Table);
    }
    return Table;
}

FReply SUnitEditor::OpenUnit()
{
    if (!CanClose())
    {
        return FReply::Handled();
    }
    TStrongObjectPtr<UUnitEditorModel> Candidate(NewObject<UUnitEditorModel>());
    if (Candidate->LoadUnit(Model->SourceUnit, Status))
    {
        SetModel(Candidate.Get());
        Status = FText::FromString(TEXT("Loaded from Blueprint and AbilitySystemComponent."));
    }
    return FReply::Handled();
}

FReply SUnitEditor::NewBlank()
{
    if (!CanClose())
    {
        return FReply::Handled();
    }
    UUnitEditorModel* NewModel = NewObject<UUnitEditorModel>();
    NewModel->CreationParentClass = Model->CreationParentClass;
    SetModel(NewModel);
    Status = FText::FromString(TEXT("New unit. Select parent, team, visuals, controller and abilities, then Create unit."));
    return FReply::Handled();
}

FReply SUnitEditor::NewFromTemplate()
{
    if (!CanClose())
    {
        return FReply::Handled();
    }

    if (const TStrongObjectPtr<UUnitEditorModel> Candidate(NewObject<UUnitEditorModel>()); Candidate->LoadUnit(Model->SourceUnit, Status))
    {
        Candidate->CreationTemplate = Candidate->LoadedUnit;
        Candidate->LoadedUnit = nullptr;
        Candidate->UnitName += TEXT("_Copy");
        SetModel(Candidate.Get());
        Status = FText::FromString(TEXT("Template loaded. Creation will copy its Blueprint and make an independent attribute table."));
    }
    return FReply::Handled();
}

FText SUnitEditor::GetDestinationText() const
{
    if (Model->LoadedUnit)
    {
        return FText::FromString(TEXT("Editing: ") + Model->LoadedUnit->GetPathName() + TEXT("\nAttributes: ")
                                 + (Model->AttributeTable ? Model->AttributeTable->GetPathName() : TEXT("No table assigned — Save will create one.")));
    }
    FString BlueprintPath, TablePath;
    FText Error;
    return UUnitEditorModel::MakeAssetPaths(Model->UnitName, Model->Team, BlueprintPath, TablePath, Error) ?
        FText::FromString(BlueprintPath + TEXT("\n") + TablePath) :
        Error;
}

FReply SUnitEditor::SaveUnit()
{
    if (GEditor->PlayWorld)
    {
        Status = FText::FromString(TEXT("Stop Play / Simulate before saving unit assets."));
        return FReply::Handled();
    }

    if (!Model->Validate(Status))
    {
        return FReply::Handled();
    }

    UBlueprint* Blueprint = nullptr;
    UDataTable* DataTable = nullptr;
    GetDataForSave(Blueprint, DataTable);
    return SaveUnit(Blueprint, DataTable);
}

FReply SUnitEditor::SaveUnit(UBlueprint* Blueprint, UDataTable* DataTable)
{
    if (!IsValid(Blueprint) || !IsValid(DataTable))
    {
        return FReply::Handled();
    }

    // Keep failed or cancelled saves editable, rather than creating a second pair on retry.
    Model->LoadedUnit = Blueprint;
    Model->SourceUnit = Blueprint;
    Model->AttributeTable = DataTable;
    bFormChanged = true;
    if (!Model->ApplyTo(Blueprint, DataTable, Status))
    {
        Details->ForceRefresh();
        return FReply::Handled();
    }

    const TArray<UPackage*> Packages{ Blueprint->GetOutermost(), DataTable->GetOutermost() };
    const FEditorFileUtils::EPromptReturnCode Result = FEditorFileUtils::PromptForCheckoutAndSave(Packages, false, false);
    bFormChanged = Result != FEditorFileUtils::PR_Success;
    Status = FText::FromString(bFormChanged ? TEXT("Changes applied in memory, but saving was cancelled or failed. Retry Save unit.") :
                                              TEXT("Blueprint compiled. Unit and attribute table saved."));
    Details->ForceRefresh();
    return FReply::Handled();
}

FReply SUnitEditor::BrowseAssets() const
{
    TArray<UObject*> Assets;
    if (Model->LoadedUnit)
    {
        Assets.Add(Model->LoadedUnit);
    }

    if (Model->AttributeTable)
    {
        Assets.Add(Model->AttributeTable);
    }

    if (!Assets.IsEmpty())
    {
        GEditor->SyncBrowserToObjects(Assets);
    }
    return FReply::Handled();
}
