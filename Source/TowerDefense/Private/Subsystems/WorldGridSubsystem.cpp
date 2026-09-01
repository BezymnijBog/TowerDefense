// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/WorldGridSubsystem.h"

#include "Interfaces/CellPlacedInterface.h"
#include "Interfaces/InterfaceFunctionLibrary.h"
#include "Settings/TowerDefenseWorldSettings.h"

FGridCellInfo::FGridCellInfo(const FIntVector2& GridCoordinate, const FVector& WorldCoordinate) :
    GridCoordinate(GridCoordinate), WorldCoordinate(WorldCoordinate)
{}

bool FGridCellInfo::IsAvailable() const
{
    return !Occupant.IsValid() && !ReservedBy.IsValid();
}

bool FGridCellInfo::TryReserveCell(AActor* Reserving)
{
    if (ReservedBy.IsValid() || !IsValid(Reserving))
    {
        return false;
    }

    ReservedBy = Reserving;
    return true;
}

void FGridCellInfo::FreeCell()
{
    ReservedBy = nullptr;
}

void UWorldGridSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    CacheSettingsFromWorld(InWorld);
    CreateGrid();
}

void UWorldGridSubsystem::RegisterActor(AActor* Actor)
{
    if (!Actor->Implements<UCellPlacedInterface>())
    {
        return;
    }

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
            FColor Color = Cell->Occupant.IsValid() ? FColor::Blue : FColor::Green;
            DrawDebugSphere(GetWorld(), Cell->WorldCoordinate, 40.f, 16, Color, false, 5.f);
        }
    }
}

void UWorldGridSubsystem::UnRegisterActor(AActor* Actor)
{
    if (!Actor->Implements<UCellPlacedInterface>())
    {
        return;
    }

    for (const FIntVector2& OccupiedCell : UInterfaceFunctionLibrary::GetOccupiedCells(Actor))
    {
        WorldGrid.FindChecked(OccupiedCell).Occupant = nullptr;
    }
    for (const FIntVector2& AdjacentCell : UInterfaceFunctionLibrary::GetAdjacentCells(Actor))
    {
        if (FGridCellInfo* const Cell = WorldGrid.Find(AdjacentCell); Cell)
        {
            Cell->PossibleTargets.Remove(Actor);
        }
    }
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

const FGridCellInfo* UWorldGridSubsystem::GetCellInfo(const FIntVector2& CellCoordinate) const
{
    return WorldGrid.Find(CellCoordinate);
}

bool UWorldGridSubsystem::TryReserveCell(const FGridCellInfo* CellInfo, AActor* ReserveBy)
{
    if (!CellInfo)
    {
        return false;
    }

    return WorldGrid.FindChecked(CellInfo->GridCoordinate).TryReserveCell(ReserveBy);
}

void UWorldGridSubsystem::FreeCell(const FGridCellInfo* CellInfo)
{
    if (CellInfo)
    {
        WorldGrid.FindChecked(CellInfo->GridCoordinate).FreeCell();
    }
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
