// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CameraPawn.generated.h"

class UCameraRotatingMovement;
class UCameraPawnMovement;
class UCameraComponent;
class UInputAction;
class USpringArmComponent;

struct FInputActionValue;

UCLASS()
class TOWERDEFENSE_API ACameraPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    explicit ACameraPawn(const FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void MoveForward(float Value) override;
    virtual void MoveRight(float Value) override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom", meta = (ClampMin = "0", Units = "cm"))
    float ZoomSensitivity = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom", meta = (ClampMin = "1", Units = "cm"))
    float ZoomReferenceHeight = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMin = "0"))
    float KeyboardPitchSpeed = 40.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMin = "0"))
    float MousePitchSensitivity = 0.15f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMin = "15"))
    float CameraMinPitch = 30.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMax = "85"))
    float CameraMaxPitch = 80.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMin = "0"))
    float SpringArmMinLength = 1500.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Pitch", meta = (ClampMin = "0", ClampMax = "10000"))
    float SpringArmMaxLength = 5500.f;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraPawnMovement> CameraPawnMovement;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraRotatingMovement> CameraPawnRotation;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PanActivation;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PanMove;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> RotationAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> ZoomAction;

    float VerticalSpeed = 0.0f;
    FRotator RotationInput = FRotator::ZeroRotator;

private:
    void AddMovement(const FInputActionValue& Input);
    void UpdateRotation(const FInputActionValue& Input);
    void AddZoom(const FInputActionValue& Input);

    void BeginPan();
    void EndPan();
    void CancelPan();
    void UpdatePan(const FInputActionValue& Input);

    void UpdateVerticalSpeed(float DeltaSeconds);
    void UpdateVerticalOffset(float DeltaSeconds);

    bool ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const;

    FPlane PanPlane;
    FVector2D OldScreenPosition = FVector2D::ZeroVector;
    bool bIsPanning = false;
};
