// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Equipment/UnitEquipmentDefinition.h"
#include "EquipmentEditorModel.generated.h"

// The form owns a transient copy, so editing never changes the source asset until Save.
UCLASS(Transient)
class UEquipmentEditorModel : public UObject
{
    GENERATED_BODY()

public:
    UEquipmentEditorModel();
    void InitializeNew();
    bool Load(UUnitEquipmentDefinition* Asset, FText& OutStatus);
    bool Save(bool bCreateNew, FText& OutStatus);
    UUnitEquipmentDefinition* GetDraft() const { return Draft; }
    UUnitEquipmentDefinition* GetLoadedAsset() const { return LoadedAsset; }

    UPROPERTY(EditAnywhere, Category = "Source")
    TObjectPtr<UUnitEquipmentDefinition> SourceAsset;

    UPROPERTY(VisibleAnywhere, Category = "Source")
    TObjectPtr<UUnitEquipmentDefinition> LoadedAsset;

    UPROPERTY(EditAnywhere, Category = "Save new asset")
    FString AssetName = TEXT("DA_NewEquipment");

    UPROPERTY(EditAnywhere, Category = "Save new asset", meta = (ContentDir))
    FDirectoryPath OutputFolder;

private:
    bool GetNewAssetPath(FString& OutPath, FText& OutStatus) const;
    UUnitEquipmentDefinition* GetSaveTarget(bool bCreateNew, FText& OutStatus);

    UPROPERTY(Transient)
    TObjectPtr<UUnitEquipmentDefinition> Draft;

    // A failed initial save can be retried without colliding with its in-memory package.
    bool bPendingNewSave = false;
};
