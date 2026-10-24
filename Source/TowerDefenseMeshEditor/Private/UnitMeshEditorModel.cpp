// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "UnitMeshEditorModel.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UnitMeshBuilder.h"

UUnitMeshEditorModel::UUnitMeshEditorModel()
{
    OutputFolder.Path = TEXT("/Game/TowerDefense/Core/Meshes/Units");
}

USkeletalMesh* UUnitMeshEditorModel::Build(FText& OutError) const
{
    return UnitMeshBuilder::Build(Parts, OutError);
}

USkeletalMesh* UUnitMeshEditorModel::SaveNew(FText& OutError) const
{
    const FString PackageName = OutputFolder.Path / AssetName;
    if (AssetName.IsEmpty() || AssetName.Contains(TEXT("/")) || AssetName.Contains(TEXT(".")) || !OutputFolder.Path.StartsWith(TEXT("/Game/"))
        || !FPackageName::IsValidLongPackageName(PackageName, false, &OutError))
    {
        OutError = FText::FromString(TEXT("Choose a valid asset name and a folder under /Game/."));
        return nullptr;
    }

    if (FindPackage(nullptr, *PackageName) || FPackageName::DoesPackageExist(PackageName))
    {
        OutError = FText::FromString(TEXT("An asset already exists at this path. Choose a new name."));
        return nullptr;
    }

    USkeletalMesh* Mesh = Build(OutError);
    if (!Mesh)
    {
        return nullptr;
    }

    if (UPackage* Package = CreatePackage(*PackageName); !Mesh->Rename(*AssetName, Package, REN_DontCreateRedirectors | REN_NonTransactional))
    {
        OutError = FText::FromString(TEXT("Unable to move the assembled mesh into its package."));
        return nullptr;
    }

    Mesh->ClearFlags(RF_Transient);
    Mesh->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
    UUnitMeshAssembly* Recipe = NewObject<UUnitMeshAssembly>(Mesh);
    Recipe->Parts = Parts;
    Mesh->AddAssetUserData(Recipe);
    FAssetRegistryModule::AssetCreated(Mesh);
    std::ignore = Mesh->MarkPackageDirty();
    return Mesh;
}

bool UUnitMeshEditorModel::LoadAssembly(USkeletalMesh* Mesh, FText& OutError)
{
    const UUnitMeshAssembly* Recipe = Mesh ? Cast<UUnitMeshAssembly>(Mesh->GetAssetUserDataOfClass(UUnitMeshAssembly::StaticClass())) : nullptr;
    if (!Recipe)
    {
        OutError = FText::FromString(TEXT("Select a skeletal mesh previously created by Unit Mesh Editor in the Content Browser."));
        return false;
    }
    Parts = Recipe->Parts;
    AssetName = Mesh->GetName() + TEXT("_Copy");
    OutputFolder.Path = FPackageName::GetLongPackagePath(Mesh->GetOutermost()->GetName());
    return true;
}

TArray<FString> UUnitMeshEditorModel::GetAttachmentNames() const
{
    TArray<FString> Names = { TEXT("None") };
    if (!Parts.Body)
    {
        return Names;
    }
    for (const USkeletalMeshSocket* Socket : Parts.Body->GetActiveSocketList())
    {
        if (Socket)
        {
            Names.AddUnique(Socket->SocketName.ToString());
        }
    }
    for (const FMeshBoneInfo& Bone : Parts.Body->GetRefSkeleton().GetRawRefBoneInfo())
    {
        Names.AddUnique(Bone.Name.ToString());
    }
    if (const USkeleton* Skeleton = Parts.Body->GetSkeleton())
    {
        for (const FMeshBoneInfo& Bone : Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo())
        {
            Names.AddUnique(Bone.Name.ToString());
        }
    }
    return Names;
}
