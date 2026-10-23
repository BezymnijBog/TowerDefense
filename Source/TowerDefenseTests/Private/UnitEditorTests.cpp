// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && WITH_EDITOR
#include "AI/TowerDefenseAICharacter.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "UnitEditorModel.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
const FName ExtraRow(TEXT("OtherAttributeSet.KeepMe"));

void VerifySavedUnit(FAutomationTestBase& Test, const UUnitEditorModel& Model, const UUnitEditorModel& Reloaded, UDataTable* Table)
{
    Test.TestEqual(TEXT("Team persisted"), Reloaded.Team, EUnitEditorTeam::Enemy);
    Test.TestEqual(TEXT("Pattern persisted"), Reloaded.AttackPattern, EUnitAttackPattern::Ranged);
    Test.TestEqual(TEXT("Centimetres persisted"), Reloaded.AttackRange, 350.f);
    Test.TestEqual(TEXT("Cells persisted"), Reloaded.AttackRadiusInCells, 3);
    Test.TestEqual(TEXT("ASC references independent table"), Reloaded.AttributeTable.Get(), Table);
    Test.TestEqual(TEXT("Mesh persisted"), Reloaded.SkeletalMesh.Get(), Model.SkeletalMesh.Get());
    Test.TestEqual(TEXT("Animation persisted"), Reloaded.AnimationClass.Get(), Model.AnimationClass.Get());
    Test.TestEqual(TEXT("Controller persisted"), Reloaded.ControllerClass.Get(), Model.ControllerClass.Get());
    Test.TestEqual(TEXT("Abilities preserved"), Reloaded.DefaultAbilities.Num(), Model.DefaultAbilities.Num());
    const FAttributeMetaData* Damage = Table->FindRow<FAttributeMetaData>(TEXT("AICharacterAttributeSet.Damage"), TEXT("Test"));
    if (Test.TestNotNull(TEXT("Damage row"), Damage))
    {
        Test.TestEqual(TEXT("Damage value"), Damage->BaseValue, 37.f);
    }
    Test.TestNotNull(TEXT("Unrelated rows preserved"), Table->FindRow<FAttributeMetaData>(ExtraRow, TEXT("Test")));
}
} // namespace

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnitEditorPathsTest,
                                 "TowerDefense.UnitEditor.AssetPaths",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnitEditorPathsTest::RunTest(const FString& Parameters)
{
    FString BlueprintPath;
    FString TablePath;
    FText Error;
    TestTrue(TEXT("Enemy paths"), UUnitEditorModel::MakeAssetPaths(TEXT("Bowman"), EUnitEditorTeam::Enemy, BlueprintPath, TablePath, Error));
    TestEqual(TEXT("Enemy Blueprint"), BlueprintPath, FString(TEXT("/Game/TowerDefense/Core/Actors/Enemy/BP_Enemy_Bowman")));
    TestEqual(TEXT("Enemy table"), TablePath, FString(TEXT("/Game/TowerDefense/Core/Miscellaneous/DefaultAttributes/Enemy/DT_EnemyAttributes_Bowman")));
    TestTrue(TEXT("Defender paths"), UUnitEditorModel::MakeAssetPaths(TEXT("Guard"), EUnitEditorTeam::Defender, BlueprintPath, TablePath, Error));
    TestEqual(TEXT("Defender Blueprint"), BlueprintPath, FString(TEXT("/Game/TowerDefense/Core/Actors/Defenders/BP_Defender_Guard")));
    TestEqual(TEXT("Defender table"), TablePath, FString(TEXT("/Game/TowerDefense/Core/Miscellaneous/DefaultAttributes/Defender/DT_DefenderAttributes_Guard")));
    TestFalse(TEXT("No path traversal"), UUnitEditorModel::MakeAssetPaths(TEXT("../Guard"), EUnitEditorTeam::Defender, BlueprintPath, TablePath, Error));
    TestFalse(TEXT("No empty names"), UUnitEditorModel::MakeAssetPaths(TEXT(""), EUnitEditorTeam::Defender, BlueprintPath, TablePath, Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnitEditorRoundTripTest,
                                 "TowerDefense.UnitEditor.BlueprintAndAttributesRoundTrip",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnitEditorRoundTripTest::RunTest(const FString& Parameters)
{
    UBlueprint* Source = LoadObject<UBlueprint>(nullptr, TEXT("/Game/TowerDefense/Core/Actors/Defenders/Ranged/BP_Archer.BP_Archer"));
    if (!TestNotNull(TEXT("Existing archer Blueprint"), Source))
    {
        return false;
    }
    TStrongObjectPtr<UUnitEditorModel> Model(NewObject<UUnitEditorModel>());
    FText Error;
    if (!TestTrue(TEXT("Load existing unit"), Model->LoadUnit(Source, Error)))
    {
        AddError(Error.ToString());
        return false;
    }
    if (!TestNotNull(TEXT("Table read from AbilitySystem"), Model->AttributeTable.Get()))
    {
        return false;
    }
    UDataTable* SourceTable = Model->AttributeTable;
    const FString OriginalTable = SourceTable->GetTableAsJSON();
    TStrongObjectPtr<UBlueprint> Copy(DuplicateObject<UBlueprint>(
        Source, GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("UnitEditorTest"))));
    TStrongObjectPtr<UDataTable> Table(DuplicateObject<UDataTable>(SourceTable, GetTransientPackage()));
    FAttributeMetaData Extra;
    Extra.BaseValue = 123.f;
    Table->AddRow(ExtraRow, Extra);
    Model->LoadedUnit = Copy.Get();
    Model->Team = EUnitEditorTeam::Enemy;
    Model->AttackPattern = EUnitAttackPattern::Ranged;
    Model->AttackRange = 350.f;
    Model->AttackRadiusInCells = 3;
    for (FUnitEditorAttribute& Attribute : Model->Attributes)
    {
        if (Attribute.Name == TEXT("Damage"))
        {
            Attribute.BaseValue = 37.f;
        }
    }
    if (!TestTrue(TEXT("Apply and compile"), Model->ApplyTo(Copy.Get(), Table.Get(), Error)))
    {
        AddError(Error.ToString());
        return false;
    }
    TStrongObjectPtr<UUnitEditorModel> Reloaded(NewObject<UUnitEditorModel>());
    if (!TestTrue(TEXT("Reload compiled Blueprint"), Reloaded->LoadUnit(Copy.Get(), Error)))
    {
        return false;
    }
    VerifySavedUnit(*this, *Model, *Reloaded, Table.Get());
    TestEqual(TEXT("Original table untouched"), SourceTable->GetTableAsJSON(), OriginalTable);
    return true;
}
#endif
