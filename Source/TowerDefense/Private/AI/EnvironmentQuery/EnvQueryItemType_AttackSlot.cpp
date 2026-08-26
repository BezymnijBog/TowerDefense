// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/EnvironmentQuery/EnvQueryItemType_AttackSlot.h"

#include "AI/AttackSlot.h"

namespace
{
constexpr uint64 GAttackSlotStructSize = sizeof(FAttackSlot);
}

UEnvQueryItemType_AttackSlot::UEnvQueryItemType_AttackSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ValueSize = GAttackSlotStructSize;
}

FAttackSlot UEnvQueryItemType_AttackSlot::GetValue(const uint8* RawData)
{
    return GetValueFromMemory<FAttackSlot>(RawData);
}

void UEnvQueryItemType_AttackSlot::SetValue(uint8* RawData, const FAttackSlot& Value)
{
    return SetValueInMemory<FAttackSlot>(RawData, Value);
}

void UEnvQueryItemType_AttackSlot::SetContextHelper(FEnvQueryContextData& ContextData, const FAttackSlot& SingleSlot)
{
    ContextData.ValueType = StaticClass();
    ContextData.NumValues = 1;
    ContextData.RawData.SetNumUninitialized(GAttackSlotStructSize);

    SetValue(ContextData.RawData.GetData(), SingleSlot);
}

void UEnvQueryItemType_AttackSlot::SetContextHelper(FEnvQueryContextData& ContextData, const TArray<FAttackSlot>& MultipleSlots)
{
    const int32 NumItems = MultipleSlots.Num();
    ContextData.ValueType = StaticClass();
    ContextData.NumValues = NumItems;
    ContextData.RawData.SetNumUninitialized(GAttackSlotStructSize * NumItems);

    check(ContextData.RawData.Num() == GAttackSlotStructSize * NumItems);

    uint8* RawData = ContextData.RawData.GetData();
    for (const FAttackSlot& Slot : MultipleSlots)
    {
        SetValue(RawData, Slot);
        RawData += GAttackSlotStructSize;
    }
}

FVector UEnvQueryItemType_AttackSlot::GetItemLocation(const uint8* RawData) const
{
    return GetValue(RawData).Location;
}

FAttackSlot UEnvQueryItemType_AttackSlot::GetItemSlot(const uint8* RawData) const
{
    return GetValue(RawData);
}
