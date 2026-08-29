// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Settings/TowerDefenseWorldSettings.h"

const FIntVector2& ATowerDefenseWorldSettings::GetGridSize() const
{
    return GridSize;
}

FIntVector2 ATowerDefenseWorldSettings::WorldToGridLocation(const FVector& WorldLocation) const
{
    return { FMath::RoundToInt32((WorldLocation - GridStart).Dot(FVector::ForwardVector) / GridStep),
             FMath::RoundToInt32((WorldLocation - GridStart).Dot(FVector::RightVector) / GridStep) };
}

FVector ATowerDefenseWorldSettings::GridToWorldLocation(const FIntVector2& GridLocation) const
{
    return GridStart + FVector::ForwardVector * GridStep * GridLocation.X + FVector::RightVector * GridStep * GridLocation.Y;
}
