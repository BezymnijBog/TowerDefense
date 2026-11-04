// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/CameraRotatingMovement.h"

UCameraRotatingMovement::UCameraRotatingMovement()
{
    RotationRate = FRotator::ZeroRotator;
    bRotationInLocalSpace = false;
}

void UCameraRotatingMovement::AddRotationInput(const FRotator& DeltaRotation)
{
    PendingRotationInput += DeltaRotation;
}

void UCameraRotatingMovement::StopMovementImmediately()
{
    Super::StopMovementImmediately();
    RotationRate = FRotator::ZeroRotator;
    PendingRotationInput = FRotator::ZeroRotator;
}

void UCameraRotatingMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateRotationRate(DeltaTime);
    MoveUpdatedComponent(FVector::ZeroVector, GetClampedRotation(UpdatedComponent->GetRelativeRotation() + RotationRate), false);
}

FRotator UCameraRotatingMovement::ConsumeInputRotation()
{
    const FRotator Temp = PendingRotationInput;
    PendingRotationInput = FRotator::ZeroRotator;
    return Temp;
}

FRotator UCameraRotatingMovement::GetClampedRotation(FRotator Input) const
{
    Input.Pitch = FMath::Clamp(Input.Pitch, -MaxPitch, -MinPitch);
    return Input;
}

void UCameraRotatingMovement::UpdateRotationRate(float DeltaTime)
{
    const FRotator Input = ConsumeInputRotation();
    if (Input.IsNearlyZero() && RotationRate.IsNearlyZero(InertiaStopSpeed))
    {
        RotationRate = FRotator::ZeroRotator;
        return;
    }

    if (Input.IsNearlyZero())
    {
        RotationRate *= FMath::Exp(-DeltaTime / InertiaDampingTime);
    }
    else
    {
        RotationRate = Input * MaxAngularSpeed;
    }
}
