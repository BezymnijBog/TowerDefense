// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Enemy/EnemyController.h"

#include "GameFramework/PawnMovementComponent.h"
#include "TowerDefense.h"

AEnemyController::AEnemyController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AEnemyController::SetPathSpline(const USplineComponent* Spline)
{
    SplinePath = Spline;
    if (IsValid(SplinePath))
    {
        SetActorTickEnabled(true);
        SplineLength = SplinePath->GetSplineLength();
        EndPoint = SplinePath->GetWorldLocationAtDistanceAlongSpline(SplineLength);
    }
    else
    {
        SetActorTickEnabled(false);
        SplineLength = 0.f;
        EndPoint = FVector::ZeroVector;
    }
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    CharacterSpeed = InPawn->GetMovementComponent()->GetMaxSpeedForNavMovement();
}

void AEnemyController::OnUnPossess()
{
    Super::OnUnPossess();
    CharacterSpeed = 0.f;
}

void AEnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if ((GetPawn()->GetActorLocation() - EndPoint).SquaredLength() < AcceptanceRadius * AcceptanceRadius)
    {
        UE_LOG(LogTowerDefense, Display, TEXT("%s reached end of spline"), *GetPawn()->GetName());
        GetPawn()->Destroy();
        SetActorTickEnabled(false);
        return;
    }

    MovedDistance = FMath::Min(MovedDistance + CharacterSpeed * DeltaSeconds, SplineLength);
    MoveToLocation(SplinePath->GetWorldLocationAtDistanceAlongSpline(MovedDistance), AcceptanceRadius - 10., false);
}
