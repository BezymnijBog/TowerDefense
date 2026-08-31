// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DetourCrowdAIController.h"
#include "Interfaces/DeathInterface.h"
#include "TowerDefenseAIController.generated.h"

class ATowerDefenseAICharacter;
class UAISense;
class UEnvQuery;

struct FEnvQueryResult;

USTRUCT()
struct FBlackboardKeys
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName AttackRange;

    UPROPERTY(EditAnywhere)
    FName AttackTarget;

    UPROPERTY(EditAnywhere)
    FName DestinationPoint;
};

UCLASS()
class TOWERDEFENSE_API ATowerDefenseAIController : public ADetourCrowdAIController, public IAbilitySystemInterface, public IDeathInterface
{
    GENERATED_BODY()

public:
    ATowerDefenseAIController();

    virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;

    AActor* GetClosestSensedActor() const;

    FVector SelectNextPatrolPoint();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    TObjectPtr<UEnvQuery> SearchSlotQuery;

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    FBlackboardKeys BlackboardKeys;

    UPROPERTY(VisibleInstanceOnly, Category = "AIController")
    bool bIsOffensive = false;

    FDelegateHandle TargetDeathHandle;

private:
    bool IsBeingBeaten() const;
    void OnQueryComplete(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> EnvQueryResult);
    void SelectNextTarget();

    void RemoveCurrentAttackTarget();
    void SetCurrentAttackTarget(AActor* NewTarget);
    bool HasValidTarget() const;
};
