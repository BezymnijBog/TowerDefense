#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Abilities/GameplayAbility_ConstructBuilding.h"
#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Actors/BaseBuilding.h"
#include "Actors/MainBuilding.h"
#include "Actors/RegularBuilding.h"
#include "Actors/WavesManager.h"
#include "Actors/WayActor.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Attributes/BuildingAttributeSet.h"
#include "Attributes/PlayerAttributeSet.h"
#include "Components/ActorTestSpawner.h"
#include "Components/BuildingConstructionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CQTest.h"
#include "Engine/StaticMesh.h"
#include "Player/TowerDefensePlayerController.h"
#include "Settings/TowerDefenseWorldSettings.h"
#include "Subsystems/GameStartSubsystem.h"
#include "Subsystems/RunSubsystem.h"
#include "Subsystems/WorldGridSubsystem.h"
#include "UObject/UnrealType.h"
#include "Utils/BaseUtils.h"
#include "Widgets/BuildingUpgradeWidget.h"

TEST_CLASS_WITH_FLAGS(FBuildingTest, "TowerDefense.Buildings", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
    FActorTestSpawner Spawner;
    URunSubsystem* Run = nullptr;
    UGameStartSubsystem* Loading = nullptr;
    UWorldGridSubsystem* Grid = nullptr;

    BEFORE_EACH()
    {
        Spawner.InitializeGameSubsystems();
        const AActor& Context = Spawner.SpawnActor<AActor>();
        Run = Utils::GetGameInstanceSubsystem<URunSubsystem>(&Context);
        Loading = Utils::GetGameInstanceSubsystem<UGameStartSubsystem>(&Context);
        Grid = Utils::GetWorldSubsystem<UWorldGridSubsystem>(&Context);
        ASSERT_THAT(IsNotNull(Run));
        ASSERT_THAT(IsNotNull(Loading));
        ASSERT_THAT(IsNotNull(Grid));
        Grid->OnWorldBeginPlay(Spawner.GetWorld());
        Loading->LoadGameAssets();
        TestCommandBuilder.Until(TEXT("Waiting for run assets"), [this] { return Loading->AreAssetsReady(); }, FTimespan::FromSeconds(10));
    }

    template <typename TBuilding>
    TBuilding& SpawnBuilding(UStaticMesh* RuinsMesh = nullptr)
    {
        const ATowerDefenseWorldSettings* Settings = CastChecked<ATowerDefenseWorldSettings>(Spawner.GetWorld().GetWorldSettings());
        TBuilding& Building = Spawner.SpawnActorAt<TBuilding>(Settings->GridToWorldLocation({ 10, 10 }), FRotator::ZeroRotator);
        UAbilitySystemComponent* AbilitySystem = Building.GetAbilitySystemComponent();
        AbilitySystem->AddAttributeSetSubobject(NewObject<UBuildingAttributeSet>(&Building));
        AbilitySystem->SetNumericAttributeBase(UBuildingAttributeSet::GetMaxHealthAttribute(), 100.f);
        AbilitySystem->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), 100.f);
        GetTiers(Building)[0].DestroyedMesh = RuinsMesh;
        Building.DispatchBeginPlay();
        return Building;
    }

    ATowerDefensePlayerController& SpawnPlayer(float Money = 500.f)
    {
        ATowerDefensePlayerController& Player = Spawner.SpawnActor<ATowerDefensePlayerController>();
        Player.SetGenericTeamId(Teams::TeamPlayer);
        UAbilitySystemComponent* Wallet = Player.GetAbilitySystemComponent();
        Wallet->AddAttributeSetSubobject(NewObject<UPlayerAttributeSet>(&Player));
        Wallet->InitAbilityActorInfo(&Player, &Player);
        Wallet->SetNumericAttributeBase(UPlayerAttributeSet::GetMoneyAttribute(), Money);
        Wallet->GiveAbility(FGameplayAbilitySpec(UGameplayAbility_ConstructBuilding::StaticClass(), 1));
        return Player;
    }

    bool RequestConstruction(ABaseBuilding & Building, ATowerDefensePlayerController & Player)
    {
        const bool bWasConstructing = Building.GetConstructionComponent()->IsConstructing();
        FGameplayEventData Payload;
        Payload.Instigator = &Player;
        Payload.Target = &Building;
        Player.GetAbilitySystemComponent()->HandleGameplayEvent(Action_Building_Construct, &Payload);
        return !bWasConstructing && Building.GetConstructionComponent()->IsConstructing();
    }

    bool EndDay()
    {
        FStructProperty* Property = FindFProperty<FStructProperty>(URunSubsystem::StaticClass(), TEXT("RunState"));
        Property->ContainerPtrToValuePtr<FRunState>(Run)->Phase = ERunPhase::PostEvent;
        return Run->AdvanceDay();
    }

    TArray<FBuildingTier>& GetTiers(ABaseBuilding & Building)
    {
        FArrayProperty* Property = FindFProperty<FArrayProperty>(UBuildingConstructionComponent::StaticClass(), TEXT("Tiers"));
        return *Property->ContainerPtrToValuePtr<TArray<FBuildingTier>>(Building.GetConstructionComponent());
    }

    TEST_METHOD(RegularBuildingKeepsCellsUntilRemoved)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                UStaticMesh* RuinsMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
                ARegularBuilding& Building = SpawnBuilding<ARegularBuilding>(RuinsMesh);
                UStaticMeshComponent* Mesh = CastChecked<UStaticMeshComponent>(Building.GetRootComponent());
                ASSERT_THAT(IsTrue(Mesh->IsVisible()));
                ASSERT_THAT(AreEqual(RuinsMesh, Mesh->GetStaticMesh()));
                ASSERT_THAT(IsTrue(Building.IsDead()));
                ATowerDefensePlayerController& Player = SpawnPlayer();
                ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                ASSERT_THAT(IsFalse(Building.IsDead()));
                ASSERT_THAT(IsFalse(Building.GetOccupiedCells().IsEmpty()));
                int32 DeathCount = 0;
                const FDelegateHandle Handle = Building.GetDeathDelegate().AddLambda([&DeathCount] { ++DeathCount; });
                Building.GetAbilitySystemComponent()->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), 0.f);
                Building.OnDeath();
                Building.GetDeathDelegate().Remove(Handle);
                ASSERT_THAT(AreEqual(1, DeathCount));
                ASSERT_THAT(IsTrue(Building.IsDead()));
                ASSERT_THAT(IsTrue(Mesh->IsVisible()));
                ASSERT_THAT(AreEqual(RuinsMesh, Mesh->GetStaticMesh()));
                ASSERT_THAT(IsTrue(Building.GetActorEnableCollision()));
                ASSERT_THAT(AreEqual(ERunPhase::DaySelection, Run->GetRunState().Phase));
                for (const FIntVector2& Cell : Building.GetOccupiedCells())
                {
                    ASSERT_THAT(IsFalse(Grid->GetCellInfo(Cell)->IsAvailable()));
                    ASSERT_THAT(IsTrue(Grid->GetCellInfo(Cell)->Occupant.Get() == &Building));
                    ASSERT_THAT(IsFalse(Grid->TryReserveCell(Grid->GetCellInfo(Cell), &Player)));
                }
                Spawner.GetWorld().GetTimerManager().Tick(5.f);
                ASSERT_THAT(IsTrue(IsValid(&Building)));
                const TArray<FIntVector2> Cells = Building.GetOccupiedCells();
                Building.Destroy();
                for (const FIntVector2& Cell : Cells)
                {
                    ASSERT_THAT(IsTrue(Grid->GetCellInfo(Cell)->IsAvailable()));
                }
            });
    }

    TEST_METHOD(DestroyedConstructionNeverCompletesOrRefundsAndCanBeRebuilt)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                ARegularBuilding& Building = SpawnBuilding<ARegularBuilding>();
                ATowerDefensePlayerController& Player = SpawnPlayer();
                UBuildingConstructionComponent* Construction = Building.GetConstructionComponent();
                ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                Building.GetAbilitySystemComponent()->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), 0.f);
                ASSERT_THAT(IsTrue(EndDay()));
                ASSERT_THAT(IsTrue(Building.IsDead()));
                ASSERT_THAT(AreEqual(0, Construction->GetNextTierIndex()));
                ASSERT_THAT(AreEqual(0, Construction->GetRemainingDays()));
                ASSERT_THAT(IsNear(400.f, Player.GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMoneyAttribute()), 0.001f));
                ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                ASSERT_THAT(IsNear(100.f, Building.GetAbilitySystemComponent()->GetNumericAttribute(UBuildingAttributeSet::GetHealthAttribute()), 0.001f));
                ASSERT_THAT(IsTrue(EndDay()));
                ASSERT_THAT(IsFalse(Building.IsDead()));
                ASSERT_THAT(AreEqual(1, Construction->GetNextTierIndex()));
            });
    }

    TEST_METHOD(MainBuildingHasTwoUpgradesAndKeepsDamageDuringConstruction)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                AMainBuilding& Building = SpawnBuilding<AMainBuilding>();
                ATowerDefensePlayerController& Player = SpawnPlayer();
                UBuildingConstructionComponent* Construction = Building.GetConstructionComponent();
                ASSERT_THAT(AreEqual(1, Construction->GetNextTierIndex()));
                ASSERT_THAT(IsFalse(Building.IsDead()));
                Building.GetAbilitySystemComponent()->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), 50.f);
                for (int32 Tier = 2; Tier <= 3; ++Tier)
                {
                    ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                    ASSERT_THAT(IsTrue(EndDay()));
                    ASSERT_THAT(AreEqual(Tier, Construction->GetNextTierIndex()));
                    ASSERT_THAT(IsNear(50.f, Building.GetAbilitySystemComponent()->GetNumericAttribute(UBuildingAttributeSet::GetHealthAttribute()), 0.001f));
                }
                ASSERT_THAT(IsFalse(RequestConstruction(Building, Player)));
            });
    }

    TEST_METHOD(MainBuildingUsesCompletedTiersRuinsWithOneMeshComponent)
    {
        TestCommandBuilder.Do(
            [this]
            {
                const TArray<UStaticMesh*> RuinsMeshes = { LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")),
                                                           LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")),
                                                           LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone")) };
                ATowerDefensePlayerController& Player = SpawnPlayer(1000.f);
                for (int32 CompletedTier = 1; CompletedTier <= 3; ++CompletedTier)
                {
                    Run->StartNewRun();
                    AMainBuilding& Building = SpawnBuilding<AMainBuilding>();
                    UBuildingConstructionComponent* Construction = Building.GetConstructionComponent();
                    TInlineComponentArray<UStaticMeshComponent*> Meshes(&Building);
                    ASSERT_THAT(AreEqual(1, Meshes.Num()));
                    for (int32 Index = 0; Index < 3; ++Index)
                    {
                        GetTiers(Building)[Index].DestroyedMesh = RuinsMeshes[Index];
                        GetTiers(Building)[Index].FinishedMesh = RuinsMeshes[(Index + 1) % 3];
                        GetTiers(Building)[Index].ConstructionMesh = RuinsMeshes[(Index + 2) % 3];
                    }
                    for (int32 Tier = 1; Tier < CompletedTier; ++Tier)
                    {
                        ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                        ASSERT_THAT(IsTrue(EndDay()));
                    }
                    if (CompletedTier < 3)
                    {
                        ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                    }
                    Building.OnDeath();
                    ASSERT_THAT(IsTrue(Building.IsDead()));
                    ASSERT_THAT(AreEqual(RuinsMeshes[CompletedTier - 1], Meshes[0]->GetStaticMesh()));
                    ASSERT_THAT(AreEqual(0, Construction->GetRemainingDays()));
                    ASSERT_THAT(AreEqual(ERunPhase::RunFailed, Run->GetRunState().Phase));
                    Building.Destroy();
                }
            });
    }

    TEST_METHOD(MainBuildingFailsRunBeforeDeathNotification)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                AMainBuilding& Building = SpawnBuilding<AMainBuilding>();
                ATowerDefensePlayerController& Player = SpawnPlayer();
                ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                bool bFailedAtDeath = false;
                bool bDeadAtNotification = false;
                int32 DeathCount = 0;
                const FDelegateHandle Handle = Building.GetDeathDelegate().AddLambda(
                    [this, &Building, &bFailedAtDeath, &bDeadAtNotification, &DeathCount]
                    {
                        ++DeathCount;
                        bFailedAtDeath = Run->GetRunState().Phase == ERunPhase::RunFailed;
                        bDeadAtNotification = Building.IsDead();
                        Run->NotifyCombatWon();
                    });
                Building.GetAbilitySystemComponent()->SetNumericAttributeBase(UBuildingAttributeSet::GetHealthAttribute(), 0.f);
                Building.OnDeath();
                Building.GetDeathDelegate().Remove(Handle);
                ASSERT_THAT(AreEqual(1, DeathCount));
                ASSERT_THAT(IsTrue(bDeadAtNotification));
                ASSERT_THAT(IsTrue(bFailedAtDeath));
                ASSERT_THAT(AreEqual(ERunPhase::RunFailed, Run->GetRunState().Phase));
                ASSERT_THAT(IsFalse(Run->AdvanceDay()));
                ASSERT_THAT(IsFalse(Run->NotifyCombatStarted()));
                ASSERT_THAT(IsTrue(IsValid(&Building)));
            });
    }

    TEST_METHOD(DayEndIsNotReentrantAndFinishesBeforeNextDay)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                ARegularBuilding& Building = SpawnBuilding<ARegularBuilding>();
                ATowerDefensePlayerController& Player = SpawnPlayer();
                UBuildingConstructionComponent* Construction = Building.GetConstructionComponent();
                ASSERT_THAT(IsTrue(RequestConstruction(Building, Player)));
                bool bReentered = false;
                int32 DayAtEnd = 0;
                bool bBuiltBeforeSelection = false;
                const FDelegateHandle EndHandle = Run->OnDayEnded.AddLambda(
                    [this, &bReentered, &DayAtEnd]
                    {
                        DayAtEnd = Run->GetRunState().CurrentDay;
                        bReentered = Run->AdvanceDay();
                    });
                const FDelegateHandle PhaseHandle = Run->OnPhaseChanged.AddLambda(
                    [Construction, &bBuiltBeforeSelection](ERunPhase Phase)
                    {
                        if (Phase == ERunPhase::DaySelection)
                        {
                            bBuiltBeforeSelection = Construction->GetNextTierIndex() == 1;
                        }
                    });
                ASSERT_THAT(IsTrue(EndDay()));
                Run->OnDayEnded.Remove(EndHandle);
                Run->OnPhaseChanged.Remove(PhaseHandle);
                ASSERT_THAT(IsFalse(bReentered));
                ASSERT_THAT(AreEqual(1, DayAtEnd));
                ASSERT_THAT(IsTrue(bBuiltBeforeSelection));
            });
    }

    TEST_METHOD(ConstructionRechecksAvailabilityAfterPayment)
    {
        TestCommandBuilder.Do(
            [this]
            {
                Run->StartNewRun();
                ARegularBuilding& Building = SpawnBuilding<ARegularBuilding>();
                ATowerDefensePlayerController& Player = SpawnPlayer();
                UAbilitySystemComponent* Wallet = Player.GetAbilitySystemComponent();
                const FDelegateHandle Handle = Wallet->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMoneyAttribute())
                                                   .AddLambda([this](const FOnAttributeChangeData&) { Run->FailRun(); });
                const bool bStarted = RequestConstruction(Building, Player);
                Wallet->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMoneyAttribute()).Remove(Handle);
                ASSERT_THAT(IsFalse(bStarted));
                ASSERT_THAT(IsTrue(Building.IsDead()));
                ASSERT_THAT(AreEqual(0, Building.GetConstructionComponent()->GetRemainingDays()));
                ASSERT_THAT(AreEqual(ERunPhase::RunFailed, Run->GetRunState().Phase));
            });
    }

    TEST_METHOD(MainBuildingCancelsDelayedWave)
    {
        TestCommandBuilder.Do(
            [this]
            {
                AWavesManager& Manager = Spawner.SpawnActor<AWavesManager>();
                AWayActor& Way = Spawner.SpawnActor<AWayActor>();
                Manager.AddSpawner(&Way);
                Manager.DispatchBeginPlay();
                FWaveDefinition Definition;
                Definition.WaveStartDelay = 100.f;
                Definition.SpawnGroup.Add(ATowerDefenseAICharacter::StaticClass());
                Way.SetWaveDefinition(Definition);
                AMainBuilding& Building = SpawnBuilding<AMainBuilding>();
                Building.OnDeath();
                ASSERT_THAT(AreEqual(ERunPhase::RunFailed, Run->GetRunState().Phase));
                Spawner.GetWorld().GetTimerManager().Tick(200.f);
                TArray<AActor*> Enemies;
                UGameplayStatics::GetAllActorsOfClass(&Spawner.GetWorld(), ATowerDefenseAICharacter::StaticClass(), Enemies);
                ASSERT_THAT(IsTrue(Enemies.IsEmpty()));
                ASSERT_THAT(IsFalse(Manager.CanStartWave()));
                ASSERT_THAT(IsFalse(Manager.IsWaveActive()));
            });
    }
};

#endif
