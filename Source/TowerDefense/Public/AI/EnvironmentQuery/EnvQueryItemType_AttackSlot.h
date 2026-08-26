// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/AttackSlot.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvQueryItemType_AttackSlot.generated.h"

struct FEnvQueryContextData;

UCLASS()
class TOWERDEFENSE_API UEnvQueryItemType_AttackSlot : public UEnvQueryItemType_VectorBase
{
    GENERATED_BODY()
public:
    using FValueType = const FAttackSlot&;

    UEnvQueryItemType_AttackSlot(const FObjectInitializer& ObjectInitializer);

    static FAttackSlot GetValue(const uint8* RawData);
    static void SetValue(uint8* RawData, const FAttackSlot& Value);

    static void SetContextHelper(FEnvQueryContextData& ContextData, const FAttackSlot& SingleSlot);
    static void SetContextHelper(FEnvQueryContextData& ContextData, const TArray<FAttackSlot>& MultipleSlots);

    virtual FVector GetItemLocation(const uint8* RawData) const override;
    virtual FAttackSlot GetItemSlot(const uint8* RawData) const;
};
