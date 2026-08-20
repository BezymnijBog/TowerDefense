// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "WaveSpawnUtils.generated.h"

class ATowerDefenseAICharacter;

UENUM()
enum class EWaveType
{
    Weak,
    Common,
    Elite,
    Boss
};

USTRUCT()
struct FWaveDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<ATowerDefenseAICharacter>> SpawnGroup;

    UPROPERTY(EditAnywhere)
    float WaveStartDelay = 3.0f;

    UPROPERTY(EditAnywhere)
    float WaveDuration = 100.0f;
};
