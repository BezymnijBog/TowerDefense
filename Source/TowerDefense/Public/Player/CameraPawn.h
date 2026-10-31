// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CameraPawn.generated.h"

class UCameraPawnMovement;
class UCameraComponent;
class UInputAction;

struct FInputActionValue;

UCLASS()
class TOWERDEFENSE_API ACameraPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    explicit ACameraPawn(const FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
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

private:
    void AddMovement(const FInputActionValue& Input);
    void UpdatePan(const FInputActionValue& Input);

    void BeginPan();
    void EndPan();
    void CancelPan();
    bool ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const;

    FPlane PanPlane;
    FVector2D OldScreenPosition = FVector2D::ZeroVector;
    bool bPanning = false;
};
