// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/AttackSlot.h"
#include "DeathInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "InterfaceFunctionLibrary.generated.h"

class AProjectile;
class UAbilitySystemComponent;
class UAIPerceptionComponent;
class UWayComponent;

struct FHireInfo;

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

    static UAIPerceptionComponent* GetPerceptionComponent(AActor* Listener);

    static TArray<FAttackSlot> GetAttackSlots(const AActor* Target);

    static void ActivateProjectile(AActor* Actor);
    static void LaunchActiveProjectile(AActor* Shooter);

    static bool GetHireInfo(UObject* Object, FHireInfo& Info);

    static TArray<FIntVector2> GetOccupiedCells(const AActor* Actor);
    static TArray<FIntVector2> GetAdjacentCells(const AActor* Actor);
    static FIntVector2 GetSize(const AActor* Actor);

    template <std::derived_from<UInterface> TInterface>
    static TArray<AActor*> GetAllActorsWithInterface(const UObject* WorldContext);
};

template <std::derived_from<UInterface> TInterface>
TArray<AActor*> UInterfaceFunctionLibrary::GetAllActorsWithInterface(const UObject* WorldContext)
{
    TArray<AActor*> Result;
    UGameplayStatics::GetAllActorsWithInterface(WorldContext, TInterface::StaticClass(), Result);
    return Result;
}
