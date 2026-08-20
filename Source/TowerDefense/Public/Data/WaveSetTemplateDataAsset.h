// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveSetTemplateDataAsset.generated.h"

class ATowerDefenseAICharacter;

UCLASS()
class TOWERDEFENSE_API UWaveSetTemplateDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TMap<TSubclassOf<ATowerDefenseAICharacter>, float> EnemyWeights;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    int32 EnemiesNum = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    float WaveDuration = 100.0f;
};
