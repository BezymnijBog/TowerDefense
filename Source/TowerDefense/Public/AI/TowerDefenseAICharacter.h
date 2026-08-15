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
    ATowerDefenseAICharacter();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UWayComponent* GetWayComponent() const;

    virtual float GetAttackRange() const override;

    virtual bool IsDead() const override;
    virtual void OnDeath() override;
    virtual FDeathDelegate& GetDeathDelegate() override;
    FDeathDelegate DeathDelegate;

protected:
    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;

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

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter")
    TWeakObjectPtr<ATowerDefenseAIController> AICharacterController;

    UPROPERTY(VisibleInstanceOnly, Category = "AICharacter")
    bool bIsDead = false;

private:
    void OnDeathTimerElapsed();
    void GiveReward();

    FTimerHandle DeathTimer;
};
