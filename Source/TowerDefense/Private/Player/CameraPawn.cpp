// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Player/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetSystemLibrary.h"

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

ACameraPawn::ACameraPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(GetRootComponent());
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::AddMovement);
        EnhancedInput->BindAction(PanMove, ETriggerEvent::Triggered, this, &ThisClass::UpdatePan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Started, this, &ThisClass::BeginPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Completed, this, &ThisClass::EndPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Canceled, this, &ThisClass::EndPan);
    }
}

void ACameraPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
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
    checkf(Input.GetValueType() == EInputActionValueType::Axis2D, TEXT("Pan should be 2D axis movement"));
    const FVector2D& InputDelta = Input.Get<FVector2D>();
    const FVector2D NewScreenPosition = OldScreenPosition - InputDelta;
    if (FVector OldWorldPos, NewWorldPos; ProjectToPanPlane(OldScreenPosition, OldWorldPos) && ProjectToPanPlane(NewScreenPosition, NewWorldPos))
    {
        AddActorWorldOffset(OldWorldPos - NewWorldPos);
        OldScreenPosition = NewScreenPosition;
    }
}

void ACameraPawn::BeginPan()
{
    const APlayerController* PlayerOwner = GetController<APlayerController>();
    if (!IsValid(PlayerOwner)) [[unlikely]]
    {
        return;
    }

    if (bPanning = GetPanPlane(PlayerOwner, PanPlane) && PlayerOwner->GetMousePosition(OldScreenPosition.X, OldScreenPosition.Y); bPanning)
    {
        GetMovementComponent()->StopMovementImmediately();
        ConsumeMovementInputVector();
    }
}

void ACameraPawn::EndPan()
{
    bPanning = false;
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
