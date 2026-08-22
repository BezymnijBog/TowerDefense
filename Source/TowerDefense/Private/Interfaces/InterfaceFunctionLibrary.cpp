// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Interfaces/InterfaceFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Actors/Projectile.h"
#include "Components/WayComponent.h"
#include "Interfaces/AttackerInterface.h"
#include "Interfaces/DeathInterface.h"
#include "Interfaces/HireElementInterface.h"
#include "Interfaces/ShooterInterface.h"
#include "Interfaces/WayInterface.h"

FDeathDelegate& UInterfaceFunctionLibrary::GetDeathDelegate(AActor* Actor)
{
    static FDeathDelegate DummyDelegate;
    if (IDeathInterface* const AsInterface = Cast<IDeathInterface>(Actor); AsInterface)
    {
        return AsInterface->GetDeathDelegate();
    }

    if (IsValid(Actor))
    {
        UE_LOG(LogActor, Error, TEXT("Input actor was nullptr"));
    }
    else
    {
        UE_LOG(LogActor, Error, TEXT("%s doesn't implement DeathInterface"), *Actor->GetName());
    }
    return DummyDelegate;
}

void UInterfaceFunctionLibrary::CallOnDeath(AActor* Actor)
{
    if (IDeathInterface* const AsInterface = Cast<IDeathInterface>(Actor); AsInterface)
    {
        return AsInterface->OnDeath();
    }
}

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

UAbilitySystemComponent* UInterfaceFunctionLibrary::GetAbilitySystemComponent(const AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return nullptr;
    }

    if (const IAbilitySystemInterface* const AsInterface = Cast<IAbilitySystemInterface>(Actor); AsInterface)
    {
        return AsInterface->GetAbilitySystemComponent();
    }

    return Actor->GetComponentByClass<UAbilitySystemComponent>();
}

void UInterfaceFunctionLibrary::ActivateProjectile(AActor* Actor)
{
    if (IShooterInterface* AsInterface = Cast<IShooterInterface>(Actor); AsInterface)
    {
        AsInterface->ActivateProjectile();
    }
}

void UInterfaceFunctionLibrary::LaunchActiveProjectile(AActor* Shooter)
{
    if (IShooterInterface* AsInterface = Cast<IShooterInterface>(Shooter); AsInterface)
    {
        AsInterface->LaunchActiveProjectile();
    }
}

bool UInterfaceFunctionLibrary::GetHireInfo(UObject* Object, FHireInfo& Info)
{
    return Object->Implements<UHireElementInterface>() ? Cast<IHireElementInterface>(Object)->GetHireInfo(Info) : false;
}
