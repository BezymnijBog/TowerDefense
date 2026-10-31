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
    Velocity = Velocity.GetClampedToMaxSize(GetMaxSpeed()) * FMath::Exp(-DeltaTime / InertiaDampingTime);
    if (Velocity.IsNearlyZero(InertiaStopSpeed))
    {
        Velocity = FVector::ZeroVector;
    }
    ConsumeInputVector();
}
