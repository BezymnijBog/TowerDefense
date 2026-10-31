// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CameraPawn.generated.h"

class UCameraPawnMovement;
class UCameraComponent;
class UInputAction;

struct FInputActionValue;

enum class ECameraMovementMode : uint8
{
    None = 0,
    Panning = 1 << 0,
    Tilting = 1 << 1,
    Zoom = 1 << 2
};
ENUM_CLASS_FLAGS(ECameraMovementMode);

UCLASS()
class TOWERDEFENSE_API ACameraPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    explicit ACameraPawn(const FObjectInitializer& ObjectInitializer);

    void SetCameraPitch(float AngleBelowHorizon);

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void MoveForward(float Value) override;
    virtual void MoveRight(float Value) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom", meta = (ClampMin = "0", Units = "cm"))
    float ZoomSensitivity = 100.f;

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

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraPawnMovement> CameraPawnMovement;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PanActivation;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PanMove;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PitchKeyboard;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PitchActivation;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PitchMouse;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> ZoomAction;

private:
    void AddMovement(const FInputActionValue& Input);
    void UpdatePan(const FInputActionValue& Input);
    void UpdateKeyboardPitch(const FInputActionValue& Input);
    void UpdateMousePitch(const FInputActionValue& Input);
    void UpdateZoom(const FInputActionValue& Input);

    void BeginTilt();
    void EndTilt();

    void BeginPan();
    void EndPan();
    void CancelPan();

    void EndZoom();

    bool ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const;

    FPlane PanPlane;
    FVector2D OldScreenPosition = FVector2D::ZeroVector;
    ECameraMovementMode MovementMode = ECameraMovementMode::None;
};
