// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/WorldGridSubsystem.h"

#include "Interfaces/CellPlacedInterface.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Settings/TowerDefenseWorldSettings.h"

FGridCellInfo::FGridCellInfo(const FIntVector2& GridCoordinate, const FVector& WorldCoordinate) :
    GridCoordinate(GridCoordinate), WorldCoordinate(WorldCoordinate)
{}

void UWorldGridSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CacheSettingsFromWorld(InWorld);
    CreateGrid();
    InitializePlacedActors(InWorld);
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
        }
    }
}

void UWorldGridSubsystem::InitializePlacedActors(const UWorld& InWorld)
{
    for (const TArray<AActor*> Actors = UInterfaceFunctionLibrary::GetAllActorsWithInterface<UCellPlacedInterface>(&InWorld); AActor* const Actor : Actors)
    {
        for (const TArray<FIntVector2> OccupiedCells = UInterfaceFunctionLibrary::GetOccupiedCells(Actor); const FIntVector2& OccupiedCell : OccupiedCells)
        {
            FGridCellInfo& Cell = WorldGrid.FindChecked(OccupiedCell);
            check(!Cell.Occupant.IsValid());
            Cell.Occupant = Actor;
        }
        for (const TArray<FIntVector2> AdjacentCells = UInterfaceFunctionLibrary::GetAdjacentCells(Actor); const FIntVector2& AdjacentCell : AdjacentCells)
        {
            if (FGridCellInfo* const Cell = WorldGrid.Find(AdjacentCell); Cell)
            {
                Cell->PossibleTargets.Emplace(Actor);
                DrawDebugSphere(&InWorld, Cell->WorldCoordinate, 40.f, 16, FColor::Red, false, 5.f);
            }
        }
    }
}
