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

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;

    AActor* GetClosestSensedActor() const;

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SelectNextTarget();

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AIController")
    FBlackboardKeys BlackboardKeys;

    UPROPERTY(VisibleInstanceOnly, Category = "AIController")
    bool bIsOffensive = false;

    FDelegateHandle TargetDeathHandle;

    bool IsBeingBeaten() const;
    void RemoveCurrentAttackTarget();
    void SetCurrentAttackTarget(AActor* NewTarget);
    bool HasValidTarget() const;
    AActor* GetCurrentTarget() const;
};
