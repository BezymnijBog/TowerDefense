// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CellPlacedInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCellPlacedInterface : public UInterface
{
    GENERATED_BODY()
};

class TOWERDEFENSE_API ICellPlacedInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual const TArray<FIntVector2>& GetOccupiedCells() const = 0;
    virtual const TArray<FIntVector2>& GetAdjacentCells() const = 0;
    virtual FIntVector2 GetSize() const = 0;
};
