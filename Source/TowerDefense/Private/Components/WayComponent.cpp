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
    const float NextPointInputKey = FMath::Floor(Way->FindInputKeyClosestToWorldLocation(GetOwner()->GetActorLocation())) + 1.f;
    return Way->GetLocationAtSplineInputKey(NextPointInputKey, ESplineCoordinateSpace::World);
}

FVector UWayComponent::GetClosestPoint() const
{
    if (!WayActor.IsValid())
    {
        return FVector::ZeroVector;
    }
    return WayActor->GetWay()->FindLocationClosestToWorldLocation(GetOwner()->GetActorLocation(), ESplineCoordinateSpace::World);
}

void UWayComponent::SetWayActor(AWayActor* InActor)
{
    WayActor = InActor;
}
