// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/WorldRoadSubsystem.h"

#include "Actors/WavesManager.h"
#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Utils/BaseUtils.h"

#include <functional>

void UWorldRoadSubsystem::RegisterActor(AWayActor* WayActor)
{
    WavesManager->AddSpawner(WayActor);
    RegisteredActors.Emplace(WayActor);
    FinishedWaves.Emplace(WayActor, false);
    WayActor->WaveIsOver.BindUObject(this, &ThisClass::OnSpawnerWaveFinished, WayActor);
}

void UWorldRoadSubsystem::UnRegisterActor(AWayActor* WayActor)
{
    WayActor->WaveIsOver.Unbind();
    RegisteredActors.Remove(WayActor);
    FinishedWaves.Remove(WayActor);
}

bool UWorldRoadSubsystem::CanSpawnUnit(const FVector& SpawnPoint) const
{
    return Algo::AnyOf(RegisteredActors, std::bind(&AWayActor::CanSpawnUnit, std::placeholders::_1, SpawnPoint));
}

bool UWorldRoadSubsystem::CanStartWave() const
{
    return !bIsWaveInProgress && WavesManager.IsValid() && WavesManager->CanStartWave();
}

bool UWorldRoadSubsystem::StartWave()
{
    if (!CanStartWave())
    {
        return false;
    }

    for (TPair<AWayActor*, bool>& Pair : FinishedWaves)
    {
        Pair.Value = false;
    }

    bIsWaveInProgress = true;
    if (!WavesManager->StartWave())
    {
        bIsWaveInProgress = false;
        return false;
    }
    return true;
}

bool UWorldRoadSubsystem::IsWaveInProgress() const
{
    return bIsWaveInProgress;
}

void UWorldRoadSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    WavesManager = Utils::GetActorOfClass<AWavesManager>(&InWorld);
    check(WavesManager.IsValid());
}

void UWorldRoadSubsystem::OnSpawnerWaveFinished(AWayActor* WayActor)
{
    if (!bIsWaveInProgress)
    {
        return;
    }

    check(FinishedWaves.Contains(WayActor));
    FinishedWaves[WayActor] = true;
    if (Algo::AllOf(FinishedWaves, [](const TPair<AWayActor*, bool>& Pair) { return Pair.Value; }))
    {
        bIsWaveInProgress = false;
        WaveCleared.Broadcast();
    }
}
