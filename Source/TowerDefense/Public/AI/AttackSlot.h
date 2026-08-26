// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AttackSlot.generated.h"

USTRUCT()
struct FAttackSlot
{
    GENERATED_BODY()

    explicit FAttackSlot(const FVector& Location, AActor* Owner);
    FAttackSlot() = default;

    UPROPERTY(EditAnywhere)
    FVector Location;

    UPROPERTY(EditAnywhere)
    FGuid Guid = FGuid::NewGuid();

    UPROPERTY(EditAnywhere)
    bool bIsAcquired = false;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> Owner = nullptr;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> Occupant = nullptr;
};
