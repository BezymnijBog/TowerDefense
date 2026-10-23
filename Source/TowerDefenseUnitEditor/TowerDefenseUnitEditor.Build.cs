using UnrealBuildTool;

public class TowerDefenseUnitEditor : ModuleRules
{
    public TowerDefenseUnitEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "TowerDefense", "GameplayAbilities", "AIModule" });
        PrivateDependencyModuleNames.AddRange(
            new[] { "UnrealEd", "Slate", "SlateCore", "PropertyEditor", "ToolMenus", "AssetRegistry", "AssetTools", "Kismet", "InputCore" });
    }
}
