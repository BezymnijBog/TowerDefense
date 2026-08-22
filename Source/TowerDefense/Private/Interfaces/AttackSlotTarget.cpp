// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Interfaces/AttackSlotTarget.h"

// Add default functionality here for any IAttackSlotTarget functions that are not pure virtual.
TArray<FVector> IAttackSlotTarget::GetSlotPoints() const
{
    return TArray<FVector>();
}
