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
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SelectNextTarget() override;

    FVector SelectNextPatrolPoint();
    void OnQueryComplete(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult);
    void RunEnvironmentQuery();

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    TObjectPtr<UEnvQuery> SearchCellQuery;

    const FGridCellInfo* ActiveCell = nullptr;
};
