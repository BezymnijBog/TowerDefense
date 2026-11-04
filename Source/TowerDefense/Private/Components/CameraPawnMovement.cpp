// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Components/CameraPawnMovement.h"

void UCameraPawnMovement::BeginDirectMovement()
{
    StopMovementImmediately();
    bDirectMovement = true;
}

void UCameraPawnMovement::EndDirectMovement()
{
    bEndDirectMovement = bDirectMovement;
    if (GetPendingInputVector().IsNearlyZero())
    {
        bDirectMovement = false;
        bEndDirectMovement = false;
    }
}

float UCameraPawnMovement::ConsumeVerticalInput()
{
    const float Temp = VerticalInput;
    VerticalInput = 0.0f;
    return Temp;
}

FVector UCameraPawnMovement::ConsumeInputVector()
{
    const FVector Input = Super::ConsumeInputVector();
    VerticalInput += Input.Z;
    return { Input.X, Input.Y, 0.0 };
}

void UCameraPawnMovement::StopMovementImmediately()
{
    Super::StopMovementImmediately();
    ConsumeInputVector();
    bDirectMovement = false;
    bEndDirectMovement = false;
}

void UCameraPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
    if (bDirectMovement)
    {
        return ApplyDirectMovementControlInputToVelocity(DeltaTime);
    }
    if (!GetPendingInputVector().IsNearlyZero())
    {
        return Super::ApplyControlInputToVelocity(DeltaTime);
    }
    ApplyInertiaMovement(DeltaTime);
}

void UCameraPawnMovement::ApplyDirectMovementControlInputToVelocity(float DeltaTime)
{
    Velocity = ConsumeInputVector() / DeltaTime;
    if (bEndDirectMovement)
    {
        bDirectMovement = false;
        bEndDirectMovement = false;
    }
}

void UCameraPawnMovement::ApplyInertiaMovement(float DeltaTime)
{
    if (Velocity *= FMath::Exp(-DeltaTime / InertiaDampingTime); Velocity.IsNearlyZero(InertiaStopSpeed))
    {
        Velocity = FVector::ZeroVector;
    }
    ConsumeInputVector();
}
