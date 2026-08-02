// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "DeathInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InterfaceFunctionLibrary.generated.h"

class UAbilitySystemComponent;
class UWayComponent;

UCLASS()
class TOWERDEFENSE_API UInterfaceFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FDeathDelegate& GetDeathDelegate(AActor* Actor);
    static void CallOnDeath(AActor* Actor);
    static bool IsActorDead(const AActor* Actor);

    static float GetAttackRange(const AActor* Actor);

    static UWayComponent* GetWayComponent(const AActor* Actor);

    static UAbilitySystemComponent* GetAbilitySystemComponent(const AActor* Actor);
};
