// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvQueryItemType_TargetCells.generated.h"

struct FGridCellInfo;
struct FEnvQueryContextData;

UCLASS()
class TOWERDEFENSE_API UEnvQueryItemType_TargetCells : public UEnvQueryItemType_VectorBase
{
    GENERATED_BODY()
public:
    using FValueType = const FGridCellInfo*;

    UEnvQueryItemType_TargetCells(const FObjectInitializer& ObjectInitializer);

    static const FGridCellInfo* GetValue(const uint8* RawData);
    static void SetValue(uint8* RawData, const FGridCellInfo* Value);

    static void SetContextHelper(FEnvQueryContextData& ContextData, const FGridCellInfo* SingleSlot);
    static void SetContextHelper(FEnvQueryContextData& ContextData, const TArray<const FGridCellInfo*>& MultipleSlots);

    virtual FVector GetItemLocation(const uint8* RawData) const override;
    virtual const FGridCellInfo* GetCellInfo(const uint8* RawData) const;
};
