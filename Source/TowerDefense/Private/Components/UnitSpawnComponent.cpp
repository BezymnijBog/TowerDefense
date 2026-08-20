// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/UnitSpawnComponent.h"

#include "AI/TowerDefenseAICharacter.h"

void UUnitSpawnComponent::EnqueueSpawn(TSubclassOf<ATowerDefenseAICharacter> NewClass)
{
    UnitClass = MoveTemp(NewClass);
    TrySpawnUnit();
}

void UUnitSpawnComponent::SetSpawnInterval(float NewInterval)
{
    SpawnInterval = NewInterval;
}

bool UUnitSpawnComponent::CanSpawnUnit() const
{
    return !bIsOccupied && !Handle.IsValid() && IsValid(UnitClass);
}

void UUnitSpawnComponent::BeginPlay()
{
    Super::BeginPlay();
    OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnEndOverlap);
}

void UUnitSpawnComponent::SpawnUnit()
{
    ATowerDefenseAICharacter* const SpawnedUnit = GetWorld()->SpawnActorDeferred<ATowerDefenseAICharacter>(UnitClass, GetComponentTransform());
    LastSpawnedActor = SpawnedUnit;
    bIsOccupied = IsValid(SpawnedUnit);
    UnitClass = nullptr;

    const float TimeRate = SpawnInterval * FMath::FRandRange(1.f - TimeRandomDeviation, 1.f + TimeRandomDeviation);
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::OnTimerElapsed, TimeRate);
    std::ignore = UnitStartSpawn.ExecuteIfBound(SpawnedUnit);
}

void UUnitSpawnComponent::OnTimerElapsed()
{
    GetWorld()->GetTimerManager().ClearTimer(Handle);
    TrySpawnUnit();
}

void UUnitSpawnComponent::TrySpawnUnit()
{
    if (CanSpawnUnit())
    {
        SpawnUnit();
    }
}

void UUnitSpawnComponent::OnEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int)
{
    if (OtherActor == LastSpawnedActor)
    {
        bIsOccupied = false;
        TrySpawnUnit();
    }
}
