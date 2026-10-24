// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "UnitMeshBuilder.h"

#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "SkeletalMeshAttributes.h"
#include "SkeletalMeshOperations.h"
#include "StaticToSkeletalMeshConverter.h"

namespace
{
FTransform GetBoneTransform(const FReferenceSkeleton& Skeleton, int32 BoneIndex)
{
    FTransform Result = Skeleton.GetRefBonePose()[BoneIndex];
    while ((BoneIndex = Skeleton.GetParentIndex(BoneIndex)) != INDEX_NONE)
    {
        Result *= Skeleton.GetRefBonePose()[BoneIndex];
    }
    return Result;
}

bool Fail(const FString& Message, FText& OutError)
{
    OutError = FText::FromString(Message);
    return false;
}

TSet<int32> GetRequiredBones(const FMeshDescription& Description, const FReferenceSkeleton& Skeleton)
{
    TSet<int32> Required;
    const FSkinWeightsVertexAttributesConstRef Weights = FSkeletalMeshConstAttributes(Description).GetVertexSkinWeights();
    for (const FVertexID Vertex : Description.Vertices().GetElementIDs())
    {
        for (const UE::AnimationCore::FBoneWeight Weight : Weights.Get(Vertex))
        {
            if (Weight.GetRawWeight() == 0)
            {
                continue;
            }
            for (int32 Bone = Weight.GetBoneIndex(); Bone != INDEX_NONE && !Required.Contains(Bone); Bone = Skeleton.GetParentIndex(Bone))
            {
                Required.Add(Bone);
            }
        }
    }
    return Required;
}

void AddMissingBones(const FReferenceSkeleton& Source, FReferenceSkeleton& Reference)
{
    FReferenceSkeletonModifier Modifier(Reference, nullptr);
    for (int32 Bone = 0; Bone < Source.GetRawBoneNum(); ++Bone)
    {
        const FName Name = Source.GetBoneName(Bone);
        if (Reference.FindRawBoneIndex(Name) == INDEX_NONE)
        {
            const int32 Parent = Source.GetParentIndex(Bone);
            const int32 TargetParent = Parent == INDEX_NONE ? INDEX_NONE : Reference.FindRawBoneIndex(Source.GetBoneName(Parent));
            Modifier.Add(FMeshBoneInfo(Name, Name.ToString(), TargetParent), Source.GetRefBonePose()[Bone]);
        }
    }
}

bool MergeReferencePose(const FUnitMeshParts& Parts, FReferenceSkeleton& Reference, FText& OutError)
{
    Reference = Parts.Body->GetRefSkeleton();
    AddMissingBones(Parts.Body->GetSkeleton()->GetReferenceSkeleton(), Reference);
    if (!Parts.Head)
    {
        return true;
    }
    const FMeshDescription* BodyDescription = Parts.Body->GetMeshDescription(0);
    const FMeshDescription* HeadDescription = Parts.Head->GetMeshDescription(0);
    if (!BodyDescription || !HeadDescription)
    {
        return Fail(TEXT("Body or head has no editable LOD0 geometry."), OutError);
    }
    const FReferenceSkeleton& HeadSkeleton = Parts.Head->GetRefSkeleton();
    const TSet<int32> BodyBones = GetRequiredBones(*BodyDescription, Parts.Body->GetRefSkeleton());
    const TSet<int32> HeadBones = GetRequiredBones(*HeadDescription, HeadSkeleton);
    FReferenceSkeletonModifier Modifier(Reference, nullptr);
    for (const int32 Bone : HeadBones)
    {
        const FName Name = HeadSkeleton.GetBoneName(Bone);
        const int32 Target = Reference.FindBoneIndex(Name);
        const FTransform& Pose = HeadSkeleton.GetRefBonePose()[Bone];
        if (Target == INDEX_NONE || (BodyBones.Contains(Target) && !Reference.GetRefBonePose()[Target].Equals(Pose, 0.01)))
        {
            return Fail(FString::Printf(TEXT("Body and head have conflicting bind poses at %s. Body: %s; Head: %s"),
                                        *Name.ToString(),
                                        Target == INDEX_NONE ? TEXT("missing") : *Reference.GetRefBonePose()[Target].ToString(),
                                        *Pose.ToString()),
                        OutError);
        }
        const int32 HeadParent = HeadSkeleton.GetParentIndex(Bone);
        const int32 TargetParent = Reference.GetParentIndex(Target);
        if ((HeadParent == INDEX_NONE) != (TargetParent == INDEX_NONE)
            || (HeadParent != INDEX_NONE && HeadSkeleton.GetBoneName(HeadParent) != Reference.GetBoneName(TargetParent)))
        {
            return Fail(TEXT("Body and head bone hierarchies differ."), OutError);
        }
        Modifier.UpdateRefPoseTransform(Target, Pose);
    }
    return true;
}

void AppendGeometry(const FMeshDescription& Source, FMeshDescription& Target, int32 BoneCount)
{
    FSkeletalMeshOperations::FSkeletalMeshAppendSettings SkinSettings;
    SkinSettings.SourceVertexIDOffset = Target.Vertices().GetArraySize();
    for (int32 BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
    {
        SkinSettings.SourceRemapBoneIndex.Add(BoneIndex);
    }
    FStaticMeshOperations::FAppendSettings Settings;
    for (bool& bMergeUV : Settings.bMergeUVChannels)
    {
        bMergeUV = true;
    }
    FStaticMeshOperations::AppendMeshDescription(Source, Target, Settings);
    FSkeletalMeshOperations::AppendSkinWeight(Source, Target, SkinSettings);
}

void RemapMaterials(FMeshDescription& Description, TConstArrayView<FSkeletalMaterial> Source, TArray<FSkeletalMaterial>& Materials)
{
    const TPolygonGroupAttributesRef<FName> Names = FStaticMeshAttributes(Description).GetPolygonGroupMaterialSlotNames();
    for (const FPolygonGroupID Group : Description.PolygonGroups().GetElementIDs())
    {
        int32 MaterialIndex = Source.IndexOfByPredicate(
            [&](const FSkeletalMaterial& Material) { return Material.ImportedMaterialSlotName == Names[Group] || Material.MaterialSlotName == Names[Group]; });
        if (MaterialIndex == INDEX_NONE)
        {
            MaterialIndex = Group.GetValue();
        }
        FSkeletalMaterial Material = Source.IsValidIndex(MaterialIndex) ? Source[MaterialIndex] : FSkeletalMaterial();
        const FName UniqueName(*FString::Printf(TEXT("PartMaterial_%d"), Materials.Num()));
        Material.MaterialSlotName = UniqueName;
        Material.ImportedMaterialSlotName = UniqueName;
        Names[Group] = UniqueName;
        Materials.Add(Material);
    }
}

bool RemapBones(FMeshDescription& Description, const FReferenceSkeleton& Source, const FReferenceSkeleton& Target, FText& OutError)
{
    TArray<int32> BoneMap;
    const TSet<int32> RequiredBones = GetRequiredBones(Description, Source);
    for (int32 BoneIndex = 0; BoneIndex < Source.GetRawBoneNum(); ++BoneIndex)
    {
        const FName BoneName = Source.GetBoneName(BoneIndex);
        const int32 TargetIndex = Target.FindBoneIndex(BoneName);
        if (TargetIndex == INDEX_NONE
            || (RequiredBones.Contains(BoneIndex) && !GetBoneTransform(Source, BoneIndex).Equals(GetBoneTransform(Target, TargetIndex), 0.01)))
        {
            return Fail(FString::Printf(TEXT("Incompatible reference pose for bone %s."), *BoneName.ToString()), OutError);
        }
        BoneMap.Add(TargetIndex);
    }
    if (!FSkeletalMeshOperations::RemapBoneIndicesOnSkinWeightAttribute(Description, BoneMap))
    {
        return Fail(TEXT("Unable to remap skin weights."), OutError);
    }
    return true;
}

bool AppendSkinned(USkeletalMesh& Source, const USkeletalMesh& Body, FMeshDescription& Target, TArray<FSkeletalMaterial>& Materials, FText& OutError)
{
    if (Source.GetSkeleton() != Body.GetSkeleton())
    {
        return Fail(TEXT("Body and head must use the same skeleton."), OutError);
    }
    const FMeshDescription* SourceDescription = Source.GetMeshDescription(0);
    if (!SourceDescription || SourceDescription->Triangles().Num() == 0)
    {
        return Fail(FString::Printf(TEXT("%s has no editable LOD0 geometry."), *Source.GetName()), OutError);
    }
    FMeshDescription Copy(*SourceDescription);
    FElementIDRemappings Remappings;
    Copy.Compact(Remappings);
    if (!RemapBones(Copy, Source.GetRefSkeleton(), Body.GetRefSkeleton(), OutError))
    {
        return false;
    }
    RemapMaterials(Copy, Source.GetMaterials(), Materials);
    AppendGeometry(Copy, Target, Body.GetRefSkeleton().GetRawBoneNum());
    return true;
}

bool AppendAttachment(const FUnitMeshAttachment& Part,
                      FName DefaultSocket,
                      const USkeletalMesh& Body,
                      FMeshDescription& Target,
                      TArray<FSkeletalMaterial>& Materials,
                      FText& OutError)
{
    if (!Part.Mesh)
    {
        return true;
    }
    int32 BoneIndex = INDEX_NONE;
    FTransform Transform;
    const FName Attachment = Part.SocketOrBone.IsNone() ? DefaultSocket : Part.SocketOrBone;
    if (!UnitMeshBuilder::ResolveAttachment(Body, Attachment, BoneIndex, Transform, OutError))
    {
        return false;
    }
    const FMeshDescription* Description = Part.Mesh->GetMeshDescription(0);
    if (!Description || Description->Triangles().Num() == 0 || !Part.Offset.IsValid())
    {
        return Fail(TEXT("Attachment has no editable LOD0 geometry or has an invalid offset."), OutError);
    }
    FMeshDescription Copy(*Description);
    FElementIDRemappings Remapping;
    Copy.Compact(Remapping);
    FSkeletalMeshAttributes Attributes(Copy);
    Attributes.Register();
    const TArray<UE::AnimationCore::FBoneWeight> Weights = { UE::AnimationCore::FBoneWeight(BoneIndex, 1.0f) };
    FSkinWeightsVertexAttributesRef SkinWeights = Attributes.GetVertexSkinWeights();
    for (const FVertexID Vertex : Copy.Vertices().GetElementIDs())
    {
        SkinWeights.Set(Vertex, Weights);
    }
    FStaticMeshOperations::ApplyTransform(Copy, Part.Offset * Transform, true);
    TArray<FSkeletalMaterial> SourceMaterials;
    for (const FStaticMaterial& Material : Part.Mesh->GetStaticMaterials())
    {
        SourceMaterials.Emplace(Material.MaterialInterface, true, false, Material.MaterialSlotName, Material.ImportedMaterialSlotName);
    }
    RemapMaterials(Copy, SourceMaterials, Materials);
    AppendGeometry(Copy, Target, Body.GetRefSkeleton().GetRawBoneNum());
    return true;
}

bool Assemble(
    const FUnitMeshParts& Parts, const USkeletalMesh& BindingMesh, FMeshDescription& Description, TArray<FSkeletalMaterial>& Materials, FText& OutError)
{
    FSkeletalMeshAttributes(Description).Register();
    if (!AppendSkinned(*Parts.Body, BindingMesh, Description, Materials, OutError))
    {
        return false;
    }
    if (Parts.Head && !AppendSkinned(*Parts.Head, BindingMesh, Description, Materials, OutError))
    {
        return false;
    }
    const bool bShield = Parts.LeftHand.Mesh && Parts.LeftHand.Mesh->GetName().Contains(TEXT("shield"), ESearchCase::IgnoreCase);
    return AppendAttachment(Parts.RightHand, TEXT("Weapon_Socket_R"), BindingMesh, Description, Materials, OutError)
        && AppendAttachment(Parts.LeftHand, bShield ? TEXT("Shield_Socket") : TEXT("Weapon_Socket_L"), BindingMesh, Description, Materials, OutError)
        && AppendAttachment(Parts.Quiver, TEXT("Backpack_Socket"), BindingMesh, Description, Materials, OutError);
}
} // namespace

bool UnitMeshBuilder::ResolveAttachment(const USkeletalMesh& Body, FName SocketOrBone, int32& OutBoneIndex, FTransform& OutTransform, FText& OutError)
{
    FName Bone = SocketOrBone;
    FTransform SocketTransform = FTransform::Identity;
    if (const USkeletalMeshSocket* Socket = Body.FindSocket(SocketOrBone))
    {
        Bone = Socket->BoneName;
        SocketTransform = Socket->GetSocketLocalTransform();
    }
    OutBoneIndex = Body.GetRefSkeleton().FindBoneIndex(Bone);
    if (OutBoneIndex == INDEX_NONE)
    {
        return Fail(FString::Printf(TEXT("Socket or bone '%s' is missing from the body."), *SocketOrBone.ToString()), OutError);
    }
    OutTransform = SocketTransform * GetBoneTransform(Body.GetRefSkeleton(), OutBoneIndex);
    return true;
}

USkeletalMesh* UnitMeshBuilder::Build(const FUnitMeshParts& Parts, FText& OutError)
{
    if (!Parts.Body || !Parts.Body->GetSkeleton())
    {
        Fail(TEXT("Select a body with a skeleton."), OutError);
        return nullptr;
    }
    FMeshDescription Description;
    TArray<FSkeletalMaterial> Materials;
    FReferenceSkeleton Reference;
    if (!MergeReferencePose(Parts, Reference, OutError))
    {
        return nullptr;
    }
    USkeletalMesh* Mesh = NewObject<USkeletalMesh>(GetTransientPackage(), NAME_None, RF_Transient);
    Mesh->SetSkeleton(Parts.Body->GetSkeleton());
    Mesh->SetRefSkeleton(Reference);
    for (const USkeletalMeshSocket* Socket : Parts.Body->GetMeshOnlySocketList())
    {
        Mesh->GetMeshOnlySocketList().Add(DuplicateObject<USkeletalMeshSocket>(Socket, Mesh));
    }
    if (!Assemble(Parts, *Mesh, Description, Materials, OutError))
    {
        return nullptr;
    }
    TArray<const FMeshDescription*> Descriptions = { &Description };
    FStaticToSkeletalMeshConverter::FInitializationParams Parameters;
    Parameters.Materials = Materials;
    if (!FStaticToSkeletalMeshConverter::InitializeSkeletalMeshFromMeshDescriptions(Mesh, Descriptions, Reference, Parameters))
    {
        Fail(TEXT("Unreal could not build the assembled skeletal mesh."), OutError);
        return nullptr;
    }
    Mesh->SetPhysicsAsset(Parts.Body->GetPhysicsAsset());
    return Mesh;
}
