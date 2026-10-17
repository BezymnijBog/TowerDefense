// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Subsystems/RunSubsystem.h"
#include "Actors/WavesManager.h"
#include "Data/DayChoiceDataAsset.h"
#include "Subsystems/GameStartSubsystem.h"
#include "Settings/TowerDefenseSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogRunSystem, Log, All);

namespace
{
FText GetDescription(EDayEventType Type)
{
    switch (Type)
    {
        case EDayEventType::BossRaid:
            return NSLOCTEXT("Run", "BossDescription", "Defeat the boss to finish this act.");
        case EDayEventType::EliteRaid:
            return NSLOCTEXT("Run", "EliteDescription", "A dangerous raid with a greater reward.");
        case EDayEventType::NormalRaid:
            return NSLOCTEXT("Run", "NormalDescription", "A regular raid with a modest reward.");
        default:
            return NSLOCTEXT("Run", "PlaceholderDescription", "Placeholder for a description");
    }
}
} // namespace

float FRunState::Difficulty() const
{
    const UTowerDefenseSettings& Settings = UTowerDefenseSettings::Get();
    return Settings.BaseEventDifficulty + (CurrentAct - 1) * Settings.DifficultyIncreasePerAct + (CurrentDay - 1) * Settings.DifficultyIncreasePerDay;
}

void URunSubsystem::StartNewRun()
{
    if (!IsValid(DayChoiceAsset))
    {
        return;
    }
    if (RunState.Phase != ERunPhase::None && RunState.Phase != ERunPhase::RunComplete && RunState.Phase != ERunPhase::RunFailed)
    {
        return;
    }

    if (WavesManager.IsValid() && WavesManager->IsWaveActive())
    {
        return;
    }

    RunState = FRunState();
    CurrentChoices.Reset();
    MadeChoices.Reset();
    SelectedChoice = FDayChoice();
    RandomSeed = FMath::Rand32();
    RandomStream.Initialize(RandomSeed);
    UE_LOG(LogRunSystem, Display, TEXT("New run started with seed %d"), RandomSeed);
    BeginDay();
}

void URunSubsystem::SetPhase(ERunPhase Phase)
{
    RunState.Phase = Phase;
    OnPhaseChanged.Broadcast(Phase);
}

void URunSubsystem::BeginDay()
{
    if (!IsValid(DayChoiceAsset))
    {
        UE_LOG(LogRunSystem, Error, TEXT("Cannot begin day before game assets have loaded."));
        return;
    }
    CurrentChoices.Reset();
    RunState.CurrentEvent = EDayEventType::None;
    SetPhase(ERunPhase::DaySelection);
    GenerateDayChoices();
    // The mandatory boss still waits for the player's start-wave command.
    if (RunState.CurrentDay == UTowerDefenseSettings::Get().DaysPerAct && CurrentChoices.Num() == 1)
    {
        SelectDayEvent(CurrentChoices[0].EventId);
    }
}

void URunSubsystem::GenerateDayChoices()
{
    if (RunState.Phase != ERunPhase::DaySelection)
    {
        return;
    }

    if (CurrentChoices.IsEmpty())
    {
        const UTowerDefenseSettings& Settings = UTowerDefenseSettings::Get();
        const TArray<EDayEventType> Types = RunState.CurrentDay == Settings.DaysPerAct ?
            TArray<EDayEventType>{ EDayEventType::BossRaid } :
            TArray<EDayEventType>{ EDayEventType::NormalRaid, EDayEventType::NormalRaid, EDayEventType::EliteRaid };
        for (const EDayEventType Type : Types)
        {
            FDayChoice Choice;
            Choice.EventType = Type;
            const uint32 A = RandomStream.GetUnsignedInt();
            const uint32 B = RandomStream.GetUnsignedInt();
            const uint32 C = RandomStream.GetUnsignedInt();
            const uint32 D = RandomStream.GetUnsignedInt();
            Choice.EventId = FGuid(A, B, C, D);
            Choice.Difficulty = RunState.Difficulty();
            const int32 BaseMoneyReward = Settings.GetReward(Type);
            Choice.MoneyReward = BaseMoneyReward * RunState.CurrentAct;
            if (const FDayChoiceData* Data = DayChoiceAsset->EventData.Find(Type); Data)
            {
                Choice.Icon = Data->Icon;
                Choice.Description = Data->Description;
            }
            else
            {
                Choice.Description = GetDescription(Type);
            }
            CurrentChoices.Add(Choice);
        }
    }
    DayChoiceGenerated.Broadcast(CurrentChoices);
}

