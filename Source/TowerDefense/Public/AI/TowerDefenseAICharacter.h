// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "TowerDefenseAICharacter.generated.h"

class ATowerDefenseAIController;
class UAICharacterAttributeSet;
class UBehaviorTree;
class UProgressBarWidget;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, class ATowerDefenseAICharacter*);

UCLASS()
class TOWERDEFENSE_API ATowerDefenseAICharacter : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATowerDefenseAICharacter();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    FOnDeathDelegate OnDeathDelegate;

protected:
    virtual void OnDeath();

    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AICharacter|Abilities")
    TObjectPtr<const UAICharacterAttributeSet> AttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AICharacter|Abilities")
    TSubclassOf<UGameplayAbility> DealDamageAbilityClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AICharacter")
    FGenericTeamId TeamId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AICharacter")
    float DeathDestroyInterval = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AICharacter")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AICharacter|Abilities")
    FGameplayAbilitySpecHandle DealDamageAbility;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AICharacter")
    TWeakObjectPtr<ATowerDefenseAIController> AICharacterController;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AICharacter")
    bool bIsDead = false;

private:
    void OnDeathTimerElapsed();

    FTimerHandle DeathTimer;
};
