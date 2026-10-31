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
                "TowerDefenseMeshEditor",
                "MeshDescription",
                "SkeletalMeshDescription",
                "StaticMeshDescription",
                "AnimationCore",
                "AssetRegistry",
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
                "EnhancedInput",
                "AIModule",
                "GameplayAbilities",
                "GameplayTags",
                "UMG",
                "TowerDefense"
            });
    }
}
