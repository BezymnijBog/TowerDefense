// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldGridFunctionLibrary.generated.h"

class ATowerDefenseWorldSettings;
/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UWorldGridFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static const ATowerDefenseWorldSettings* GetWorldSettings(const UObject* WorldContext);
    static TArray<FIntVector2> GetOccupiedCells(const UObject* WorldContext, const FBox& BoundingBox);
    static TArray<FIntVector2> GetAdjacentCells(const UObject* WorldContext, const FBox& BoundingBox);
    static TPair<FIntVector2, FIntVector2> GetBoundCells(const UObject* WorldContext, const FBox& BoundingBox);
};
