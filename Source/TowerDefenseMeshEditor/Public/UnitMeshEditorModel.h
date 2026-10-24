// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UnitMeshAssembly.h"
#include "UnitMeshEditorModel.generated.h"

UCLASS(Transient)
class TOWERDEFENSEMESHEDITOR_API UUnitMeshEditorModel : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Assembly", meta = (ShowOnlyInnerProperties))
    FUnitMeshParts Parts;

    UPROPERTY(EditAnywhere, Category = "Save")
    FString AssetName = TEXT("SK_NewUnit");

    UPROPERTY(EditAnywhere, Category = "Save", meta = (ContentDir))
    FDirectoryPath OutputFolder;

    UUnitMeshEditorModel();
    USkeletalMesh* Build(FText& OutError) const;
    USkeletalMesh* SaveNew(FText& OutError) const;
    bool LoadAssembly(USkeletalMesh* Mesh, FText& OutError);

    UFUNCTION()
    TArray<FString> GetAttachmentNames() const;
};
