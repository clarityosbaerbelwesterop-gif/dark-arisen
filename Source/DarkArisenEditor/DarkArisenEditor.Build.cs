using UnrealBuildTool;

public class DarkArisenEditor : ModuleRules
{
    public DarkArisenEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DarkArisen",
            "UnrealEd",
            "AssetTools",
            "AssetRegistry",
            "Json"
        });
    }
}
