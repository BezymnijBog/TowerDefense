

#include "Actors/BuildingBanVolume.h"

#include "Components/BuildingComponent.h"

ABuildingBanVolume::ABuildingBanVolume()
{
    BuildingComponent = CreateDefaultSubobject<UBuildingComponent>(TEXT("BuildingComponent"));
    BuildingComponent->InitBoxExtent(100. * FVector::OneVector);
}
