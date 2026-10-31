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

void ACameraPawn::SetCameraPitch(float AngleBelowHorizon)
{
    FRotator Rotation = GetActorRotation();
    Rotation.Pitch = -FMath::Clamp(AngleBelowHorizon, CameraMinPitch, CameraMaxPitch);
    Rotation.Roll = 0.f;
    SetActorRotation(Rotation);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::AddMovement);

        EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::UpdateZoom);
        EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ThisClass::EndZoom);
        EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Canceled, this, &ThisClass::EndZoom);

        EnhancedInput->BindAction(PanMove, ETriggerEvent::Triggered, this, &ThisClass::UpdatePan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Started, this, &ThisClass::BeginPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Completed, this, &ThisClass::EndPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Canceled, this, &ThisClass::CancelPan);

        EnhancedInput->BindAction(PitchKeyboard, ETriggerEvent::Triggered, this, &ThisClass::UpdateKeyboardPitch);
        EnhancedInput->BindAction(PitchActivation, ETriggerEvent::Started, this, &ThisClass::BeginTilt);
        EnhancedInput->BindAction(PitchActivation, ETriggerEvent::Completed, this, &ThisClass::EndTilt);
        EnhancedInput->BindAction(PitchActivation, ETriggerEvent::Canceled, this, &ThisClass::EndTilt);
        EnhancedInput->BindAction(PitchMouse, ETriggerEvent::Triggered, this, &ThisClass::UpdateMousePitch);
    }
}

void ACameraPawn::AddMovement(const FInputActionValue& Input)
{
    if (MovementMode != ECameraMovementMode::None)
    {
        return;
    }
    checkf(Input.GetValueType() == EInputActionValueType::Axis3D, TEXT("Movement should be 3D axis movement"));
    const FVector MoveVector = Input.Get<FVector>();
    MoveRight(MoveVector.X);
    MoveForward(MoveVector.Y);
    MoveUp_World(MoveVector.Z);
}

void ACameraPawn::MoveForward(float Value)
{
    const FRotator HorizontalRotation(0.f, CameraComponent->GetComponentRotation().Yaw, 0.f);
    AddMovementInput(HorizontalRotation.Vector(), Value);
}

void ACameraPawn::MoveRight(float Value)
{
    const FRotationMatrix HorizontalRotation(FRotator(0.f, CameraComponent->GetComponentRotation().Yaw, 0.f));
    AddMovementInput(HorizontalRotation.GetUnitAxis(EAxis::Y), Value);
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();
    check(CameraMinPitch < CameraMaxPitch);
}

void ACameraPawn::UpdatePan(const FInputActionValue& Input)
{
    if (!EnumHasAnyFlags(MovementMode, ECameraMovementMode::Panning))
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

void ACameraPawn::UpdateKeyboardPitch(const FInputActionValue& Input)
{
    if (MovementMode == ECameraMovementMode::None)
    {
        SetCameraPitch(-GetActorRotation().Pitch + Input.Get<float>() * KeyboardPitchSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void ACameraPawn::UpdateMousePitch(const FInputActionValue& Input)
{
    BeginTilt();
    SetCameraPitch(-GetActorRotation().Pitch + Input.Get<float>() * MousePitchSensitivity);
}

void ACameraPawn::UpdateZoom(const FInputActionValue& Input)
{
    if (EnumHasAnyFlags(MovementMode, ~ECameraMovementMode::Zoom))
    {
        return;
    }
    MovementMode |= ECameraMovementMode::Zoom;
    CameraPawnMovement->BeginDirectMovement();
    const double HeightScale = FMath::Max(GetActorLocation().Z, 1.) / FMath::Max(ZoomReferenceHeight, 1.f);
    CameraPawnMovement->AddInputVector(FVector::UpVector * Input.Get<float>() * ZoomSensitivity * HeightScale);
}

void ACameraPawn::BeginTilt()
{
    if (!EnumHasAnyFlags(MovementMode, ECameraMovementMode::Tilting))
    {
        CancelPan();
        EndZoom();
        MovementMode |= ECameraMovementMode::Tilting;
    }
}

void ACameraPawn::EndTilt()
{
    MovementMode &= ~ECameraMovementMode::Tilting;
}

void ACameraPawn::BeginPan()
{
    const APlayerController* PlayerOwner = GetController<APlayerController>();
    if (MovementMode != ECameraMovementMode::None || !IsValid(PlayerOwner))
    {
        return;
    }

    CancelPan();
    if (GetPanPlane(PlayerOwner, PanPlane) && PlayerOwner->GetMousePosition(OldScreenPosition.X, OldScreenPosition.Y))
    {
        MovementMode |= ECameraMovementMode::Panning;
        CameraPawnMovement->BeginDirectMovement();
    }
}

void ACameraPawn::EndPan()
{
    MovementMode &= ~ECameraMovementMode::Panning;
    CameraPawnMovement->EndDirectMovement();
}

void ACameraPawn::CancelPan()
{
    MovementMode &= ~ECameraMovementMode::Panning;
    MovementComponent->StopMovementImmediately();
}

void ACameraPawn::EndZoom()
{
    MovementMode &= ~ECameraMovementMode::Zoom;
    CameraPawnMovement->EndDirectMovement();
}

bool ACameraPawn::ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const
{
    FVector Position;
    FVector Direction;
    if (!DeprojectScreenPositionToWorld(GetController<APlayerController>(), ScreenPosition, Position, Direction))
    {
        return false;
    }

    const double IntersectionDist = FMath::RayPlaneIntersectionParam(Position, Direction, PanPlane);
    WorldPosition = Position + Direction * IntersectionDist;
    return FMath::IsFinite(IntersectionDist) && IntersectionDist > 0;
}
