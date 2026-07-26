// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "EnemyController.generated.h"

class UAITask_MoveTo;

UCLASS()
class TOWERDEFENSE_API AEnemyController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyController();

    void SetPathSpline(const USplineComponent* Spline);

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EnemyController")
    TObjectPtr<const USplineComponent> SplinePath;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyController")
    float AcceptanceRadius = 20.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EnemyController")
    float SplineLength = 0.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EnemyController")
    float CharacterSpeed = 0.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EnemyController")
    float MovedDistance = 0.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EnemyController")
    FVector EndPoint = FVector::ZeroVector;
};
