// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Equipment/UnitEquipmentDefinition.h"

#include "Attributes/AICharacterAttributeSet.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

bool UUnitEquipmentDefinition::Validate(EUnitEquipmentSlot ExpectedSlot, FText& OutError) const
{
    const bool bSkeletalSlot = Slot == EUnitEquipmentSlot::Body || Slot == EUnitEquipmentSlot::Head;
    if (Slot != ExpectedSlot || (bSkeletalSlot ? !SkeletalMesh : !StaticMesh) || !Offset.IsValid())
    {
        OutError =
            FText::Format(NSLOCTEXT("Equipment", "InvalidItem", "{0}: choose the correct slot, mesh and a valid transform."), FText::FromString(GetPathName()));
        return false;
    }
    for (const FUnitEquipmentModifier& Modifier : Modifiers)
    {
        if (!Modifier.Attribute.IsValid() || !Modifier.Attribute.GetAttributeSetClass()->IsChildOf(UAICharacterAttributeSet::StaticClass())
            || Modifier.Attribute == UAICharacterAttributeSet::GetHitPointsAttribute() || !FMath::IsFinite(Modifier.Bonus))
        {
            OutError = FText::Format(NSLOCTEXT("Equipment",
                                               "InvalidModifier",
                                               "{0}: bonuses require a unit attribute and a finite value. Use ExtraHitPoints instead of current HitPoints."),
                                     FText::FromString(GetPathName()));
            return false;
        }
    }
    return true;
}

TArray<const UUnitEquipmentDefinition*> FUnitEquipmentLoadout::GetItems() const
{
    return { Body, Head, RightHand, LeftHand, Quiver };
}

bool FUnitEquipmentLoadout::Validate(FText& OutError) const
{
    const TArray<const UUnitEquipmentDefinition*> Items = GetItems();
    for (int32 Index = 0; Index < Items.Num(); ++Index)
    {
        if (Items[Index] && !Items[Index]->Validate(static_cast<EUnitEquipmentSlot>(Index), OutError))
        {
            return false;
        }
    }
    return true;
}
