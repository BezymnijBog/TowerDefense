// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldGridSubsystem.generated.h"

class ATowerDefenseWorldSettings;

USTRUCT()
struct FGridCellInfo
{
    GENERATED_BODY()

    FGridCellInfo() = default;
    FGridCellInfo(const FIntVector2& GridCoordinate, const FVector& WorldCoordinate);

    UPROPERTY(EditAnywhere)
    FIntVector2 GridCoordinate = FIntVector2::ZeroValue;

    UPROPERTY(EditAnywhere)
    FVector WorldCoordinate = FVector::ZeroVector;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> Occupant = nullptr;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> ReservedBy = nullptr;

    UPROPERTY(EditAnywhere)
    TSet<TWeakObjectPtr<AActor>> PossibleTargets;
};

UCLASS()
class TOWERDEFENSE_API UWorldGridSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    void RegisterActor(AActor* Actor);
    void UnRegisterActor(AActor* Actor);

    FIntVector2 ClosestGridCell(const FVector& Location) const;
    const FVector& ClosestGridCellLocation(const FVector& Location) const;

    const FGridCellInfo* GetCellInfo(const FIntVector2& CellCoordinate) const;

private:
    void CacheSettingsFromWorld(const UWorld& InWorld);
    void CreateGrid();

    TWeakObjectPtr<const ATowerDefenseWorldSettings> WorldSettings;
    TMap<FIntVector2, FGridCellInfo> WorldGrid;
};
