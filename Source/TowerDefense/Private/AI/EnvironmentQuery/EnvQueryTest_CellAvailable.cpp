// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryTest_CellAvailable.h"

#include "AI/EnvironmentQuery/EnvQueryItemType_TargetCells.h"
#include "Subsystems/WorldGridSubsystem.h"

namespace
{
const FGridCellInfo* GetItemSlot(const FEnvQueryInstance& QueryInstance, int32 ItemIndex)
{
    const UEnvQueryItemType_TargetCells* ItemSlotCDO = QueryInstance.ItemType->GetDefaultObject<UEnvQueryItemType_TargetCells>();
    check(ItemSlotCDO);
    return ItemSlotCDO->GetCellInfo(QueryInstance.RawData.GetData() + QueryInstance.Items[ItemIndex].DataOffset);
}
} // namespace

UEnvQueryTest_CellAvailable::UEnvQueryTest_CellAvailable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Cost = EEnvTestCost::Low;
    SetWorkOnFloatValues(false);

    ValidItemType = UEnvQueryItemType_TargetCells::StaticClass();
}

void UEnvQueryTest_CellAvailable::RunTest(FEnvQueryInstance& QueryInstance) const
{
    const bool bIsAttackSlot = QueryInstance.ItemType && QueryInstance.ItemType->IsChildOf<UEnvQueryItemType_TargetCells>();
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
        const FGridCellInfo* const GridCell = GetItemSlot(QueryInstance, It.GetIndex());
        if (const bool bIsAvailable = GridCell->IsAvailable(); bIsAvailable)
        {
            It.SetScore(TestPurpose, FilterType, bIsAvailable, bWantsValid);
        }
        else
        {
            It.ForceItemState(IncompatibleStatus);
        }
    }
}

FText UEnvQueryTest_CellAvailable::GetDescriptionDetails() const
{
    return FText::FromString(TEXT("Check if grid cell is available"));
}
