// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InteriorProject : ModuleRules
{
	public InteriorProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"ProceduralMeshComponent",
			"GeometryCore",
			"GeometryFramework", 
			"GeometryScriptingCore",
			"GeometryScriptingEditor",
			"DynamicMesh",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput",
			"Slate", 
			"SlateCore"
		});
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
