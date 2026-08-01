// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/WayActor.h"

#include "Components/SplineComponent.h"

AWayActor::AWayActor()
{
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    SetRootComponent(SplineComponent);
}

USplineComponent* AWayActor::GetWay() const
{
    return SplineComponent;
}
