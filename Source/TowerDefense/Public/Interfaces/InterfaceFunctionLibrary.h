// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "DeathInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InterfaceFunctionLibrary.generated.h"

class AProjectile;
class UAbilitySystemComponent;
class UAIPerceptionComponent;
class UWayComponent;
enum class EUnitAttackPattern : uint8;
struct FHireInfo;

UCLASS()
class TOWERDEFENSE_API UInterfaceFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FDeathDelegate& GetDeathDelegate(AActor* Actor);
    static void CallOnDeath(AActor* Actor);
    static bool IsActorDead(const AActor* Actor);
    static bool IsValidAndAlive(const AActor* Actor);

    static float GetAttackRange(const AActor* Actor);
    static int32 GetAttackRadiusInCells(const AActor* Actor);
    static EUnitAttackPattern GetAttackPattern(const AActor* Actor);
    static bool CanAttackFrom(const AActor* Actor, const FVector& Position, const AActor* Target);
    static bool CanAttack(const AActor* Actor, const AActor* Target);

    static UWayComponent* GetWayComponent(const AActor* Actor);

    static UAbilitySystemComponent* GetAbilitySystemComponent(const AActor* Actor);

    static UAIPerceptionComponent* GetPerceptionComponent(AActor* Listener);

    static void ActivateProjectile(AActor* Actor);
    static void LaunchActiveProjectile(AActor* Shooter);

    static bool GetHireInfo(UObject* Object, FHireInfo& Info);

    static const TArray<FIntVector2>& GetOccupiedCells(const AActor* Actor);
    static const TArray<FIntVector2>& GetAdjacentCells(const AActor* Actor);
    static FIntVector2 GetSize(const AActor* Actor);

    static bool CanStartConstruction(const AActor* Actor);
    static void StartConstruction(AActor* Actor);
};
