// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DetourCrowdAIController.h"
#include "Interfaces/AttackerInterface.h"
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
class TOWERDEFENSE_API ATowerDefenseAIController :
    public ADetourCrowdAIController,
    public IAbilitySystemInterface,
    public IDeathInterface,
    public IAttackerInterface
{
    GENERATED_BODY()

public:
    ATowerDefenseAIController();

    virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;

    virtual float GetAttackRange() const override;
    virtual int32 GetAttackRadiusInCells() const override;
    virtual EUnitAttackPattern GetAttackPattern() const override;
    virtual bool CanAttackFrom(const FVector& Position, const AActor* Target) const override;
    virtual bool CanAttack(const AActor* Target) const override;

    AActor* GetClosestSensedActor() const;
    bool CanAttackCurrentTarget() const;

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SelectNextTarget();

    UPROPERTY(EditDefaultsOnly, Category = "AIController", meta = (ClampMin = "0.1"))
    float PositionRefreshInterval = 0.5f;

    FTimerHandle PositionRefreshTimer;

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
