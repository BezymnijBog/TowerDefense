// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "EquipmentDefinitionDetails.h"

#include "Animation/Skeleton.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Equipment/UnitEquipmentDefinition.h"
#include "IDetailPropertyRow.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"

TSharedRef<IDetailCustomization> FEquipmentDefinitionDetails::MakeInstance()
{
    return MakeShared<FEquipmentDefinitionDetails>();
}

void FEquipmentDefinitionDetails::CustomizeDetails(IDetailLayoutBuilder& Builder)
{
    const TSharedRef<IPropertyHandle> Property = Builder.GetProperty(GET_MEMBER_NAME_CHECKED(UUnitEquipmentDefinition, SocketOrBone));
    if (IDetailPropertyRow* Row = Builder.EditDefaultProperty(Property))
    {
        Row->CustomWidget().NameContent()[Property->CreatePropertyNameWidget()].ValueContent().MinDesiredWidth(
            250.f)[PropertyCustomizationHelpers::MakePropertyComboBox(
            Property, FOnGetPropertyComboBoxStrings::CreateStatic(&FEquipmentDefinitionDetails::GetAttachmentOptions))];
    }
}

void FEquipmentDefinitionDetails::GetAttachmentOptions(TArray<TSharedPtr<FString>>& Options, TArray<TSharedPtr<SToolTip>>& Tooltips, TArray<bool>& Restricted)
{
    TArray<FString> Names;
    if (const USkeleton* Skeleton = LoadObject<USkeleton>(nullptr, TEXT("/Game/TowerDefense/Core/Meshes/SKEL_Units.SKEL_Units")))
    {
        for (const FMeshBoneInfo& Bone : Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo())
        {
            Names.AddUnique(Bone.Name.ToString());
        }
        for (const USkeletalMeshSocket* Socket : Skeleton->Sockets)
        {
            if (Socket)
            {
                Names.AddUnique(Socket->SocketName.ToString());
            }
        }
    }
    Names.Sort();
    Names.Remove(TEXT("None"));
    Names.Insert(TEXT("None"), 0);
    for (const FString& Name : Names)
    {
        Options.Add(MakeShared<FString>(Name));
        Tooltips.Add(nullptr);
        Restricted.Add(false);
    }
}
