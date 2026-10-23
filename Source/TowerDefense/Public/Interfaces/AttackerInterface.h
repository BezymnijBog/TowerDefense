// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackerInterface.generated.h"

UENUM()
enum class EUnitAttackPattern : uint8
{
    Adjacent,
    TwoHanded,
    Ranged
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackerInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API IAttackerInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual float GetAttackRange() const = 0;
    virtual int32 GetAttackRadiusInCells() const = 0;
    virtual EUnitAttackPattern GetAttackPattern() const = 0;
    virtual bool CanAttackFrom(const FVector& Position, const AActor* Target) const = 0;
    virtual bool CanAttack(const AActor* Target) const = 0;
};
