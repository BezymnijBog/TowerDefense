// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TowerDefense : ModuleRules
{
	public TowerDefense(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"GameplayAbilities",
			"GameplayTags",
			"Engine",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AIModule",
			"EnhancedInput",
			"GameplayTasks",
			"InputCore",
			"NavigationSystem",
			"Niagara",
		});
	}
}