bool URunSubsystem::SelectDayEvent(FGuid EventId)
{
    if (RunState.Phase != ERunPhase::DaySelection || !WavesManager.IsValid())
    {
        return false;
    }
    const FDayChoice* Choice = CurrentChoices.FindByPredicate([EventId](const FDayChoice& Item) { return Item.EventId == EventId; });
    if (!Choice)
    {
        return false;
    }
    EWaveType WaveType;
    switch (Choice->EventType)
    {
        case EDayEventType::NormalRaid:
            WaveType = EWaveType::Common;
            break;
        case EDayEventType::EliteRaid:
            WaveType = EWaveType::Elite;
            break;
        case EDayEventType::BossRaid:
            WaveType = EWaveType::Boss;
            break;
        default:
            return false;
    }
    if (!WavesManager->PrepareWave(WaveType, Choice->Difficulty))
    {
        return false;
    }
    SelectedChoice = *Choice;
    MadeChoices.Add(SelectedChoice);
    RunState.CurrentEvent = Choice->EventType;
    SetPhase(ERunPhase::CombatPreparation);
    return true;
}

bool URunSubsystem::NotifyCombatStarted()
{
    if (RunState.Phase != ERunPhase::CombatPreparation)
    {
        return false;
    }
    SetPhase(ERunPhase::DayEvent);
    return true;
}

void URunSubsystem::NotifyCombatWon()
{
    if (RunState.Phase != ERunPhase::DayEvent)
    {
        return;
    }
    // Lock the result before applying rewards to reject duplicate completion signals.
    RunState.Phase = ERunPhase::PostEvent;
    OnCombatWon.Broadcast(SelectedChoice.MoneyReward);
    OnPhaseChanged.Broadcast(RunState.Phase);
}

bool URunSubsystem::AdvanceDay()
{
    if (RunState.Phase != ERunPhase::PostEvent)
    {
        return false;
    }
    if (const UTowerDefenseSettings& Settings = UTowerDefenseSettings::Get(); RunState.CurrentDay == Settings.DaysPerAct)
    {
        if (RunState.CurrentAct == Settings.ActsPerRun)
        {
            SetPhase(ERunPhase::RunComplete);
            return true;
        }
        ++RunState.CurrentAct;
        RunState.CurrentDay = 1;
    }
    else
    {
        ++RunState.CurrentDay;
    }
    BeginDay();
    return true;
}

void URunSubsystem::FailRun()
{
    if (RunState.Phase != ERunPhase::None && RunState.Phase != ERunPhase::RunComplete && RunState.Phase != ERunPhase::RunFailed)
    {
        SetPhase(ERunPhase::RunFailed);
    }
}

void URunSubsystem::RegisterWavesManager(AWavesManager* Manager)
{
    WavesManager = Manager;
    if (IsValid(GameStart) && GameStart->AreAssetsReady())
    {
        StartNewRun();
    }
    else if (IsValid(GameStart))
    {
        // Also support starting PIE directly on the gameplay map.
        GameStart->LoadGameAssets();
    }
}

void URunSubsystem::UnregisterWavesManager(const AWavesManager* Manager)
{
    if (WavesManager.Get() == Manager)
    {
        WavesManager.Reset();
        FailRun();
    }
}

void URunSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GameStart = Collection.InitializeDependency<UGameStartSubsystem>();
    if (IsValid(GameStart))
    {
        GameStart->OnCollectGameAssets.AddUObject(this, &ThisClass::CollectGameAssets);
        GameStart->OnGameAssetsLoaded.AddUObject(this, &ThisClass::OnGameAssetsLoaded);
    }
}

void URunSubsystem::Deinitialize()
{
    if (IsValid(GameStart))
    {
        GameStart->OnCollectGameAssets.RemoveAll(this);
        GameStart->OnGameAssetsLoaded.RemoveAll(this);
    }
    WavesManager.Reset();
    DayChoiceGenerated.Clear();
    OnPhaseChanged.Clear();
    OnCombatWon.Clear();
    Super::Deinitialize();
}

void URunSubsystem::CollectGameAssets(TArray<FSoftObjectPath>& Assets)
{
    Assets.AddUnique(UTowerDefenseSettings::Get().DayChoiceAsset.ToSoftObjectPath());
}

void URunSubsystem::OnGameAssetsLoaded(bool bSuccess)
{
    DayChoiceAsset = bSuccess ? UTowerDefenseSettings::Get().DayChoiceAsset.Get() : nullptr;
    if (bSuccess && WavesManager.IsValid())
    {
        StartNewRun();
    }
}
