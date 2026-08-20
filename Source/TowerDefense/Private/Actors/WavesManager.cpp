// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/WavesManager.h"

#include "Actors/WayActor.h"
#include "Algo/ForEach.h"
#include "Data/WaveSetTemplateDataAsset.h"
#include "Subsystems/WorldRoadSubsystem.h"

AWavesManager::AWavesManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWavesManager::StartWave()
{
    for (const TWeakObjectPtr<AWayActor>& Spawner : WaveSpawners)
    {
        if (Spawner.IsValid())
        {
            Spawner->SetWaveDefinition(NextWaveDefinition);
        }
    }
}

void AWavesManager::PrepareNextWave()
{
    const UWaveSetTemplateDataAsset* const WaveTemplate = WaveDataAssets.FindChecked(NextWaveType);
    check(IsValid(WaveTemplate));

    FWaveDefinition NewDefinition;
    NewDefinition.SpawnGroup.Reserve(WaveTemplate->EnemiesNum);
    NewDefinition.WaveDuration = WaveTemplate->WaveDuration;
    for (const TPair<TSubclassOf<ATowerDefenseAICharacter>, float>& Weights : WaveTemplate->EnemyWeights)
    {
        TArray<TSubclassOf<ATowerDefenseAICharacter>> EnemiesPack;
        EnemiesPack.AddDefaulted(FMath::RoundHalfToEven(Weights.Value * WaveTemplate->EnemiesNum));
        Algo::ForEach(EnemiesPack, [&Weights](TSubclassOf<ATowerDefenseAICharacter>& Class) { Class = Weights.Key; });
        NewDefinition.SpawnGroup.Append(MoveTemp(EnemiesPack));
    }
    NewDefinition.WaveStartDelay *= FMath::FRand() + 0.5;
    NextWaveDefinition = MoveTemp(NewDefinition);
}

void AWavesManager::AddSpawner(AWayActor* Spawner)
{
    WaveSpawners.Emplace(Spawner);
}

void AWavesManager::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetSubsystem<UWorldRoadSubsystem>()->WaveCleared.AddUObject(this, &ThisClass::PrepareNextWave);
    PrepareNextWave();
}
