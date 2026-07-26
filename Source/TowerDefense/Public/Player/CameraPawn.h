// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CameraPawn.generated.h"

class UCameraComponent;
class UInputAction;

struct FInputActionValue;

UCLASS()
class TOWERDEFENSE_API ACameraPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    explicit ACameraPawn(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

private:
    void AddMovement(const FInputActionValue& Input);
};
