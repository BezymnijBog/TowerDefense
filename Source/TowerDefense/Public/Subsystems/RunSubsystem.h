// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RunSubsystem.generated.h"

class UDayChoiceDataAsset;

UENUM(BlueprintType)
enum class ERunPhase : uint8
{
    None = 0,
    DaySelection,
    DayEvent,
    PostEvent,
    RunComplete,
    RunFailed,
    CombatPreparation
};

UENUM(BlueprintType)
enum class EDayEventType : uint8
{
    None = 0,
    NormalRaid,
    EliteRaid,
    MysteryEvent,
    Merchant,
    BossRaid
};

USTRUCT()
struct FDayChoice
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EDayEventType EventType = EDayEventType::None;

    UPROPERTY(EditAnywhere)
    FGuid EventId = FGuid::NewGuid();

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere)
    FText Description;

    UPROPERTY(EditAnywhere)
    int32 MoneyReward = 0;

    UPROPERTY(EditAnywhere)
    float Difficulty = 1.f;
};

USTRUCT()
struct FRunState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int32 CurrentAct = 1;

    UPROPERTY(EditAnywhere)
    int32 CurrentDay = 1;

    UPROPERTY(EditAnywhere)
    ERunPhase Phase = ERunPhase::None;

    UPROPERTY(EditAnywhere)
    EDayEventType CurrentEvent = EDayEventType::None;

    float Difficulty() const;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDayChoiceGenerated, const TArray<FDayChoice>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FRunPhaseChanged, ERunPhase);
DECLARE_MULTICAST_DELEGATE_OneParam(FRunCombatWon, int32);

class AWavesManager;

UCLASS()
class TOWERDEFENSE_API URunSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    FDayChoiceGenerated DayChoiceGenerated;
    FRunPhaseChanged OnPhaseChanged;
    FRunCombatWon OnCombatWon;

    void StartNewRun();
    void GenerateDayChoices();
    bool SelectDayEvent(FGuid EventId);
    bool AdvanceDay();
    void FailRun();

    FRunState GetRunState() const { return RunState; }
    TArray<FDayChoice> GetCurrentChoices() const { return CurrentChoices; }
    UDayChoiceDataAsset* GetDayChoiceAsset() const { return DayChoiceAsset; }

    void RegisterWavesManager(AWavesManager* Manager);
    void UnregisterWavesManager(const AWavesManager* Manager);
    bool NotifyCombatStarted();
    void NotifyCombatWon();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    void SetPhase(ERunPhase Phase);
    void BeginDay();
    void OnDayChoiceLoaded();

    UPROPERTY()
    TWeakObjectPtr<AWavesManager> WavesManager;

    UPROPERTY()
    FDayChoice SelectedChoice;

    UPROPERTY()
    FRunState RunState;

    UPROPERTY()
    TArray<FDayChoice> CurrentChoices;

    UPROPERTY()
    TArray<FDayChoice> MadeChoices;

    UPROPERTY()
    int32 RandomSeed = 0;

    UPROPERTY()
    TObjectPtr<UDayChoiceDataAsset> DayChoiceAsset;

    FRandomStream RandomStream;
};
