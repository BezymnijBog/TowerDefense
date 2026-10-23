// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/AttackerInterface.h"
#include "Interfaces/DeathInterface.h"
#include "TowerDefenseAICharacter.generated.h"

class ATowerDefenseAIController;
class UAICharacterAttributeSet;
class UBehaviorTree;
class UProgressBarWidget;
class UWayComponent;
class UWidgetComponent;

struct FDefaultAbilityDescription;

UCLASS()
class TOWERDEFENSE_API ATowerDefenseAICharacter :
    public ACharacter,
    public IGenericTeamAgentInterface,
    public IAbilitySystemInterface,
    public IAttackerInterface,
    public IDeathInterface
{
    GENERATED_BODY()

public:
    ATowerDefenseAICharacter(const FObjectInitializer& ObjectInitializer);

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UWayComponent* GetWayComponent() const;

    static bool IsCellInAttackRange(EUnitAttackPattern Pattern, const FIntVector2& Delta, int32 Radius);

    virtual float GetAttackRange() const override;
    virtual EUnitAttackPattern GetAttackPattern() const override;
    virtual int32 GetAttackRadiusInCells() const override;
    virtual bool CanAttackFrom(const FVector& Position, const AActor* Target) const override;
    virtual bool CanAttack(const AActor* Target) const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;
    FDeathDelegate DeathDelegate;

protected:
    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UWayComponent> WayComponent;

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter|Abilities")
    TObjectPtr<const UAICharacterAttributeSet> AttributeSet;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter|Abilities")
    TArray<FDefaultAbilityDescription> DefaultAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter")
    FGenericTeamId TeamId;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter")
    float DeathDestroyInterval = 5.f;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter")
    float AttackRange = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter|Combat")
    EUnitAttackPattern AttackPattern = EUnitAttackPattern::Adjacent;

    UPROPERTY(EditDefaultsOnly, Category = "AICharacter|Combat", meta = (ClampMin = "1", ClampMax = "20"))
    int32 AttackRadiusInCells = 1;

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter")
    TWeakObjectPtr<ATowerDefenseAIController> AICharacterController;

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter")
    bool bIsDead = false;

private:
    void OnDeathTimerElapsed();
    void GiveReward();
    bool IsTargetInRange(const FVector& Position, const AActor* Target) const;

    FTimerHandle DeathTimer;
};
