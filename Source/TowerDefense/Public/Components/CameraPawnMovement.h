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

    float GetMaxHeight() const { return MaxHeight; }
    float GetMinHeight() const { return MinHeight; }

    virtual void StopMovementImmediately() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void ApplyControlInputToVelocity(float DeltaTime) override;

    void ApplyDirectMovementControlInputToVelocity(float DeltaTime);
    void ApplyInertiaMovement(float DeltaTime);

    UPROPERTY(EditAnywhere, Category = "Camera|Movement", meta = (ClampMin = "0.01", ClampMax = "10.0", Units = "s"))
    float InertiaDampingTime = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Camera|Movement", meta = (ClampMin = "0.0001", ClampMax = "10.0", Units = "cm/s"))
    float InertiaStopSpeed = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Camera|Height", meta = (Units = "cm"))
    float MinHeight = 100.f;

    UPROPERTY(EditAnywhere, Category = "Camera|Height", meta = (Units = "cm"))
    float MaxHeight = 10000.f;

private:
    void ClampMovementHeight();

    bool bDirectMovement = false;
    bool bEndDirectMovement = false;
};
