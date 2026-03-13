// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Practce : ModuleRules
{
	public Practce(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "AIModule", "StateTreeModule",
			"GameplayStateTreeModule", "UMG", "Slate",
			"HTTP", "Json", "JsonUtilities"
		});


		PrivateDependencyModuleNames.AddRange(new string[] { "IoStoreHttpClient" });

		PublicIncludePaths.AddRange(new string[] {
			"Practce",
			"Practce/Variant_Horror",
			"Practce/Variant_Horror/UI",
			"Practce/Variant_Shooter",
			"Practce/Variant_Shooter/AI",
			"Practce/Variant_Shooter/UI",
			"Practce/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
