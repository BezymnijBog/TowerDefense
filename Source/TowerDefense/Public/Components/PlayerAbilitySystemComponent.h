// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PlayerAbilitySystemComponent.generated.h"

class UPlayerAttributeSet;

struct FDefaultAbilityDescription;

UCLASS()
class TOWERDEFENSE_API UPlayerAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Abilities")
    TObjectPtr<const UPlayerAttributeSet> PlayerAttributes;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<FDefaultAbilityDescription> DefaultAbilities;

    UPROPERTY(VisibleInstanceOnly, Category = "Abilities")
    TArray<FGameplayAbilitySpecHandle> GivenAbilities;

private:
    void OnMoneyChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
};
