// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TowerDefenseAIController.generated.h"

class UAISense;

UCLASS()
class TOWERDEFENSE_API ATowerDefenseAIController : public AAIController
{
    GENERATED_BODY()

public:
    ATowerDefenseAIController();

    AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
};
