// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PingleStudioTestTask : ModuleRules
{
	public PingleStudioTestTask(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
