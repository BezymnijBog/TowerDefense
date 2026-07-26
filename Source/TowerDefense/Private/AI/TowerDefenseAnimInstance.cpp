// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/TowerDefenseAnimInstance.h"

#include "AI/TowerDefenseAICharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UTowerDefenseAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    bDeathAnimation = UKismetMathLibrary::RandomBool();

    OwningCharacter = Cast<ATowerDefenseAICharacter>(GetOwningActor());
    if (!OwningCharacter.IsValid())
    {
        UE_LOG(LogAnimation, Error, TEXT("Invalid owning character for %s"), *GetName());
        return;
    }

    OwnerMovement = OwningCharacter->GetMovementComponent();
    if (!OwnerMovement.IsValid())
    {
        UE_LOG(LogAnimation, Error, TEXT("Invalid character movement component for %s"), *GetName());
        return;
    }
}

void UTowerDefenseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!OwningCharacter.IsValid() || !OwnerMovement.IsValid())
    {
        UE_LOG(LogAnimation, Error, TEXT("Stale character pointer for %s"), *GetName());
        return;
    }

    GroundSpeed = OwnerMovement->GetVelocityForNavMovement().Length();
    bIsDead = OwningCharacter->IsDead();
}
