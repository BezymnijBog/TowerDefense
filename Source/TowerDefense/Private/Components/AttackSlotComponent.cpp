// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/AttackSlotComponent.h"

#include "AI/AttackSlot.h"

UAttackSlotComponent::UAttackSlotComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAttackSlotComponent::SetBaseTransform(const FTransform& NewTransform)
{
    BaseTransform = NewTransform;
}

void UAttackSlotComponent::InitializeSlotsLocal(const TArray<FVector>& SlotLocations)
{
    Slots.Empty(SlotLocations.Num());
    Algo::Transform(SlotLocations, Slots, [this](const FVector& Location) { return FAttackSlot(BaseTransform.TransformPosition(Location), GetOwner()); });
    for (const FAttackSlot& Slot : Slots)
    {
        DrawDebugSphere(GetWorld(), Slot.Location, 34.f, 16, FColor::Red, false, 5.f);
    }
}

bool UAttackSlotComponent::TryAcquireSlot(const FGuid& SlotGuid)
{
    FAttackSlot* const Slot = Slots.FindByPredicate([&SlotGuid](const FAttackSlot& Slot) { return Slot.Guid == SlotGuid; });
    if (!Slot || Slot->bIsAcquired)
    {
        return false;
    }

    Slot->bIsAcquired = true;
    return true;
}

const TArray<FAttackSlot>& UAttackSlotComponent::GetSlots() const
{
    return Slots;
}

void UAttackSlotComponent::BeginPlay()
{
    Super::BeginPlay();
}
