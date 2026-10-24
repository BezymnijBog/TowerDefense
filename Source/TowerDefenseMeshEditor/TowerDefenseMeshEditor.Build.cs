using UnrealBuildTool;

public class TowerDefenseMeshEditor : ModuleRules
{
    public TowerDefenseMeshEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        PrivateDependencyModuleNames.AddRange(new[]
        {
            "UnrealEd",
            "Slate",
            "SlateCore",
            "InputCore",
            "ToolMenus",
            "PropertyEditor",
            "AssetRegistry",
            "ContentBrowser",
            "MeshDescription",
            "StaticMeshDescription",
            "SkeletalMeshDescription",
            "SkeletalMeshUtilitiesCommon",
            "AnimationCore",
            "RenderCore"
        });
    }
}
