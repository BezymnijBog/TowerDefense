// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && WITH_EDITOR

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMesh.h"
#include "HAL/FileManager.h"
#include "MeshDescription.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "SkeletalMeshAttributes.h"
#include "UnitMeshEditorModel.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
bool LoadParts(UUnitMeshEditorModel& Model)
{
    Model.Parts.Body = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Toony_Tiny_RTS_Set_UE5/Meshes/Characters_Parts/Units/SK_Body_02b.SK_Body_02b"));
    Model.Parts.Head = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Toony_Tiny_RTS_Set_UE5/Meshes/Characters_Parts/Units/SK_Head_02a.SK_Head_02a"));
    Model.Parts.RightHand.Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Toony_Tiny_RTS_Set_UE5/Meshes/Weapons/SM_w_sword.SM_w_sword"));
    Model.Parts.LeftHand.Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Toony_Tiny_RTS_Set_UE5/Meshes/Weapons/SM_shield_03.SM_shield_03"));
    Model.Parts.Quiver.Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Toony_Tiny_RTS_Set_UE5/Meshes/Weapons/SM_quiver_A.SM_quiver_A"));
    return Model.Parts.Body && Model.Parts.Head && Model.Parts.RightHand.Mesh && Model.Parts.LeftHand.Mesh && Model.Parts.Quiver.Mesh;
}

