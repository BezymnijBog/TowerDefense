// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryTest_SlotAvailable.h"

#include "AI/EnvironmentQuery/EnvQueryItemType_AttackSlot.h"

namespace
{
FAttackSlot GetItemSlot(const FEnvQueryInstance& QueryInstance, int32 ItemIndex)
{
    const UEnvQueryItemType_AttackSlot* ItemSlotCDO = QueryInstance.ItemType->GetDefaultObject<UEnvQueryItemType_AttackSlot>();
    check(ItemSlotCDO);
    return ItemSlotCDO->GetItemSlot(QueryInstance.RawData.GetData() + QueryInstance.Items[ItemIndex].DataOffset);
}
} // namespace

UEnvQueryTest_SlotAvailable::UEnvQueryTest_SlotAvailable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Cost = EEnvTestCost::Low;
    SetWorkOnFloatValues(false);

    ValidItemType = UEnvQueryItemType_AttackSlot::StaticClass();
}

void UEnvQueryTest_SlotAvailable::RunTest(FEnvQueryInstance& QueryInstance) const
{
    const bool bIsAttackSlot = QueryInstance.ItemType && QueryInstance.ItemType->IsChildOf<UEnvQueryItemType_AttackSlot>();
    const UObject* QueryOwner = QueryInstance.Owner.Get();
    if (!IsValid(QueryOwner) || !bIsAttackSlot)
    {
        return;
    }

    BoolValue.BindData(QueryOwner, QueryInstance.QueryID);
    const bool bWantsValid = BoolValue.GetValue();

    const EEnvItemStatus::Type IncompatibleStatus = bRejectOccupiedSlots ? EEnvItemStatus::Failed : EEnvItemStatus::Passed;

    for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
    {
        GetItemSlot(QueryInstance, It.GetIndex());
        if (const bool bIsAvailable = !UEnvQueryItemType_AttackSlot::GetValue(QueryInstance.RawData.GetData()).bIsAcquired; bIsAvailable)
        {
            It.SetScore(TestPurpose, FilterType, bIsAvailable, bWantsValid);
        }
        else
        {
            It.ForceItemState(IncompatibleStatus);
        }
    }
}

FText UEnvQueryTest_SlotAvailable::GetDescriptionDetails() const
{
    return FText::FromString(TEXT("Check if attack slot is available"));
}
