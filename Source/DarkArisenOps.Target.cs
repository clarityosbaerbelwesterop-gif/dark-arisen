using UnrealBuildTool;

public class DarkArisenOpsTarget : TargetRules
{
    public DarkArisenOpsTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Program;
        LinkType = TargetLinkType.Monolithic;
        LaunchModuleName = "DarkArisenOps";
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        bCompileAgainstEngine = false;
        bCompileAgainstCoreUObject = false;
        bCompileAgainstApplicationCore = false;
        bBuildDeveloperTools = false;
        bBuildWithEditorOnlyData = false;
        bIsBuildingConsoleApplication = true;
        bUseLoggingInShipping = true;
    }
}
