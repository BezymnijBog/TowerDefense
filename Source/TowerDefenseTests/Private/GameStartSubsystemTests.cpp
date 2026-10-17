#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS
#include "CQTest.h"
#include "Components/ActorTestSpawner.h"
#include "Subsystems/GameStartSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "Utils/BaseUtils.h"

TEST_CLASS_WITH_FLAGS(FGameAssetPreparationTest, "TowerDefense.GameStart", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
    FActorTestSpawner Spawner;
    UGameStartSubsystem* Loading = nullptr;
    URunSubsystem* Run = nullptr;
    FDelegateHandle CollectionHandle;
    FDelegateHandle CompletionHandle;
    bool bCollected = false;
    bool bCompleted = false;
    bool bSuccess = false;

    BEFORE_EACH()
    {
        Spawner.InitializeGameSubsystems();
        const AActor& Context = Spawner.SpawnActor<AActor>();
        Loading = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(&Context);
        Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(&Context);
        ASSERT_THAT(IsNotNull(Loading));
        ASSERT_THAT(IsNotNull(Run));
    }

    AFTER_EACH()
    {
        if (IsValid(Loading))
        {
            Loading->OnCollectGameAssets.Remove(CollectionHandle);
            Loading->OnGameAssetsLoaded.Remove(CompletionHandle);
        }
    }

    TEST_METHOD(AssetPreparation)
    {
        ASSERT_THAT(IsFalse(Loading->AreAssetsReady()));
        ASSERT_THAT(IsNull(Run->GetDayChoiceAsset()));
        Run->StartNewRun();
        ASSERT_THAT(AreEqual(ERunPhase::None, Run->GetRunState().Phase));
        ASSERT_THAT(IsTrue(Run->GetCurrentChoices().IsEmpty()));

        CompletionHandle = Loading->OnGameAssetsLoaded.AddLambda(
            [this](bool bLoaded)
            {
                bCompleted = true;
                bSuccess = bLoaded;
            });
        Loading->LoadGameAssets();
        TestCommandBuilder.Until(TEXT("Waiting for initial game assets"), [this] { return bCompleted; })
            .Do(
                [this]()
                {
                    ASSERT_THAT(IsTrue(bSuccess));
                    ASSERT_THAT(IsNotNull(Run->GetDayChoiceAsset()));
                    CollectionHandle = Loading->OnCollectGameAssets.AddLambda(
                        [this](TArray<FSoftObjectPath>& Assets)
                        {
                            bCollected = true;
                            Assets.Add(FSoftObjectPath());
                        });
                    bCompleted = false;
                    TestRunner->AddExpectedError(TEXT("Failed to load game asset:"), EAutomationExpectedErrorFlags::Contains, 1);
                    Loading->LoadGameAssets();
                    ASSERT_THAT(IsTrue(bCollected));
                    ASSERT_THAT(IsTrue(bCompleted));
                    ASSERT_THAT(IsFalse(bSuccess));
                    ASSERT_THAT(IsFalse(Loading->AreAssetsReady()));
                    ASSERT_THAT(IsNull(Run->GetDayChoiceAsset()));

                    // Keep the real subsystem subscriptions when retrying.
                    Loading->OnCollectGameAssets.Remove(CollectionHandle);
                    bCompleted = false;
                    Loading->LoadGameAssets();
                })
            .Until(TEXT("Waiting for game asset retry"), [this]() { return bCompleted; })
            .Then(
                [this]()
                {
                    ASSERT_THAT(IsTrue(bSuccess));
                    ASSERT_THAT(IsTrue(Loading->AreAssetsReady()));
                    ASSERT_THAT(IsNotNull(Run->GetDayChoiceAsset()));
                });
    }
};
#endif
