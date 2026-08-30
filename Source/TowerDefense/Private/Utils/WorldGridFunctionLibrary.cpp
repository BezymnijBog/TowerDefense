// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Utils/WorldGridFunctionLibrary.h"

#include "Settings/TowerDefenseWorldSettings.h"

const ATowerDefenseWorldSettings* UWorldGridFunctionLibrary::GetWorldSettings(const UObject* WorldContext)
{
    check(GEngine);
    const UWorld* const World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
    return IsValid(World) ? Cast<ATowerDefenseWorldSettings>(World->GetWorldSettings()) : nullptr;
}

TArray<FIntVector2> UWorldGridFunctionLibrary::GetOccupiedCells(const UObject* WorldContext, const FBox& BoundingBox)
{
    const TPair<FIntVector2, FIntVector2> BoundCells = GetBoundCells(WorldContext, BoundingBox);
    const FIntVector2& MinCell = BoundCells.Key;
    const FIntVector2& MaxCell = BoundCells.Value;
    TArray<FIntVector2> Result;
    Result.Reserve((MaxCell.X - MinCell.X + 1) * (MaxCell.Y - MinCell.Y + 1));
    for (int32 IdX = MinCell.X; IdX <= MaxCell.X; ++IdX)
    {
        for (int32 IdY = MinCell.Y; IdY <= MaxCell.Y; ++IdY)
        {
            Result.Emplace(IdX, IdY);
        }
    }
    return Result;
}

TArray<FIntVector2> UWorldGridFunctionLibrary::GetAdjacentCells(const UObject* WorldContext, const FBox& BoundingBox)
{
    const TPair<FIntVector2, FIntVector2> BoundCells = GetBoundCells(WorldContext, BoundingBox);
    const FIntVector2& MinCell = BoundCells.Key;
    const FIntVector2& MaxCell = BoundCells.Value;
    const FIntVector2 CellsNum = MaxCell - MinCell + FIntVector2(1, 1);
    TArray<FIntVector2> Result;
    Result.Reserve(2 * (CellsNum.X + 1) + 2 * (CellsNum.Y + 1));
    for (int32 IdX = 0; IdX <= CellsNum.X; ++IdX)
    {
        Result.Emplace(MinCell.X - 1 + IdX, MinCell.Y - 1);
        Result.Emplace(MaxCell.X + 1 - IdX, MaxCell.Y + 1);
    }
    for (int32 IdY = 0; IdY <= CellsNum.Y; ++IdY)
    {
        Result.Emplace(MaxCell.X + 1, MinCell.Y - 1 + IdY);
        Result.Emplace(MinCell.X - 1, MaxCell.Y + 1 - IdY);
    }
    return Result;
}

TPair<FIntVector2, FIntVector2> UWorldGridFunctionLibrary::GetBoundCells(const UObject* WorldContext, const FBox& BoundingBox)
{
    const ATowerDefenseWorldSettings* const Settings = GetWorldSettings(WorldContext);
    if (!IsValid(Settings))
    {
        return { FIntVector2{ 0, 0 }, FIntVector2{ 0, 0 } };
    }

    const FVector2D& GridStep = Settings->GetGridStep();
    const FVector CellCornerToCenter = FVector::ForwardVector * GridStep.X * 0.5 + FVector::RightVector * GridStep.Y * 0.5;
    return { Settings->WorldToGridLocation(BoundingBox.Min + CellCornerToCenter), Settings->WorldToGridLocation(BoundingBox.Max - CellCornerToCenter) };
}
