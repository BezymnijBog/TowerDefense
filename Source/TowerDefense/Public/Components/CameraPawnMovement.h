// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "CameraPawnMovement.generated.h"

UCLASS()
class TOWERDEFENSE_API UCameraPawnMovement : public UFloatingPawnMovement
{
    GENERATED_BODY()

public:
    void BeginDirectMovement();
    void EndDirectMovement();

    virtual void StopMovementImmediately() override;

protected:
    virtual void ApplyControlInputToVelocity(float DeltaTime) override;

    void ApplyDirectMovementControlInputToVelocity(float DeltaTime);
    void ApplyInertiaMovement(float DeltaTime);

    UPROPERTY(EditAnywhere, Category = "Camera|Movement", meta = (ClampMin = "0.01", ClampMax = "10.0", Units = "s"))
    float InertiaDampingTime = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Camera|Movement", meta = (ClampMin = "0.0001", ClampMax = "10.0", Units = "cm/s"))
    float InertiaStopSpeed = 1.0f;

private:
    bool bDirectMovement = false;
    bool bEndDirectMovement = false;
};
