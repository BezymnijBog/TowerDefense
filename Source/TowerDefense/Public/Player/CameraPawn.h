// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CameraPawn.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;

struct FInputActionValue;

UCLASS()
class TOWERDEFENSE_API ACameraPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    explicit ACameraPawn(const FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction> PanActivation;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction> PanMove;

private:
    void AddMovement(const FInputActionValue& Input);
    void UpdatePan(const FInputActionValue& Input);

    void BeginPan();
    void EndPan();
    bool ProjectToPanPlane(const FVector2D& ScreenPosition, FVector& WorldPosition) const;

    FPlane PanPlane;
    FVector2D OldScreenPosition = FVector2D::ZeroVector;
    bool bPanning = false;
};
