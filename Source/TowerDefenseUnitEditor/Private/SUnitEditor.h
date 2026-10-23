// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UnitEditorModel.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

class IAssetTools;
class IDetailsView;
struct FPropertyChangedEvent;

class SUnitEditor : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SUnitEditor) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& Arguments);
    bool CanClose() const;

private:
    void OnChanged(const FPropertyChangedEvent& Event);
    void SetModel(UUnitEditorModel* NewModel);
    void GetDataForSave(UBlueprint*& Blueprint, UDataTable*& DataTable);

    UBlueprint* CreateBlueprint(const FString& BlueprintPath, IAssetTools& AssetTools) const;
    UDataTable* CreateDataTable(const FString& DataTablePath, IAssetTools& AssetTools) const;

    FReply OpenUnit();
    FReply NewBlank();
    FReply NewFromTemplate();
    FText GetDestinationText() const;
    FReply SaveUnit();
    FReply SaveUnit(UBlueprint* Blueprint, UDataTable* DataTable);
    FReply BrowseAssets() const;

    TStrongObjectPtr<UUnitEditorModel> Model;
    TSharedPtr<IDetailsView> Details;
    FText Status;
    bool bFormChanged = false;
};
