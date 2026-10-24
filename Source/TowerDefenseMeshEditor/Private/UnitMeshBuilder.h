// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UnitMeshAssembly.h"

struct FMeshDescription;
struct FReferenceSkeleton;
struct FSkeletalMaterial;

namespace UnitMeshBuilder
{
USkeletalMesh* Build(const FUnitMeshParts& Parts, FText& OutError);
bool ResolveAttachment(const USkeletalMesh& Body, FName SocketOrBone, int32& OutBoneIndex, FTransform& OutTransform, FText& OutError);
} // namespace UnitMeshBuilder
