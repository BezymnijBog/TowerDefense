// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryItemType_TargetCells.h"

#include "Subsystems/WorldGridSubsystem.h"

namespace
{
constexpr uint64 GAttackSlotStructSize = sizeof(const FGridCellInfo*);
}

UEnvQueryItemType_TargetCells::UEnvQueryItemType_TargetCells(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ValueSize = GAttackSlotStructSize;
}

const FGridCellInfo* UEnvQueryItemType_TargetCells::GetValue(const uint8* RawData)
{
    return GetValueFromMemory<const FGridCellInfo*>(RawData);
}

void UEnvQueryItemType_TargetCells::SetValue(uint8* RawData, const FGridCellInfo* Value)
{
    return SetValueInMemory<const FGridCellInfo*>(RawData, Value);
}

void UEnvQueryItemType_TargetCells::SetContextHelper(FEnvQueryContextData& ContextData, const FGridCellInfo* SingleSlot)
{
    ContextData.ValueType = StaticClass();
    ContextData.NumValues = 1;
    ContextData.RawData.SetNumUninitialized(GAttackSlotStructSize);

    SetValue(ContextData.RawData.GetData(), SingleSlot);
}

void UEnvQueryItemType_TargetCells::SetContextHelper(FEnvQueryContextData& ContextData, const TArray<FValueType>& MultipleSlots)
{
    const int32 NumItems = MultipleSlots.Num();
    ContextData.ValueType = StaticClass();
    ContextData.NumValues = NumItems;
    ContextData.RawData.SetNumUninitialized(GAttackSlotStructSize * NumItems);

    check(ContextData.RawData.Num() == GAttackSlotStructSize * NumItems);

    uint8* RawData = ContextData.RawData.GetData();
    for (const FValueType& Slot : MultipleSlots)
    {
        SetValue(RawData, Slot);
        RawData += GAttackSlotStructSize;
    }
}

FVector UEnvQueryItemType_TargetCells::GetItemLocation(const uint8* RawData) const
{
    return GetValue(RawData)->WorldCoordinate;
}

const FGridCellInfo* UEnvQueryItemType_TargetCells::GetCellInfo(const uint8* RawData) const
{
    return GetValue(RawData);
}
