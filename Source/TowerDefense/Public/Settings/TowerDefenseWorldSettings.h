// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "TowerDefenseWorldSettings.generated.h"

UCLASS()
class TOWERDEFENSE_API ATowerDefenseWorldSettings : public AWorldSettings
{
    GENERATED_BODY()

public:
    const FIntVector2& GetGridSize() const;
    FIntVector2 WorldToGridLocation(const FVector& WorldLocation) const;
    FVector GridToWorldLocation(const FIntVector2& GridLocation) const;

protected:
    UPROPERTY(EditAnywhere, Category = WorldGrid, meta = (MakeEditWidget))
    FVector GridStart = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = WorldGrid, meta = (MakeEditWidget))
    FIntVector2 GridSize = { 100, 100 };

    UPROPERTY(EditAnywhere, Category = WorldGrid, meta = (MakeEditWidget))
    double GridStep = 100.;
};
