// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdinWwiseAdapter : ModuleRules
{
	public OdinWwiseAdapter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Odin",
				"AkAudio",
				"Wwise",
				"OdinLibrary"
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}