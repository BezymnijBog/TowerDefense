// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "AbilityDescription.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FDefaultAbilityDescription
{
    GENERATED_BODY()

    FGameplayAbilitySpec ToSpec() const;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayAbility> Ability;

    UPROPERTY(EditAnywhere)
    bool bActivateAtStartup = false;

    UPROPERTY(EditAnywhere)
    int32 Level = 1;

    UPROPERTY(EditAnywhere)
    int32 InputID = INDEX_NONE;
};
