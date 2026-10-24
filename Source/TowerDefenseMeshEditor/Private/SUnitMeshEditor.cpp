// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "SUnitMeshEditor.h"

#include "ContentBrowserModule.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "Engine/SkeletalMesh.h"
#include "FileHelpers.h"
#include "IContentBrowserSingleton.h"
#include "IDetailsView.h"
#include "Misc/MessageDialog.h"
#include "PropertyEditorModule.h"
#include "SUnitMeshPreview.h"
#include "UnitMeshEditorModel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Text/STextBlock.h"

void SUnitMeshEditor::Construct(const FArguments&)
{
    Model.Reset(NewObject<UUnitMeshEditorModel>());
    FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    FDetailsViewArgs DetailsArguments;
    DetailsArguments.bAllowSearch = true;
    DetailsArguments.bHideSelectionTip = true;
    Details = PropertyEditor.CreateDetailView(DetailsArguments);
    Details->SetObject(Model.Get());
    Details->OnFinishedChangingProperties().AddSP(this, &SUnitMeshEditor::OnChanged);
    Status =
        FText::FromString(TEXT("Select body and optional parts, then Build preview. Save creates a new LOD0 skeletal mesh. Socket or bone: None = automatic."));
    ChildSlot[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()[MakeToolbar()]
              + SVerticalBox::Slot().FillHeight(1.0f)[SNew(SSplitter) + SSplitter::Slot().Value(0.4f)[Details.ToSharedRef()]
                                                      + SSplitter::Slot().Value(0.6f)[SAssignNew(Viewport, SUnitMeshPreview)]]
              + SVerticalBox::Slot().AutoHeight().Padding(8)[SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] { return Status; })]];
}

TSharedRef<SWidget> SUnitMeshEditor::MakeToolbar()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().Padding(
            4)[SNew(SButton).Text(FText::FromString(TEXT("Load selected assembly"))).OnClicked(this, &SUnitMeshEditor::LoadSelected)]
        + SHorizontalBox::Slot().AutoWidth().Padding(4)[SNew(SButton).Text(FText::FromString(TEXT("Build preview"))).OnClicked(this, &SUnitMeshEditor::Preview)]
        + SHorizontalBox::Slot().AutoWidth().Padding(4)[SNew(SButton).Text(FText::FromString(TEXT("Save new mesh"))).OnClicked(this, &SUnitMeshEditor::Save)];
}

void SUnitMeshEditor::OnChanged(const FPropertyChangedEvent&)
{
    PendingSave.Reset();
    bModified = true;
    Status = FText::FromString(TEXT("Assembly changed. Build preview to update the viewport."));
}

bool SUnitMeshEditor::CanClose() const
{
    return !bModified || FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("Discard unsaved assembly changes?"))) == EAppReturnType::Yes;
}

FReply SUnitMeshEditor::Preview()
{
    if (USkeletalMesh* Mesh = Model->Build(Status))
    {
        Viewport->SetMesh(Mesh);
        Status = FText::FromString(TEXT("Preview updated. Body/head skinning and socket transforms are preserved."));
    }
    return FReply::Handled();
}

FReply SUnitMeshEditor::LoadSelected()
{
    if (!CanClose())
    {
        return FReply::Handled();
    }
    TArray<FAssetData> SelectedAssets;
    FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")).Get().GetSelectedAssets(SelectedAssets);
    USkeletalMesh* Selected = SelectedAssets.Num() == 1 ? Cast<USkeletalMesh>(SelectedAssets[0].GetAsset()) : nullptr;
    if (Model->LoadAssembly(Selected, Status))
    {
        PendingSave.Reset();
        Details->ForceRefresh();
        bModified = false;
        return Preview();
    }
    return FReply::Handled();
}

FReply SUnitMeshEditor::Save()
{
    if (!GEditor || GEditor->PlayWorld)
    {
        Status = FText::FromString(TEXT("Stop Play In Editor before saving an assembly."));
        return FReply::Handled();
    }

    if (!PendingSave.IsValid())
    {
        PendingSave.Reset(Model->SaveNew(Status));
    }
    if (!PendingSave.IsValid())
    {
        return FReply::Handled();
    }

    const TArray<UPackage*> Packages = { PendingSave->GetOutermost() };
    if (FEditorFileUtils::PromptForCheckoutAndSave(Packages, false, false) != FEditorFileUtils::PR_Success)
    {
        Status = FText::FromString(TEXT("Mesh created in memory but not saved. Press Save again to retry or save it in the Content Browser."));
        return FReply::Handled();
    }

    Viewport->SetMesh(PendingSave.Get());
    GEditor->SyncBrowserToObjects(TArray<UObject*>{ PendingSave.Get() });
    Status = FText::FromString(FString::Printf(TEXT("Saved %s"), *PendingSave->GetPathName()));
    bModified = false;
    return FReply::Handled();
}
