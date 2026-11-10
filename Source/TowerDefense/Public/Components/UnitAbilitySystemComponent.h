// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Utils/AbilityDescription.h"
#include "UnitAbilitySystemComponent.generated.h"

class UAICharacterAttributeSet;

DECLARE_DELEGATE(FUnitDead)
DECLARE_DELEGATE_OneParam(FDeathThresholdExceeded, int32)
DECLARE_DELEGATE_OneParam(FHitPointPercentChanged, float)

UCLASS()
class TOWERDEFENSE_API UUnitAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    FUnitDead UnitDead;
    FDeathThresholdExceeded DeathThresholdExceeded;
    FHitPointPercentChanged HitPointPercentChanged;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TArray<FDefaultAbilityDescription> DefaultAbilities;

    // Applied to the ability system before startup abilities are activated.
    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    FGameplayTagContainer DefaultTags;

    UPROPERTY(VisibleInstanceOnly, Category = "AbilitySystem")
    TObjectPtr<const UAICharacterAttributeSet> AttributeSet;

private:
    void OnHitPointsChanged(const FOnAttributeChangeData& Data);
    void OnStrengthChanged(const FOnAttributeChangeData& Data);

    bool bIsDead = false;
    int32 CurrentDeathThreshold = 0;
};
