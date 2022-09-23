// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Busters : ModuleRules
{
	public Busters(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "HeadMountedDisplay",
            "UMG",
            "OnlineSubsystem",
            "OnlineSubsystemSteam"
        });
	}
}
