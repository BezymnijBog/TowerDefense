// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "AI/DefenderCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Subsystems/WorldGridSubsystem.h"
#include "Utils/WorldGridFunctionLibrary.h"
#include "Utils/BaseUtils.h"

void ADefenderCharacter::OnDeath()
{
    Super::OnDeath();
    if (UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this); bOccupiesStationaryGridCells && IsValid(Grid))
    {
        Grid->UnRegisterActor(this);
    }
}

const TArray<FIntVector2>& ADefenderCharacter::GetAdjacentCells() const
{
    return AdjacentCells;
}

const TArray<FIntVector2>& ADefenderCharacter::GetOccupiedCells() const
{
    return OccupiedCells;
}

FIntVector2 ADefenderCharacter::GetSize() const
{
    return { 1, 1 };
}

void ADefenderCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (!bOccupiesStationaryGridCells)
    {
        return;
    }
    AdjacentCells = UWorldGridFunctionLibrary::GetAdjacentCells(this, GetCapsuleComponent()->Bounds.GetBox());
    OccupiedCells = UWorldGridFunctionLibrary::GetOccupiedCells(this, GetCapsuleComponent()->Bounds.GetBox());
    if (UWorldGridSubsystem* Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(this); IsValid(Grid))
    {
        Grid->RegisterActor(this);
    }
}
