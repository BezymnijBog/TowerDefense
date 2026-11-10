// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "EquipmentEditorModel.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
struct FPropertyChangedEvent;

class SEquipmentEditor : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SEquipmentEditor) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& Arguments);
    bool CanClose() const;

private:
    TSharedRef<SWidget> MakeToolbar();
    void SetModel(UEquipmentEditorModel* NewModel);
    void OnChanged(const FPropertyChangedEvent& Event);
    FReply NewItem();
    FReply LoadItem();
    FReply SaveItem(bool bCreateNew);
    FReply BrowseAsset() const;

    TStrongObjectPtr<UEquipmentEditorModel> Model;
    TSharedPtr<IDetailsView> DestinationDetails;
    TSharedPtr<IDetailsView> ItemDetails;
    FText Status;
    bool bFormChanged = false;
};
