// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "UnitMeshAssembly.generated.h"

class USkeletalMesh;
class UStaticMesh;

USTRUCT()
struct TOWERDEFENSEMESHEDITOR_API FUnitMeshAttachment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Attachment")
    TObjectPtr<UStaticMesh> Mesh;

    // None selects the appropriate SKEL_Units socket for this slot.
    UPROPERTY(EditAnywhere, Category = "Attachment", meta = (GetOptions = "GetAttachmentNames"))
    FName SocketOrBone;

    UPROPERTY(EditAnywhere, Category = "Attachment")
    FTransform Offset = FTransform::Identity;
};

USTRUCT()
struct TOWERDEFENSEMESHEDITOR_API FUnitMeshParts
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parts")
    TObjectPtr<USkeletalMesh> Body;

    UPROPERTY(EditAnywhere, Category = "Parts")
    TObjectPtr<USkeletalMesh> Head;

    UPROPERTY(EditAnywhere, Category = "Parts")
    FUnitMeshAttachment RightHand;

    UPROPERTY(EditAnywhere, Category = "Parts")
    FUnitMeshAttachment LeftHand;

    UPROPERTY(EditAnywhere, Category = "Parts")
    FUnitMeshAttachment Quiver;
};

// The recipe is retained in editor assets and stripped from cooked builds.
UCLASS()
class TOWERDEFENSEMESHEDITOR_API UUnitMeshAssembly : public UAssetUserData
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FUnitMeshParts Parts;

    virtual bool IsEditorOnly() const override { return true; }
};
