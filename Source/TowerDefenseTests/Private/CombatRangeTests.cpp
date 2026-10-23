#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS
#include "AI/TowerDefenseAICharacter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatRangeTest, "TowerDefense.Combat.AttackCellShapes",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatRangeTest::RunTest(const FString& Parameters)
{
    for (int32 X = -4; X <= 4; ++X)
    {
        for (int32 Y = -4; Y <= 4; ++Y)
        {
            const FIntVector2 Delta(X, Y);
            const FString Cell = FString::Printf(TEXT("(%d, %d)"), X, Y);
            const bool bOrigin = X == 0 && Y == 0;
            const bool bAdjacent = !bOrigin && FMath::Abs(X) <= 1 && FMath::Abs(Y) <= 1;
            const bool bTwoHanded = bAdjacent || (FMath::Abs(X) == 2 && Y == 0) || (X == 0 && FMath::Abs(Y) == 2);
            TestEqual(TEXT("Shield ") + Cell, ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::Adjacent, Delta, 1), bAdjacent);
            TestEqual(TEXT("Two handed ") + Cell, ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::TwoHanded, Delta, 2), bTwoHanded);
        }
    }
    TestTrue(TEXT("Archer reaches three cells"), ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::Ranged, {3, 0}, 3));
    TestTrue(TEXT("Archer reaches diagonal inside circle"), ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::Ranged, {2, 2}, 3));
    TestFalse(TEXT("Archer cannot reach outside circle"), ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::Ranged, {3, 1}, 3));
    TestFalse(TEXT("Archer cannot attack its own cell"), ATowerDefenseAICharacter::IsCellInAttackRange(EUnitAttackPattern::Ranged, {0, 0}, 3));
    return true;
}
#endif
