// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/BaseUtils.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

namespace
{
FGameplayEventData MakePayload(AActor* Instigator, AActor* Target, float Magnitude)
{
    FGameplayEventData Payload;
    Payload.Instigator = Instigator;
    Payload.Target = Target;
    Payload.EventMagnitude = Magnitude;
    return Payload;
}
} // namespace

namespace Teams
{
ETeamAttitude::Type AttitudeSolveFunction(FGenericTeamId Left, FGenericTeamId Right)
{
    if (Left == Right)
    {
        return ETeamAttitude::Friendly;
    }

    if (Left == TeamNone || Right == TeamNone)
    {
        return ETeamAttitude::Neutral;
    }

    return ETeamAttitude::Hostile;
}
}

namespace AbilitySystem
{
void SendGameplayEventToInstigator(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, MoveTemp(Tag), MakePayload(Instigator, Target, Magnitude));
}

void SendGameplayEventToTarget(AActor* Instigator, AActor* Target, FGameplayTag Tag, float Magnitude)
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, MoveTemp(Tag), MakePayload(Instigator, Target, Magnitude));
}
} // namespace AbilitySystem

