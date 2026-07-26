// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Utils/BaseUtils.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
class UProgressBarWidget;
class AEnemyController;
class UEnemyAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, class AEnemyCharacter*);

UCLASS()
class TOWERDEFENSE_API AEnemyCharacter : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;

    FOnDeathDelegate OnDeathDelegate;

protected:
    void OnDeath();

    void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TWeakObjectPtr<UProgressBarWidget> HealthWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Abilities")
    TObjectPtr<const UEnemyAttributeSet> AttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Abilities")
    TSubclassOf<UGameplayAbility> DealDamageAbilityClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    FGenericTeamId TeamId = Teams::TeamEnemy;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float DeathDestroyInterval = 5.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Abilities")
    FGameplayAbilitySpecHandle DealDamageAbility;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy")
    TWeakObjectPtr<AEnemyController> EnemyController;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy")
    bool bIsDead = false;

private:
    void OnDeathTimerElapsed();

    FTimerHandle DeathTimer;
};
