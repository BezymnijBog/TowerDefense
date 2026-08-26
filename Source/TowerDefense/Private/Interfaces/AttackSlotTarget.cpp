// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Interfaces/AttackSlotTarget.h"

#include "AI/AttackSlot.h"

// Add default functionality here for any IAttackSlotTarget functions that are not pure virtual.
TArray<FAttackSlot> IAttackSlotTarget::GetSlotPoints() const
{
    return TArray<FAttackSlot>();
}
