// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/WayComponent.h"

#include "Actors/WayActor.h"
#include "Components/SplineComponent.h"

FVector UWayComponent::GetNextTargetPoint() const
{
    if (!WayActor.IsValid())
    {
        return FVector::ZeroVector;
    }

    const USplineComponent* const Way = WayActor->GetWay();
    const float NextPointInputKey = FMath::RoundHalfToEven(Way->FindInputKeyClosestToWorldLocation(GetOwner()->GetActorLocation())) + 1.f;
    const FVector Offset = Way->GetRotationAtSplineInputKey(NextPointInputKey, ESplineCoordinateSpace::Local).RotateVector(SpawnOffset);
    const FVector Result = Way->GetLocationAtSplineInputKey(NextPointInputKey, ESplineCoordinateSpace::World) + Offset;
    return Result;
}

FVector UWayComponent::GetClosestPoint() const
{
    if (!WayActor.IsValid())
    {
        return FVector::ZeroVector;
    }
    const USplineComponent* const Way = WayActor->GetWay();
    const FVector OwnerLocation = GetOwner()->GetActorLocation();
    const FVector Offset = Way->FindRotationClosestToWorldLocation(OwnerLocation, ESplineCoordinateSpace::Local).RotateVector(SpawnOffset);
    return Way->FindLocationClosestToWorldLocation(OwnerLocation, ESplineCoordinateSpace::World) + Offset;
}

void UWayComponent::SetSpawnOffset(const FVector& Offset)
{
    SpawnOffset = Offset;
}

void UWayComponent::SetWayActor(AWayActor* InActor)
{
    WayActor = InActor;
}
