// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AI/TowerDefenseAIController.h"
#include "EnemyController.generated.h"

struct FGridCellInfo;
/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API AEnemyController : public ATowerDefenseAIController
{
    GENERATED_BODY()

public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    virtual void OnDeath() override;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SelectNextTarget() override;

    FVector SelectNextPatrolPoint();
    void OnQueryComplete(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult);
    void RunEnvironmentQuery();
    void ReleaseCombatCell();
    AActor* FindTargetFrom(const FVector& Position) const;

    int32 PendingQueryId = INDEX_NONE;
    const FGridCellInfo* FailedCell = nullptr;
    double FailedCellRetryTime = 0.0;

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    TObjectPtr<UEnvQuery> SearchCellQuery;

    const FGridCellInfo* ActiveCell = nullptr;
};
