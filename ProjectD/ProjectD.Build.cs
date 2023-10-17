// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectD : ModuleRules
{
    public ProjectD(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
            new string[] {
                        "ProjectD"
            }
        );

                PrivateIncludePaths.AddRange(
            new string[] {
            }
        );


        

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "HeadMountedDisplay",
                "EnhancedInput",
                "GameplayTags",
                "GameplayTasks",
                "GameplayAbilities",                
                "GameFeatures",                
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "InputCore",
                "Slate",
                "SlateCore",
                "RenderCore",
                "DeveloperSettings",
                "EnhancedInput",
                "CommonUI",
                "CommonInput",
                "CommonGame",
                "CommonUser",
            }
        );
}
}
