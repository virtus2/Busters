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
            "NetCore",
            "PhysicsCore",
            "GameplayTags",
            "AnimGraphRuntime",
            "ControlRig",
            "RigVM",
            "Niagara",
            "UMG",
            "EnhancedInput",
            "OnlineSubsystem",
            "OnlineSubsystemSteam",
            "ALS",
            "ALSCamera",
            "ALSEditor",
            "ALSExtras",
        });
	}
}
