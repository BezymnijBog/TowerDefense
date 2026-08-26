// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSlotComponent.generated.h"

struct FAttackSlot;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERDEFENSE_API UAttackSlotComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttackSlotComponent();

    void SetBaseTransform(const FTransform& NewTransform);
    void InitializeSlotsLocal(const TArray<FVector>& SlotLocations);
    bool TryAcquireSlot(const FGuid& SlotGuid);
    const TArray<FAttackSlot>& GetSlots() const UE_LIFETIMEBOUND;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleInstanceOnly, Category = Slot)
    FTransform BaseTransform;

    UPROPERTY(VisibleInstanceOnly, Category = Slot)
    TArray<FAttackSlot> Slots;
};
