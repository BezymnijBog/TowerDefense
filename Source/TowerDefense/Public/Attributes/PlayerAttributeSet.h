// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtils.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSE_API UPlayerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Money);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayAttributeData Money;
};