void VerifyEquipment(
    FAutomationTestBase& Test, const USkeletalMesh& Mesh, const FMeshDescription& Description, int32 VertexOffset, UStaticMesh& Equipment, FName SocketName)
{
    const USkeletalMeshSocket* Socket = Mesh.FindSocket(SocketName);
    if (!Test.TestNotNull(TEXT("Socket retained"), Socket))
    {
        return;
    }
    const FReferenceSkeleton& Skeleton = Mesh.GetRefSkeleton();
    const int32 BoneIndex = Skeleton.FindBoneIndex(Socket->BoneName);
    FTransform BoneTransform = Skeleton.GetRefBonePose()[BoneIndex];
    for (int32 Parent = Skeleton.GetParentIndex(BoneIndex); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
    {
        BoneTransform *= Skeleton.GetRefBonePose()[Parent];
    }
    const FTransform AttachmentTransform = Socket->GetSocketLocalTransform() * BoneTransform;
    const FSkeletalMeshConstAttributes Attributes(Description);
    const FMeshDescription& Source = *Equipment.GetMeshDescription(0);
    const TVertexAttributesConstRef<FVector3f> SourcePositions = FStaticMeshConstAttributes(Source).GetVertexPositions();
    for (const FVertexID Vertex : Source.Vertices().GetElementIDs())
    {
        const FVertexID TargetVertex(VertexOffset++);
        const FVertexBoneWeightsConst Weights = Attributes.GetVertexSkinWeights().Get(TargetVertex);
        Test.TestEqual(TEXT("Equipment is rigidly skinned"), Weights.Num(), 1);
        if (Weights.Num() == 1)
        {
            Test.TestEqual(TEXT("Equipment uses socket's bone"), int32(Weights[0].GetBoneIndex()), BoneIndex);
        }
        const FVector Expected = AttachmentTransform.TransformPosition(FVector(SourcePositions[Vertex]));
        Test.TestTrue(TEXT("Socket transform including scale is baked"), FVector(Attributes.GetVertexPositions()[TargetVertex]).Equals(Expected, 0.01));
    }
}

void VerifySkinnedPart(
    FAutomationTestBase& Test, USkeletalMesh& SourceMesh, const USkeletalMesh& TargetMesh, const FMeshDescription& TargetDescription, int32 VertexOffset)
{
    const FMeshDescription& SourceDescription = *SourceMesh.GetMeshDescription(0);
    const FSkeletalMeshConstAttributes Source(SourceDescription);
    const FSkeletalMeshConstAttributes Target(TargetDescription);
    for (const FVertexID Vertex : SourceDescription.Vertices().GetElementIDs())
    {
        const FVertexID TargetVertex(VertexOffset++);
        Test.TestTrue(TEXT("Skinned geometry position retained"),
                      Source.GetVertexPositions()[Vertex].Equals(Target.GetVertexPositions()[TargetVertex], 0.001f));
        const FVertexBoneWeightsConst TargetWeights = Target.GetVertexSkinWeights().Get(TargetVertex);
        for (const UE::AnimationCore::FBoneWeight Weight : Source.GetVertexSkinWeights().Get(Vertex))
        {
            const FName BoneName = SourceMesh.GetRefSkeleton().GetBoneName(Weight.GetBoneIndex());
            bool bFound = false;
            for (const UE::AnimationCore::FBoneWeight TargetWeight : TargetWeights)
            {
                bFound |= TargetMesh.GetRefSkeleton().GetBoneName(TargetWeight.GetBoneIndex()) == BoneName
                    && FMath::Abs(int32(TargetWeight.GetRawWeight()) - int32(Weight.GetRawWeight())) <= 1;
            }
            Test.TestTrue(TEXT("Original skin influence retained by bone name"), bFound);
        }
    }
}

void VerifyAssembly(FAutomationTestBase& Test, UUnitMeshEditorModel& Model, USkeletalMesh& Mesh)
{
    const FMeshDescription* Description = Mesh.GetMeshDescription(0);
    if (!Test.TestNotNull(TEXT("Editable geometry retained"), Description))
    {
        return;
    }
    VerifySkinnedPart(Test, *Model.Parts.Body, Mesh, *Description, 0);
    VerifySkinnedPart(Test, *Model.Parts.Head, Mesh, *Description, Model.Parts.Body->GetMeshDescription(0)->Vertices().Num());
    int32 Offset = Model.Parts.Body->GetMeshDescription(0)->Vertices().Num() + Model.Parts.Head->GetMeshDescription(0)->Vertices().Num();
    VerifyEquipment(Test, Mesh, *Description, Offset, *Model.Parts.RightHand.Mesh, TEXT("Weapon_Socket_R"));
    Offset += Model.Parts.RightHand.Mesh->GetMeshDescription(0)->Vertices().Num();
    VerifyEquipment(Test, Mesh, *Description, Offset, *Model.Parts.LeftHand.Mesh, TEXT("Shield_Socket"));
    Offset += Model.Parts.LeftHand.Mesh->GetMeshDescription(0)->Vertices().Num();
    VerifyEquipment(Test, Mesh, *Description, Offset, *Model.Parts.Quiver.Mesh, TEXT("Backpack_Socket"));
    Offset += Model.Parts.Quiver.Mesh->GetMeshDescription(0)->Vertices().Num();
    Test.TestEqual(TEXT("All parts are present"), Description->Vertices().Num(), Offset);
    Test.TestEqual(TEXT("Original skeleton retained"), Mesh.GetSkeleton(), Model.Parts.Body->GetSkeleton());
    for (const FSkeletalMaterial& Material : Mesh.GetMaterials())
    {
        Test.TestNotNull(TEXT("Part material retained"), Material.MaterialInterface.Get());
    }
}
void VerifySavedAssembly(FAutomationTestBase& Test, UUnitMeshEditorModel& Model, USkeletalMesh& Mesh)
{
    const FString Directory = FPaths::ProjectSavedDir() / TEXT("Automation/UnitMeshEditor");
    IFileManager::Get().MakeDirectory(*Directory, true);
    const FString Filename = Directory / (Mesh.GetName() + TEXT(".uasset"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Filename);
    };
    FSavePackageArgs Arguments;
    Arguments.TopLevelFlags = RF_Public | RF_Standalone;
    if (!Test.TestTrue(TEXT("Mesh package saves"), UPackage::SavePackage(Mesh.GetOutermost(), &Mesh, *Filename, Arguments)))
    {
        return;
    }
    UPackage* ReloadedPackage = CreatePackage(*(TEXT("/Temp/") + Mesh.GetName() + TEXT("_Reloaded")));
    ReloadedPackage = LoadPackage(ReloadedPackage, *Filename, LOAD_None);
    USkeletalMesh* Reloaded = ReloadedPackage ? FindObject<USkeletalMesh>(ReloadedPackage, *Mesh.GetName()) : nullptr;
    if (!Test.TestNotNull(TEXT("Saved mesh reloads from disk"), Reloaded))
    {
        return;
    }
    Test.TestTrue(TEXT("Reloaded object is independent"), Reloaded != &Mesh);
    VerifyAssembly(Test, Model, *Reloaded);
    TStrongObjectPtr<UUnitMeshEditorModel> LoadedModel(NewObject<UUnitMeshEditorModel>());
    FText Error;
    Test.TestTrue(TEXT("Saved assembly recipe reloads"), LoadedModel->LoadAssembly(Reloaded, Error));
    Test.TestEqual(TEXT("Recipe retains selected head"), LoadedModel->Parts.Head.Get(), Model.Parts.Head.Get());
    Reloaded->ClearFlags(RF_Standalone);
    ResetLoaders(ReloadedPackage);
}
} // namespace

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnitMeshAssemblyTest,
                                 "TowerDefense.UnitMeshEditor.Assembly",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnitMeshAssemblyTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UUnitMeshEditorModel> Model(NewObject<UUnitMeshEditorModel>());
    if (!TestTrue(TEXT("FAB fixture assets exist"), LoadParts(*Model)))
    {
        return false;
    }
    FText Error;
    TStrongObjectPtr<USkeletalMesh> Mesh(Model->Build(Error));
    if (!TestNotNull(*Error.ToString(), Mesh.Get()))
    {
        return false;
    }
    VerifyAssembly(*this, *Model, *Mesh);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnitMeshValidationTest,
                                 "TowerDefense.UnitMeshEditor.Validation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnitMeshValidationTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UUnitMeshEditorModel> Model(NewObject<UUnitMeshEditorModel>());
    FText Error;
    TestNull(TEXT("Missing body rejected"), Model->Build(Error));
    if (!TestTrue(TEXT("FAB fixture assets exist"), LoadParts(*Model)))
    {
        return false;
    }
    Model->Parts.RightHand.SocketOrBone = TEXT("MissingSocket");
    TestNull(TEXT("Invalid attachment rejected"), Model->Build(Error));
    Model->Parts.RightHand.Mesh = nullptr;
    Model->Parts.LeftHand.Mesh = nullptr;
    Model->Parts.Quiver.Mesh = nullptr;
    Model->Parts.Head = nullptr;
    TStrongObjectPtr<USkeletalMesh> Mesh(Model->Build(Error));
    TestNotNull(TEXT("All optional parts can be empty"), Mesh.Get());
    Model->AssetName = TEXT("../Invalid");
    TestNull(TEXT("Invalid save path rejected"), Model->SaveNew(Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnitMeshSaveTest,
                                 "TowerDefense.UnitMeshEditor.SaveAndReload",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnitMeshSaveTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UUnitMeshEditorModel> Model(NewObject<UUnitMeshEditorModel>());
    if (!TestTrue(TEXT("FAB fixture assets exist"), LoadParts(*Model)))
    {
        return false;
    }
    Model->AssetName = TEXT("SK_Automation_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    FText Error;
    TStrongObjectPtr<USkeletalMesh> Mesh(Model->SaveNew(Error));
    if (!TestNotNull(*Error.ToString(), Mesh.Get()))
    {
        return false;
    }
    ON_SCOPE_EXIT
    {
        FAssetRegistryModule::AssetDeleted(Mesh.Get());
        Mesh->ClearFlags(RF_Standalone);
        Mesh->GetOutermost()->SetDirtyFlag(false);
    };
    TestNull(TEXT("Existing asset is never overwritten"), Model->SaveNew(Error));
    VerifySavedAssembly(*this, *Model, *Mesh);
    return true;
}

#endif
