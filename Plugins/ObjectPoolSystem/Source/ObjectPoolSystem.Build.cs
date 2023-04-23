// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ObjectPoolSystem : ModuleRules
{
    public ObjectPoolSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG" });
    }
}