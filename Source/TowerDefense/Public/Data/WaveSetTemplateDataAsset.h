// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveSetTemplateDataAsset.generated.h"

class ATowerDefenseAICharacter;

USTRUCT()
struct FWaveTemplate
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TMap<TSubclassOf<ATowerDefenseAICharacter>, float> EnemyWeights;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    int32 EnemiesNum = 0;
};

UCLASS()
class TOWERDEFENSE_API UWaveSetTemplateDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TArray<FWaveTemplate> WaveSet;
};
