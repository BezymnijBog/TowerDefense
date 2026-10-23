// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS
#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Settings/TowerDefenseSettings.h"
#include "Subsystems/GameStartSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

TEST_CLASS_WITH_FLAGS(FRunSelectionGuardsTest, "TowerDefense.Run", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
    FActorTestSpawner Spawner;
    URunSubsystem* Run = nullptr;
    UGameStartSubsystem* Loading = nullptr;

    BEFORE_EACH()
    {
        Spawner.InitializeGameSubsystems();
        const AActor& Context = Spawner.SpawnActor<AActor>();
        Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(&Context);
        Loading = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(&Context);
        ASSERT_THAT(IsNotNull(Run));
        ASSERT_THAT(IsNotNull(Loading));
        Loading->LoadGameAssets();
        TestCommandBuilder.Until(TEXT("Waiting for run assets"), [this]() { return Loading->AreAssetsReady(); }, FTimespan::FromSeconds(10));
    }

    TEST_METHOD(SelectionGuards)
    {
        TestCommandBuilder.Do(
            [this]
            {
                ASSERT_THAT(IsNotNull(Run->GetDayChoiceAsset()));
                Run->StartNewRun();
                const TArray<FDayChoice> Choices = Run->GetCurrentChoices();
                const int32 ExpectedChoiceCount = UTowerDefenseSettings::Get().DaysPerAct == 1 ? 1 : 3;
                ASSERT_THAT(AreEqual(ExpectedChoiceCount, Choices.Num()));
                ASSERT_THAT(AreEqual(1, Run->GetRunState().CurrentDay));
                ASSERT_THAT(AreEqual(ERunPhase::DaySelection, Run->GetRunState().Phase));
                Run->GenerateDayChoices();
                Run->StartNewRun();
                const TArray<FDayChoice> ReopenedChoices = Run->GetCurrentChoices();
                ASSERT_THAT(AreEqual(Choices.Num(), ReopenedChoices.Num()));
                for (int32 Index = 0; Index < Choices.Num(); ++Index)
                {
                    ASSERT_THAT(AreEqual(Choices[Index].EventId, ReopenedChoices[Index].EventId));
                    for (int32 OtherIndex = Index + 1; OtherIndex < Choices.Num(); ++OtherIndex)
                    {
                        ASSERT_THAT(IsFalse(Choices[Index].EventId == Choices[OtherIndex].EventId));
                    }
                }
                ASSERT_THAT(IsFalse(Run->SelectDayEvent(Choices[0].EventId)));
                ASSERT_THAT(IsFalse(Run->SelectDayEvent(FGuid::NewGuid())));
                ASSERT_THAT(IsFalse(Run->AdvanceDay()));
                ASSERT_THAT(IsFalse(Run->NotifyCombatStarted()));
                Run->NotifyCombatWon();
                ASSERT_THAT(AreEqual(ERunPhase::DaySelection, Run->GetRunState().Phase));
                Run->FailRun();
                Run->NotifyCombatWon();
                Run->GenerateDayChoices();
                ASSERT_THAT(IsFalse(Run->AdvanceDay()));
                ASSERT_THAT(AreEqual(ERunPhase::RunFailed, Run->GetRunState().Phase));
                Run->StartNewRun();
                ASSERT_THAT(AreEqual(1, Run->GetRunState().CurrentAct));
                ASSERT_THAT(AreEqual(ERunPhase::DaySelection, Run->GetRunState().Phase));
            });
    }
};
#endif
