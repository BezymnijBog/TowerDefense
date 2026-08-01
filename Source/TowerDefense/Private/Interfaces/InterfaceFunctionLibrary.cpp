// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Interfaces/InterfaceFunctionLibrary.h"

#include "Components/WayComponent.h"
#include "Interfaces/AttackerInterface.h"
#include "Interfaces/DeathInterface.h"
#include "Interfaces/WayInterface.h"

bool UInterfaceFunctionLibrary::IsActorDead(const AActor* Actor)
{
    if (const IDeathInterface* const AsInterface = Cast<IDeathInterface>(Actor); AsInterface)
    {
        return AsInterface->IsDead();
    }

    return false;
}

float UInterfaceFunctionLibrary::GetAttackRange(const AActor* Actor)
{
    if (const IAttackerInterface* const AsInterface = Cast<IAttackerInterface>(Actor); AsInterface)
    {
        return AsInterface->GetAttackRange();
    }
    return 0.0f;
}

UWayComponent* UInterfaceFunctionLibrary::GetWayComponent(const AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return nullptr;
    }

    if (const IWayInterface* const AsInterface = Cast<IWayInterface>(Actor); AsInterface)
    {
        return AsInterface->GetWayComponent();
    }

    return Actor->GetComponentByClass<UWayComponent>();
}
