// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WayComponent.generated.h"

class AWayActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERDEFENSE_API UWayComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    FVector GetNextTargetPoint() const;
    FVector GetClosestPoint() const;

    void SetWayActor(AWayActor* InActor);

protected:
    UPROPERTY(EditAnywhere, Category = WayComponent)
    TWeakObjectPtr<AWayActor> WayActor;
};
