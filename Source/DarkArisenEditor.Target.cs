using UnrealBuildTool;
using System.Collections.Generic;

public class DarkArisenEditorTarget : TargetRules
{
    public DarkArisenEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        CppStandard = CppStandardVersion.Cpp20;
        ExtraModuleNames.AddRange(new[] { "DarkArisen", "DarkArisenEditor" });
    }
}
