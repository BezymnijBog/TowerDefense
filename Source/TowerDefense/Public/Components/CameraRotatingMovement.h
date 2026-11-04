// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "CameraRotatingMovement.generated.h"

UCLASS()
class TOWERDEFENSE_API UCameraRotatingMovement : public URotatingMovementComponent
{
    GENERATED_BODY()

public:
    UCameraRotatingMovement();

    void AddRotationInput(const FRotator& DeltaRotation);

    float GetMaxPitch() const { return MaxPitch; }
    float GetMinPitch() const { return MinPitch; }

    virtual void StopMovementImmediately() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    FRotator ConsumeInputRotation();

protected:
    UPROPERTY(EditAnywhere, Category = "Camera|Rotation", meta = (ClampMin = "0.01", ClampMax = "10.0", Units = "s"))
    float InertiaDampingTime = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Camera|Rotation", meta = (ClampMin = "0.0001", Units = "deg/s"))
    float InertiaStopSpeed = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Camera|Rotation", meta = (ClampMin = "0.0", Units = "deg/s"))
    float MaxAngularSpeed = 1.f;

    UPROPERTY(EditAnywhere, Category = "Camera|Rotation", meta = (ClampMin = "0.0", Units = "deg/s"))
    float MaxPitch = 80.f;

    UPROPERTY(EditAnywhere, Category = "Camera|Rotation", meta = (ClampMin = "0.0", Units = "deg/s"))
    float MinPitch = 30.f;

private:
    FRotator GetClampedRotation(FRotator Input) const;
    void UpdateRotationRate(float DeltaTime);

    FRotator PendingRotationInput = FRotator::ZeroRotator;
};
