using UnrealBuildTool;

public class TowerDefenseTests : ModuleRules
{
    public TowerDefenseTests(ReadOnlyTargetRules Target) : base(Target)
    {
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new[]
            {
                "TowerDefenseUnitEditor",
                "UnrealEd"
            });
        }

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "CQTest",
                "Engine",
                "AIModule",
                "GameplayAbilities",
                "GameplayTags",
                "TowerDefense"
            });
    }
}
