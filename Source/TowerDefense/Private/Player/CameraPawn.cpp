// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Player/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CameraPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"

namespace
{
bool GetPanPlane(const APlayerController* Controller, FPlane& OutPlane)
{
    if (FHitResult Hit; IsValid(Controller) && Controller->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        OutPlane = { Hit.ImpactPoint, FVector::UpVector };
        return true;
    }
    return false;
}

bool DeprojectScreenPositionToWorld(const APlayerController* Controller, const FVector2D& ScreenPosition, FVector& Position, FVector& Direction)
{
    return IsValid(Controller) && Controller->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, Position, Direction);
}
} // namespace

ACameraPawn::ACameraPawn(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer.SetDefaultSubobjectClass(MovementComponentName, UCameraPawnMovement::StaticClass()))
{
    PrimaryActorTick.bCanEverTick = true;
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(GetRootComponent());

    CameraPawnMovement = CastChecked<UCameraPawnMovement>(MovementComponent);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::AddMovement);
        EnhancedInput->BindAction(PanMove, ETriggerEvent::Triggered, this, &ThisClass::UpdatePan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Started, this, &ThisClass::BeginPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Completed, this, &ThisClass::EndPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Canceled, this, &ThisClass::CancelPan);
    }
}

void ACameraPawn::AddMovement(const FInputActionValue& Input)
{
    if (bPanning)
    {
        return;
    }
    checkf(Input.GetValueType() == EInputActionValueType::Axis3D, TEXT("Movement should be 3D axis movement"));
    const FVector MoveVector = Input.Get<FVector>();
    MoveRight(MoveVector.X);
    MoveForward(MoveVector.Y);
    MoveUp_World(MoveVector.Z);
}

void ACameraPawn::UpdatePan(const FInputActionValue& Input)
{
    if (!bPanning)
    {
        BeginPan();
        return;
    }

    checkf(Input.GetValueType() == EInputActionValueType::Axis2D, TEXT("Pan should be 2D axis movement"));
    const FVector2D& InputDelta = Input.Get<FVector2D>();
    const FVector2D NewScreenPosition = OldScreenPosition - InputDelta;
    if (FVector OldWorldPos, NewWorldPos; ProjectToPanPlane(OldScreenPosition, OldWorldPos) && ProjectToPanPlane(NewScreenPosition, NewWorldPos))
    {
        CameraPawnMovement->AddInputVector(OldWorldPos - NewWorldPos);
        OldScreenPosition = NewScreenPosition;
    }
}

void ACameraPawn::BeginPan()
{
    const APlayerController* PlayerOwner = GetController<APlayerController>();
    if (bPanning || !IsValid(PlayerOwner))
    {
        return;
    }

    CancelPan();
    if (bPanning = GetPanPlane(PlayerOwner, PanPlane) && PlayerOwner->GetMousePosition(OldScreenPosition.X, OldScreenPosition.Y); bPanning)
    {
        CameraPawnMovement->BeginDirectMovement();
    }
}

void ACameraPawn::EndPan()
{
    bPanning = false;
    CameraPawnMovement->EndDirectMovement();
}

void ACameraPawn::CancelPan()
{
    bPanning = false;
    MovementComponent->StopMovementImmediately();
}

bool ACameraPawn::ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const
{
    FVector Position;
    FVector Direction;
    if (!DeprojectScreenPositionToWorld(GetController<APlayerController>(), ScreenPosition, Position, Direction))
    {
        return false;
    }

    WorldPosition = FMath::RayPlaneIntersection(Position, Direction, PanPlane);
    return true;
}
