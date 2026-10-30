// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/WavesManager.h"

#include "Actors/WayActor.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Algo/ForEach.h"
#include "Data/WaveSetTemplateDataAsset.h"
#include "Subsystems/WorldRoadSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "TowerDefense.h"
#include "Utils/BaseUtils.h"

AWavesManager::AWavesManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AWavesManager::CanStartWave() const
{
    const URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this);
    if (!IsValid(Run) || !bWavePrepared || bWaveActive || Run->GetRunState().Phase != ERunPhase::CombatPreparation)
    {
        return false;
    }
    for (const TWeakObjectPtr<AWayActor>& Spawner : WaveSpawners)
    {
        if (Spawner.IsValid())
        {
            return true;
        }
    }
    return false;
}

bool AWavesManager::StartWave()
{
    URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this);
    if (!IsValid(Run) || !CanStartWave() || !Run->NotifyCombatStarted())
    {
        return false;
    }
    bWavePrepared = false;
    bWaveActive = true;
    for (const TWeakObjectPtr<AWayActor>& Spawner : WaveSpawners)
    {
        if (Spawner.IsValid())
        {
            Spawner->SetWaveDefinition(NextWaveDefinition);
        }
    }
    return true;
}

bool AWavesManager::PrepareWave(EWaveType WaveType, float Difficulty)
{
    if (bWaveActive)
    {
        return false;
    }

    UWaveSetTemplateDataAsset* const* Entry = WaveDataAssets.Find(WaveType);
    const UWaveSetTemplateDataAsset* WaveTemplate = Entry ? *Entry : nullptr;
    if (!IsValid(WaveTemplate) || WaveTemplate->EnemiesNum <= 0 || !FMath::IsFinite(Difficulty) || Difficulty <= 0.f
        || !FMath::IsFinite(WaveTemplate->WaveDuration) || WaveTemplate->WaveDuration <= 0.f)
    {
        UE_LOG(LogTowerDefense, Error, TEXT("Cannot prepare wave type %d: missing or invalid template"), static_cast<int32>(WaveType));
        return false;
    }

    FWaveDefinition NewDefinition;
    NewDefinition.SpawnGroup.Reserve(FMath::CeilToInt(WaveTemplate->EnemiesNum * Difficulty));
    NewDefinition.WaveDuration = WaveTemplate->WaveDuration;
    for (const TPair<TSubclassOf<ATowerDefenseAICharacter>, float>& Weights : WaveTemplate->EnemyWeights)
    {
        if (!Weights.Key || !FMath::IsFinite(Weights.Value) || Weights.Value <= 0.f)
        {
            continue;
        }
        TArray<TSubclassOf<ATowerDefenseAICharacter>> EnemiesPack;
        EnemiesPack.AddDefaulted(FMath::RoundHalfToEven(Weights.Value * WaveTemplate->EnemiesNum * Difficulty));
        Algo::ForEach(EnemiesPack, [&Weights](TSubclassOf<ATowerDefenseAICharacter>& Class) { Class = Weights.Key; });
        NewDefinition.SpawnGroup.Append(MoveTemp(EnemiesPack));
    }
    if (NewDefinition.SpawnGroup.IsEmpty())
    {
        UE_LOG(LogTowerDefense, Error, TEXT("Cannot prepare an empty wave"));
        return false;
    }
    NewDefinition.WaveStartDelay *= FMath::FRand() + 0.5;
    NextWaveDefinition = MoveTemp(NewDefinition);
    NextWaveType = WaveType;
    bWavePrepared = true;
    return true;
}

void AWavesManager::AddSpawner(AWayActor* Spawner)
{
    WaveSpawners.Emplace(Spawner);
}

void AWavesManager::BeginPlay()
{
    Super::BeginPlay();
    if (UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); IsValid(Roads))
    {
        Roads->WaveCleared.AddUObject(this, &ThisClass::OnWaveCleared);
    }
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnPhaseChanged.AddUObject(this, &ThisClass::OnRunPhaseChanged);
        Run->RegisterWavesManager(this);
    }
}

void AWavesManager::OnWaveCleared()
{
    if (bWaveActive)
    {
        bWaveActive = false;
        // Allow a settlement death in the same frame to take precedence over victory.
        GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::ResolveVictory);
    }
}

void AWavesManager::ResolveVictory()
{
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->NotifyCombatWon();
    }
}

void AWavesManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearAllTimersForObject(this);
    if (UWorldRoadSubsystem* Roads = Utils::GetWorldSubsystem<UWorldRoadSubsystem>(this); IsValid(Roads))
    {
        Roads->WaveCleared.RemoveAll(this);
    }
    if (URunSubsystem* Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(this); IsValid(Run))
    {
        Run->OnPhaseChanged.RemoveAll(this);
        Run->UnregisterWavesManager(this);
    }
    Super::EndPlay(EndPlayReason);
}

void AWavesManager::OnRunPhaseChanged(ERunPhase Phase)
{
    if (Phase == ERunPhase::RunFailed)
    {
        bWavePrepared = false;
        bWaveActive = false;
        for (const TWeakObjectPtr<AWayActor>& Spawner : WaveSpawners)
        {
            if (Spawner.IsValid())
            {
                Spawner->CancelWave();
            }
        }
    }
}
