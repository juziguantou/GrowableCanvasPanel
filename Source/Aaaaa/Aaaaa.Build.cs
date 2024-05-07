
using UnrealBuildTool;

public class Aaaaa : ModuleRules
{
    public Aaaaa(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SlateCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

    }
}

