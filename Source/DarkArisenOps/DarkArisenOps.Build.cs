using UnrealBuildTool;

public class DarkArisenOps : ModuleRules
{
    public DarkArisenOps(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "Json"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.AddRange(new[]
            {
                "wintrust.lib",
                "crypt32.lib"
            });
        }
    }
}
