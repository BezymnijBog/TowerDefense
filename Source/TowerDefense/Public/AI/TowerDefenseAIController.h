// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DetourCrowdAIController.h"
#include "TowerDefenseAIController.generated.h"

class ATowerDefenseAICharacter;
class UAISense;

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
class TOWERDEFENSE_API ATowerDefenseAIController : public ADetourCrowdAIController, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATowerDefenseAIController();

    virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    AActor* GetClosestSensedActor() const;

    FVector SelectNextPatrolPoint();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter")
    FBlackboardKeys BlackboardKeys;

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter")
    bool bIsOffensive = false;

private:
    bool IsBeingBeaten() const;
    void SelectNextTarget();
};
