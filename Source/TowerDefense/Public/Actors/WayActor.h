// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WayActor.generated.h"

class USplineComponent;

UCLASS()
class TOWERDEFENSE_API AWayActor : public AActor
{
    GENERATED_BODY()

public:
    AWayActor();

    USplineComponent* GetWay() const;

    bool CanSpawnUnit(const FVector& SpawnLocation) const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, Category = Components)
    TObjectPtr<USplineComponent> SplineComponent;

    UPROPERTY(EditAnywhere, Category = WayActor)
    double MaxSpawnDistance = 100.;
};
