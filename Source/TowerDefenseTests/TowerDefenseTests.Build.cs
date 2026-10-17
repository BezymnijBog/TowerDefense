using UnrealBuildTool;

public class TowerDefenseTests : ModuleRules
{
    public TowerDefenseTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "CQTest",
            "Engine",
            "TowerDefense"
        });
    }
}
