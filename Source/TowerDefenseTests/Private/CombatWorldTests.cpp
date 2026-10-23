#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS
#include "CQTest.h"
#include "Components/ActorTestSpawner.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Actors/BaseBuilding.h"
#include "UObject/UnrealType.h"
#include "AbilitySystemComponent.h"
#include "Abilities/TowerDefenceTags.h"
#include "AI/TowerDefenseAICharacter.h"
#include "Actors/Projectile.h"
#include "Settings/TowerDefenseWorldSettings.h"
#include "Subsystems/WorldGridSubsystem.h"
#include "Utils/BaseUtils.h"

TEST_CLASS_WITH_FLAGS(FCombatWorldTest, "TowerDefense.Combat",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
    FActorTestSpawner Spawner;

    ATowerDefenseAICharacter& SpawnUnit(const FVector& Location, uint8 Team)
    {
        FActorSpawnParameters Parameters;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        ATowerDefenseAICharacter& Unit = Spawner.SpawnActorAt<ATowerDefenseAICharacter>(Location, FRotator::ZeroRotator, Parameters);
        Unit.SetGenericTeamId(Team);
        Unit.GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Unit.GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
        return Unit;
    }

    TEST_METHOD(CombatCellHasOneOwner)
    {
        AActor& First = Spawner.SpawnActor<AActor>();
        AActor& Second = Spawner.SpawnActor<AActor>();
        FGridCellInfo Cell;
        ASSERT_THAT(IsTrue(Cell.TryReserveCell(&First)));
        ASSERT_THAT(IsTrue(Cell.TryReserveCell(&First)));
        ASSERT_THAT(IsFalse(Cell.TryReserveCell(&Second)));
        ASSERT_THAT(IsFalse(Cell.IsAvailable()));
        Cell.FreeCell();
        Cell.Occupant = &First;
        ASSERT_THAT(IsFalse(Cell.TryReserveCell(&Second)));
        Cell.Occupant = nullptr;
        ASSERT_THAT(IsTrue(Cell.TryReserveCell(&Second)));
        Second.Destroy();
        ASSERT_THAT(IsTrue(Cell.IsAvailable()));
    }

    TEST_METHOD(TwoHandedStrikePassesAlliesButNotEnemies)
    {
        const ATowerDefenseWorldSettings* Settings = Cast<ATowerDefenseWorldSettings>(Spawner.GetWorld().GetWorldSettings());
        ASSERT_THAT(IsNotNull(Settings));
        const FVector Height(0., 0., 200.);
        ATowerDefenseAICharacter& Attacker = SpawnUnit(Settings->GridToWorldLocation({0, 0}) + Height, Teams::TeamPlayer);
        ATowerDefenseAICharacter& Target = SpawnUnit(Settings->GridToWorldLocation({2, 0}) + Height, Teams::TeamEnemy);
        FEnumProperty* PatternProperty = FindFProperty<FEnumProperty>(ATowerDefenseAICharacter::StaticClass(), TEXT("AttackPattern"));
        ASSERT_THAT(IsNotNull(PatternProperty));
        PatternProperty->GetUnderlyingProperty()->SetIntPropertyValue(PatternProperty->ContainerPtrToValuePtr<void>(&Attacker),
            static_cast<uint64>(EUnitAttackPattern::TwoHanded));
        ASSERT_THAT(IsTrue(Attacker.CanAttack(&Target)));
        ATowerDefenseAICharacter& Blocker = SpawnUnit(Settings->GridToWorldLocation({1, 0}) + Height, Teams::TeamPlayer);
        ASSERT_THAT(IsTrue(Attacker.CanAttack(&Target)));
        Blocker.SetGenericTeamId(Teams::TeamEnemy);
        ASSERT_THAT(IsFalse(Attacker.CanAttack(&Target)));
        ASSERT_THAT(IsTrue(Attacker.CanAttack(&Blocker)));
    }

    TEST_METHOD(UnitHitTargetUsesOverlapsAndIgnoresVisibility)
    {
        ATowerDefenseAICharacter& Attacker = SpawnUnit({0., 0., 200.}, Teams::TeamPlayer);
        ATowerDefenseAICharacter& Target = SpawnUnit({100., 0., 200.}, Teams::TeamEnemy);
        ASSERT_THAT(IsTrue(Target.GetCapsuleComponent()->GetCollisionResponseToChannel(Collision::UnitHitTarget) == ECR_Overlap));
        ASSERT_THAT(IsTrue(Attacker.CanAttack(&Target)));

        AActor& Wall = Spawner.SpawnActor<AActor>();
        UBoxComponent* Box = NewObject<UBoxComponent>(&Wall);
        Wall.SetRootComponent(Box);
        Box->SetBoxExtent({10., 50., 50.});
        Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Box->SetCollisionResponseToAllChannels(ECR_Ignore);
        Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        Box->RegisterComponent();
        Wall.SetActorLocation({50., 0., 200.});
        ASSERT_THAT(IsTrue(Attacker.CanAttack(&Target)));
        Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
        Box->SetCollisionResponseToChannel(Collision::UnitHitTarget, ECR_Block);
        ASSERT_THAT(IsFalse(Attacker.CanAttack(&Target)));
    }

    TEST_METHOD(BuildingHitBoxOverlapsUnitHitTarget)
    {
        ABaseBuilding& Building = Spawner.SpawnActor<ABaseBuilding>();
        const UBoxComponent* HitBox = Building.FindComponentByClass<UBoxComponent>();
        ASSERT_THAT(IsNotNull(HitBox));
        ASSERT_THAT(IsTrue(HitBox->IsQueryCollisionEnabled()));
        ASSERT_THAT(IsTrue(HitBox->GetCollisionResponseToChannel(Collision::UnitHitTarget) == ECR_Overlap));
    }

    TEST_METHOD(ArrowHitsFirstEnemyOnceAndIgnoresAlly)
    {
        ATowerDefenseAICharacter& Shooter = SpawnUnit({0., 0., 200.}, Teams::TeamPlayer);
        SpawnUnit({100., 0., 200.}, Teams::TeamPlayer);
        ATowerDefenseAICharacter& FirstEnemy = SpawnUnit({200., 0., 200.}, Teams::TeamEnemy);
        ATowerDefenseAICharacter& Target = SpawnUnit({300., 0., 200.}, Teams::TeamEnemy);
        int32 HitCount = 0;
        const AActor* HitTarget = nullptr;
        Shooter.GetAbilitySystemComponent()->GenericGameplayEventCallbacks.FindOrAdd(FGameplayTag::RequestGameplayTag(TEXT("Action.Damage.Take"))).AddLambda(
            [&HitCount, &HitTarget](const FGameplayEventData* Payload)
            {
                ++HitCount;
                HitTarget = Payload->Target.Get();
            });
        AProjectile& Projectile = Spawner.SpawnActorAt<AProjectile>({0., 0., 200.}, FRotator::ZeroRotator);
        Projectile.OnHitDelegate.BindLambda([](AProjectile*) {});
        Projectile.Activate(&Target, &Shooter);
        Projectile.SetActorLocation({400., 0., 200.});
        static_cast<AActor&>(Projectile).Tick(0.016f);
        ASSERT_THAT(AreEqual(1, HitCount));
        ASSERT_THAT(IsTrue(HitTarget == &FirstEnemy));
        static_cast<AActor&>(Projectile).Tick(0.016f);
        ASSERT_THAT(AreEqual(1, HitCount));
        Shooter.GetAbilitySystemComponent()->GenericGameplayEventCallbacks.Remove(FGameplayTag::RequestGameplayTag(TEXT("Action.Damage.Take")));
    }
};
#endif
