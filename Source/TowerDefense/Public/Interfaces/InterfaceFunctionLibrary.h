// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InterfaceFunctionLibrary.generated.h"

class UWayComponent;

UCLASS()
class TOWERDEFENSE_API UInterfaceFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static bool IsActorDead(const AActor* Actor);
    static float GetAttackRange(const AActor* Actor);
    static UWayComponent* GetWayComponent(const AActor* Actor);
};
