// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Player/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CameraPawnMovement.h"
#include "Components/CameraRotatingMovement.h"
#include "Components/SphereComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"

namespace
{
bool DeprojectScreenPositionToWorld(const APlayerController* Controller, const FVector2D& ScreenPosition, FVector& Position, FVector& Direction)
{
    return IsValid(Controller) && Controller->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, Position, Direction);
}
} // namespace

ACameraPawn::ACameraPawn(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer.SetDefaultSubobjectClass(MovementComponentName, UCameraPawnMovement::StaticClass()))
{
    PrimaryActorTick.bCanEverTick = true;
    bAddDefaultMovementBindings = false;

    GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCollisionComponent()->SetGenerateOverlapEvents(false);
    SetActorEnableCollision(false);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    CameraPawnMovement = CastChecked<UCameraPawnMovement>(MovementComponent);
    CameraPawnRotation = CreateDefaultSubobject<UCameraRotatingMovement>(TEXT("Camera Rotation Component"));
    CameraPawnRotation->SetUpdatedComponent(CameraBoom);
    CameraBoom->AddTickPrerequisiteComponent(CameraPawnMovement);
    CameraBoom->AddTickPrerequisiteComponent(CameraPawnRotation);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent); IsValid(EnhancedInput))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::AddMovement);
        EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::AddZoom);

        EnhancedInput->BindAction(PanMove, ETriggerEvent::Triggered, this, &ThisClass::UpdatePan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Started, this, &ThisClass::BeginPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Completed, this, &ThisClass::EndPan);
        EnhancedInput->BindAction(PanActivation, ETriggerEvent::Canceled, this, &ThisClass::CancelPan);

        EnhancedInput->BindAction(RotationAction, ETriggerEvent::Triggered, this, &ThisClass::UpdateRotation);
    }
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

void ACameraPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateVerticalSpeed(DeltaSeconds);
    UpdateVerticalOffset(DeltaSeconds);
}

void ACameraPawn::AddMovement(const FInputActionValue& Input)
{
    checkf(Input.GetValueType() == EInputActionValueType::Axis3D, TEXT("Movement should be 3D axis movement"));
    const FVector MoveVector = Input.Get<FVector>();
    MoveRight(MoveVector.X);
    MoveForward(MoveVector.Y);
    MoveUp_World(MoveVector.Z);
}

void ACameraPawn::UpdateRotation(const FInputActionValue& Input)
{
    checkf(Input.GetValueType() == EInputActionValueType::Axis2D, TEXT("Rotation should be 2D axis movement"));
    const FVector2D RotationVector = Input.Get<FVector2D>();
    CameraPawnRotation->AddRotationInput({ RotationVector.Y, RotationVector.X, 0.0 });
}

void ACameraPawn::AddZoom(const FInputActionValue& Input)
{
    CameraPawnMovement->BeginDirectMovement();
    const double CameraHeight = -CameraBoom->GetTargetRotation().Vector().Z * CameraBoom->TargetArmLength;
    const double HeightScale = CameraHeight / ZoomReferenceHeight;
    AddMovementInput(FVector::UpVector, Input.Get<float>() * HeightScale * ZoomSensitivity);
    CameraPawnMovement->EndDirectMovement();
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();
    check(CameraMinPitch < CameraMaxPitch);
    CameraPawnRotation->SetUpdatedComponent(CameraBoom);
    PanPlane = FPlane(GetActorLocation(), FVector::UpVector);
}

void ACameraPawn::BeginPan()
{
    const APlayerController* PlayerOwner = GetController<APlayerController>();
    if (!bIsPanning || !IsValid(PlayerOwner))
    {
        return;
    }

    CancelPan();
    if (PlayerOwner->GetMousePosition(OldScreenPosition.X, OldScreenPosition.Y))
    {
        bIsPanning = true;
        CameraPawnMovement->BeginDirectMovement();
    }
}

void ACameraPawn::EndPan()
{
    bIsPanning = false;
    CameraPawnMovement->EndDirectMovement();
}

void ACameraPawn::CancelPan()
{
    bIsPanning = false;
    MovementComponent->StopMovementImmediately();
}

void ACameraPawn::UpdatePan(const FInputActionValue& Input)
{
    if (!bIsPanning)
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

void ACameraPawn::UpdateVerticalSpeed(float DeltaSeconds)
{
    const float VerticalInput = CameraPawnMovement->ConsumeVerticalInput();
    if (FMath::IsNearlyZero(VerticalSpeed) && FMath::IsNearlyZero(VerticalInput))
    {
        VerticalSpeed = 0.0f;
        return;
    }

    if (FMath::IsNearlyZero(VerticalInput))
    {
        VerticalSpeed *= FMath::Exp(-DeltaSeconds * 5);
    }
    else
    {
        VerticalSpeed = VerticalInput * CameraPawnMovement->GetMaxSpeed();
    }
}

void ACameraPawn::UpdateVerticalOffset(float DeltaSeconds)
{
    if (FMath::IsNearlyZero(VerticalSpeed))
    {
        return;
    }

    FRotator Rotation = CameraBoom->GetTargetRotation();
    const FVector CurrentOffset = -Rotation.Vector() * CameraBoom->TargetArmLength;

    const double RadiusSqr = CurrentOffset.SizeSquared2D();
    const double Radius = FMath::Sqrt(RadiusSqr);

    const double DesiredHeight = CurrentOffset.Z + VerticalSpeed * DeltaSeconds;
    const double DesiredPitch = -FMath::RadiansToDegrees(FMath::Atan2(DesiredHeight, Radius));

    Rotation.Pitch = FMath::Clamp(DesiredPitch, -CameraPawnRotation->GetMaxPitch(), -CameraPawnRotation->GetMinPitch());
    double Length = FMath::Sqrt(RadiusSqr + DesiredHeight * DesiredHeight);

    if (Rotation.Pitch != DesiredPitch)
    {
        const double BoundaryHeight = -Radius * FMath::Tan(FMath::DegreesToRadians(Rotation.Pitch));
        const double BoundaryLength = FMath::Sqrt(RadiusSqr + BoundaryHeight * BoundaryHeight);
        Length = BoundaryLength + (DesiredHeight - BoundaryHeight);
    }

    CameraBoom->TargetArmLength = FMath::Clamp(Length, SpringArmMinLength, SpringArmMaxLength);
    CameraBoom->SetWorldRotation(Rotation);
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
