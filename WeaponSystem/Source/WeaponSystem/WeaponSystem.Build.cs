// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WeaponSystem : ModuleRules
{
	public WeaponSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			[
				// ... add public include paths required here ...
				"WeaponSystem/Public"
			]
		);
				
		
		PrivateIncludePaths.AddRange(
			[
				// ... add other private include paths required here ...
				"WeaponsSystem/Private"
			]
		);
			
		
		PublicDependencyModuleNames.AddRange(
			[
				"Core"
				// ... add other public dependencies that you statically link with here ...
			]
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"NoctAbilitySystem",
				"Niagara"
				// ... add private dependencies that you statically link with here ...	
			]
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			[
				// ... add any modules that your module loads dynamically here ...
			]
		);
	}
}
