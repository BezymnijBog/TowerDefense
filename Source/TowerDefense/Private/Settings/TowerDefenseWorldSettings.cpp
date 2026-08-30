// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Settings/TowerDefenseWorldSettings.h"

const FVector2D& ATowerDefenseWorldSettings::GetGridStep() const
{
    return GridStep;
}

const FIntVector2& ATowerDefenseWorldSettings::GetGridSize() const
{
    return GridSize;
}

FIntVector2 ATowerDefenseWorldSettings::WorldToGridLocation(const FVector& WorldLocation) const
{
    return { FMath::RoundToInt32((WorldLocation - GridStart).Dot(FVector::ForwardVector) / GridStep.X),
             FMath::RoundToInt32((WorldLocation - GridStart).Dot(FVector::RightVector) / GridStep.Y) };
}

FVector ATowerDefenseWorldSettings::GridToWorldLocation(const FIntVector2& GridLocation) const
{
    return GridStart + FVector::ForwardVector * GridStep.X * GridLocation.X + FVector::RightVector * GridStep.Y * GridLocation.Y;
}
