

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBanVolume.generated.h"

class UBuildingComponent;

UCLASS()
class TOWERDEFENSE_API ABuildingBanVolume : public AActor
{
    GENERATED_BODY()

public:
    ABuildingBanVolume();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBuildingComponent> BuildingComponent;
};
