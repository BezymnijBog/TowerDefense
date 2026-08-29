// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/WorldGridSubsystem.h"

#include "Settings/TowerDefenseWorldSettings.h"

FGridCellInfo::FGridCellInfo(const FIntVector2& GridCoordinate, const FVector& WorldCoordinate) :
    GridCoordinate(GridCoordinate), WorldCoordinate(WorldCoordinate)
{}

void UWorldGridSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CacheSettingsFromWorld(InWorld);
    CreateGrid();
}

FIntVector2 UWorldGridSubsystem::ClosestGridCell(const FVector& Location) const
{
    check(WorldSettings.IsValid());
    return WorldSettings->WorldToGridLocation(Location);
}

const FVector& UWorldGridSubsystem::ClosestGridCellLocation(const FVector& Location) const
{
    if (const FGridCellInfo* const GridCell = WorldGrid.Find(ClosestGridCell(Location)); GridCell)
    {
        return GridCell->WorldCoordinate;
    }
    return FVector::ZeroVector;
}

void UWorldGridSubsystem::CacheSettingsFromWorld(const UWorld& InWorld)
{
    WorldSettings = Cast<ATowerDefenseWorldSettings>(InWorld.GetWorldSettings());
    check(WorldSettings.IsValid());
}

void UWorldGridSubsystem::CreateGrid()
{
    const FIntVector2& GridSize = WorldSettings->GetGridSize();
    WorldGrid.Reserve(GridSize.X * GridSize.Y);
    for (int32 IdX = -GridSize.X / 2; IdX < GridSize.X / 2; ++IdX)
    {
        for (int32 IdY = -GridSize.Y / 2; IdY < GridSize.Y / 2; ++IdY)
        {
            const FIntVector2 Coordinate(IdX, IdY);
            const FVector WorldLoc = WorldSettings->GridToWorldLocation(Coordinate);
            WorldGrid.Emplace(Coordinate, { Coordinate, WorldLoc });
            DrawDebugPoint(GetWorld(), WorldLoc, 2, FColor::Red, false, 5.);
        }
    }
}
