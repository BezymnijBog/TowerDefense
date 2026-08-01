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

protected:
    UPROPERTY(VisibleAnywhere, Category=Components)
    TObjectPtr<USplineComponent> SplineComponent;
};
