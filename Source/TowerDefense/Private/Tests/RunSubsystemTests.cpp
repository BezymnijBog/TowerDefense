#include "Subsystems/RunSubsystem.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/GameInstance.h"
#include "Misc/AutomationTest.h"
#include "Settings/TowerDefenseSettings.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunSelectionGuardsTest, "TowerDefense.Run.SelectionGuards",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunSelectionGuardsTest::RunTest(const FString& Parameters)
{
    UGameInstance* Instance = NewObject<UGameInstance>();
    URunSubsystem* Run = NewObject<URunSubsystem>(Instance);
    Run->StartNewRun();
    const TArray<FDayChoice> Choices = Run->GetCurrentChoices();
    const int32 ExpectedChoiceCount = UTowerDefenseSettings::Get().DaysPerAct == 1 ? 1 : 2;
    TestEqual(TEXT("First day offers the configured encounter choices"), Choices.Num(), ExpectedChoiceCount);
    TestEqual(TEXT("Run starts at day one"), Run->GetRunState().CurrentDay, 1);
    TestEqual(TEXT("Run waits for selection"), Run->GetRunState().Phase, ERunPhase::DaySelection);

    Run->GenerateDayChoices();
    Run->StartNewRun();
    const TArray<FDayChoice> ReopenedChoices = Run->GetCurrentChoices();
    if (Choices.Num() == 2 && ReopenedChoices.Num() == 2)
    {
        TestEqual(TEXT("Reopening and repeated start do not reroll choices"), ReopenedChoices[0].EventId, Choices[0].EventId);
        TestNotEqual(TEXT("Each card has a distinct identity"), Choices[0].EventId, Choices[1].EventId);
        TestFalse(TEXT("Selection without a wave manager fails safely"), Run->SelectDayEvent(Choices[0].EventId));
    }
    TestFalse(TEXT("An unknown event cannot be selected"), Run->SelectDayEvent(FGuid::NewGuid()));
    TestFalse(TEXT("Cannot skip an unfinished day"), Run->AdvanceDay());
    TestFalse(TEXT("Cannot start combat before selection"), Run->NotifyCombatStarted());
    Run->NotifyCombatWon();
    TestEqual(TEXT("Stale victory is ignored"), Run->GetRunState().Phase, ERunPhase::DaySelection);

    Run->FailRun();
    Run->NotifyCombatWon();
    Run->GenerateDayChoices();
    TestFalse(TEXT("Failure cannot advance the day"), Run->AdvanceDay());
    TestEqual(TEXT("Failure is terminal"), Run->GetRunState().Phase, ERunPhase::RunFailed);
    Run->StartNewRun();
    TestEqual(TEXT("Explicit restart resets the act"), Run->GetRunState().CurrentAct, 1);
    TestEqual(TEXT("Explicit restart resets the phase"), Run->GetRunState().Phase, ERunPhase::DaySelection);
    return true;
}
#endif
