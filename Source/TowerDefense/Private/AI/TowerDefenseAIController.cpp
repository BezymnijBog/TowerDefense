// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/TowerDefenseAIController.h"

#include "Perception/AIPerceptionComponent.h"

ATowerDefenseAIController::ATowerDefenseAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
}

AActor* ATowerDefenseAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
    TArray<AActor*> SensedActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(MoveTemp(SenseClass), SensedActors);
    if (SensedActors.IsEmpty())
    {
        return nullptr;
    }

    const APawn* const ControlledPawn = GetPawn();
    if (!IsValid(ControlledPawn))
    {
        return SensedActors[0];
    }

    const FVector PawnLocation = ControlledPawn->GetActorLocation();
    return *Algo::MinElementBy(SensedActors, [&PawnLocation](const AActor* Actor) { return (Actor->GetActorLocation() - PawnLocation).Length(); });
}
