// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "SEquipmentEditor.h"

#include "Editor.h"
#include "IDetailsView.h"
#include "Misc/MessageDialog.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "EquipmentEditor"

void SEquipmentEditor::Construct(const FArguments&)
{
    FDetailsViewArgs Args;
    Args.bAllowSearch = true;
    Args.bHideSelectionTip = true;
    Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    FPropertyEditorModule& Properties = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    DestinationDetails = Properties.CreateDetailView(Args);
    ItemDetails = Properties.CreateDetailView(Args);
    DestinationDetails->OnFinishedChangingProperties().AddSP(this, &SEquipmentEditor::OnChanged);
    ItemDetails->OnFinishedChangingProperties().AddSP(this, &SEquipmentEditor::OnChanged);
    UEquipmentEditorModel* NewModel = NewObject<UEquipmentEditorModel>();
    NewModel->InitializeNew();
    SetModel(NewModel);
    Status =
        LOCTEXT("Instructions", "Заполните предмет, имя и папку, затем сохраните новый ассет. Для редактирования выберите Source Asset и нажмите «Загрузить».");
    ChildSlot[SNew(SBorder).Padding(
        10)[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)[MakeToolbar()]
            + SVerticalBox::Slot().FillHeight(
                1.f)[SNew(SScrollBox) + SScrollBox::Slot()[DestinationDetails.ToSharedRef()] + SScrollBox::Slot().Padding(0, 8)[ItemDetails.ToSharedRef()]]
            + SVerticalBox::Slot().AutoHeight().Padding(3, 8)[SNew(STextBlock).AutoWrapText(true).Text_Lambda([this] { return Status; })]]];
}

TSharedRef<SWidget> SEquipmentEditor::MakeToolbar()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().Padding(3)[SNew(SButton).Text(LOCTEXT("New", "Новый")).OnClicked(this, &SEquipmentEditor::NewItem)]
        + SHorizontalBox::Slot().AutoWidth().Padding(3)[SNew(SButton).Text(LOCTEXT("Load", "Загрузить")).OnClicked(this, &SEquipmentEditor::LoadItem)]
        + SHorizontalBox::Slot().AutoWidth().Padding(
            3)[SNew(SButton)
                   .Text(LOCTEXT("SaveNew", "Сохранить новый ассет"))
                   .ToolTipText(LOCTEXT("SaveNewHint", "Создать ассет по указанным Asset Name и Output Folder. Для копии выберите свободное имя."))
                   .OnClicked(this, &SEquipmentEditor::SaveItem, true)]
        + SHorizontalBox::Slot().AutoWidth().Padding(
            3)[SNew(SButton)
                   .Text(LOCTEXT("SaveChanges", "Сохранить изменения"))
                   .ToolTipText(LOCTEXT("SaveChangesHint", "Обновить Loaded Asset. Поля имени и папки используются только для создания нового ассета."))
                   .IsEnabled_Lambda([this] { return Model->GetLoadedAsset() != nullptr; })
                   .OnClicked(this, &SEquipmentEditor::SaveItem, false)]
        + SHorizontalBox::Slot().AutoWidth().Padding(3)[SNew(SButton)
                                                            .Text(LOCTEXT("Browse", "Показать ассет"))
                                                            .IsEnabled_Lambda([this] { return Model->GetLoadedAsset() != nullptr; })
                                                            .OnClicked(this, &SEquipmentEditor::BrowseAsset)];
}

bool SEquipmentEditor::CanClose() const
{
    return !bFormChanged
        || FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("Discard", "Отбросить несохранённые изменения формы экипировки?")) == EAppReturnType::Yes;
}

void SEquipmentEditor::SetModel(UEquipmentEditorModel* NewModel)
{
    Model.Reset(NewModel);
    DestinationDetails->SetObject(NewModel);
    ItemDetails->SetObject(NewModel->GetDraft());
    bFormChanged = false;
}

void SEquipmentEditor::OnChanged(const FPropertyChangedEvent& Event)
{
    if (Event.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UEquipmentEditorModel, SourceAsset))
    {
        bFormChanged = true;
        Status = LOCTEXT("Modified", "В форме есть несохранённые изменения.");
    }
}

FReply SEquipmentEditor::NewItem()
{
    if (CanClose())
    {
        UEquipmentEditorModel* NewModel = NewObject<UEquipmentEditorModel>();
        NewModel->InitializeNew();
        SetModel(NewModel);
        Status = LOCTEXT("NewItem", "Новый предмет. Выберите слот и меш, настройте бонусы и теги.");
    }
    return FReply::Handled();
}

FReply SEquipmentEditor::LoadItem()
{
    if (CanClose())
    {
        TStrongObjectPtr<UEquipmentEditorModel> Candidate(NewObject<UEquipmentEditorModel>());
        if (Candidate->Load(Model->SourceAsset, Status))
        {
            SetModel(Candidate.Get());
        }
    }
    return FReply::Handled();
}

FReply SEquipmentEditor::SaveItem(bool bCreateNew)
{
    if (Model->Save(bCreateNew, Status))
    {
        bFormChanged = false;
    }
    else
    {
        bFormChanged = true;
    }
    DestinationDetails->ForceRefresh();
    return FReply::Handled();
}

FReply SEquipmentEditor::BrowseAsset() const
{
    if (UUnitEquipmentDefinition* Asset = Model->GetLoadedAsset())
    {
        GEditor->SyncBrowserToObjects(TArray<UObject*>{ Asset });
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
