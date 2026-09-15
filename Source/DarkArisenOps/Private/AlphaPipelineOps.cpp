#include "DarkArisenOps.h"
#include "OpsCommon.h"

#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"

namespace
{
int32 RunUbtTarget(const FString& Root, const FString& EngineRoot, const FString& Target, const FString& Platform, const FString& Configuration)
{
    const FString Ubt = FPaths::Combine(EngineRoot, TEXT("Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool"));
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    FString StdOut;
    FString StdErr;
    const int32 Exit = DarkArisenOpsCommon::RunProcess(Ubt, FString::Printf(TEXT("%s %s %s -Project=\"%s\" -WaitMutex"), *Target, *Platform, *Configuration, *Project), Root, StdOut, StdErr);
    UE_LOG(LogTemp, Display, TEXT("%s"), *StdOut);
    if (Exit != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *StdErr);
    }
    return Exit;
}

int32 RunMaterialiser(const FString& Root, const FString& EngineRoot, const FString& Commandlet)
{
    const FString EditorCmd = FPaths::Combine(EngineRoot, TEXT("Engine/Binaries/Win64/UnrealEditor-Cmd.exe"));
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    if (!FPaths::FileExists(EditorCmd))
    {
        UE_LOG(LogTemp, Error, TEXT("Alpha materialisation requires UnrealEditor-Cmd.exe at %s"), *EditorCmd);
        return 1;
    }

    FString StdOut;
    FString StdErr;
    const int32 Exit = DarkArisenOpsCommon::RunProcess(EditorCmd, FString::Printf(TEXT("\"%s\" -run=%s -unattended -nop4 -nosplash -NullRHI"), *Project, *Commandlet), Root, StdOut, StdErr);
    UE_LOG(LogTemp, Display, TEXT("%s"), *StdOut);
    if (Exit != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *StdErr);
    }
    return Exit;
}

bool HasRequiredMaterializedAlphaContent(const FString& Root)
{
    const TArray<FString> RequiredFiles = {
        TEXT("Content/Alpha/Characters/Jake/SK_Jake_Alpha.uasset"),
        TEXT("Content/Alpha/Characters/Boarders/SK_Boarder_Alpha.uasset"),
        TEXT("Content/Alpha/Maps/L_AlphaStartup.umap"),
        TEXT("Content/Alpha/Maps/L_HarlowOpening.umap"),
        TEXT("Content/Alpha/Maps/L_DriftwoodBeach.umap"),
        TEXT("Content/Alpha/Maps/L_DriftwoodCamp.umap"),
        TEXT("Content/Alpha/Maps/L_MirasCove.umap"),
        TEXT("Content/Alpha/Maps/L_MangroveShallows.umap"),
        TEXT("Content/Alpha/Maps/L_KoaTradingPost.umap"),
        TEXT("Content/Alpha/Maps/L_GalleonCove.umap"),
        TEXT("Content/Alpha/Maps/L_OpenSea_FirstWake.umap"),
        TEXT("Content/Alpha/Maps/L_RexaHarbor.umap"),
        TEXT("Content/Alpha/Maps/L_RexaSafeRoutes.umap"),
        TEXT("Content/Alpha/Maps/L_CrownCitadelApproach.umap"),
        TEXT("Content/Alpha/Maps/L_SaltAndIron_Blockade.umap"),
        TEXT("Content/Alpha/Maps/L_BrokenCompact.umap"),
        TEXT("Content/Alpha/Maps/L_HerrerasFall.umap"),
        TEXT("Content/Alpha/Maps/L_WarCurrent.umap"),
        TEXT("Content/Alpha/Maps/L_HoldersWake.umap"),
        TEXT("Content/Alpha/Maps/L_NoSafeHarbor.umap"),
        TEXT("Content/Alpha/Maps/L_HighmooreRoad.umap"),
        TEXT("Content/Alpha/Maps/L_CrystalPassage.umap"),
        TEXT("Content/Alpha/Maps/L_TheNorthernOath.umap"),
        TEXT("Content/Alpha/Maps/L_FalseBearings.umap"),
        TEXT("Content/Alpha/Maps/L_EthansMarks.umap"),
        TEXT("Content/Alpha/Maps/L_ThroughTheNet.umap"),
        TEXT("Content/Alpha/Maps/L_ThePrisonCourse.umap"),
        TEXT("Content/Alpha/Maps/L_BrothersAlive.umap"),
        TEXT("Content/Alpha/Maps/L_HomewardBearing.umap")
    };

    for (const FString& Relative : RequiredFiles)
    {
        if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
        {
            UE_LOG(LogTemp, Error, TEXT("Required materialized Alpha content is missing: %s"), *Relative);
            return false;
        }
    }
    return true;
}
}

int32 FDarkArisenOps::RunBuild(const TArray<FString>& Args)
{
    const FString Root = DarkArisenOpsCommon::ResolveRepoRoot(Args);
    FString EngineRoot;
    FString Error;
    if (!DarkArisenOpsCommon::ResolveEngineRoot(Args, EngineRoot, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }

    FString Platform = DarkArisenOpsCommon::GetArgValue(Args, TEXT("--platform"));
    if (Platform.IsEmpty()) Platform = TEXT("Win64");
    FString Configuration = DarkArisenOpsCommon::GetArgValue(Args, TEXT("--configuration"));
    if (Configuration.IsEmpty()) Configuration = TEXT("Development");

    const int32 EditorExit = RunUbtTarget(Root, EngineRoot, TEXT("DarkArisenEditor"), Platform, TEXT("Development"));
    if (EditorExit != 0) return EditorExit;

    const int32 MaterialiseExit = RunMaterialiser(Root, EngineRoot, TEXT("DarkArisenMaterializeAlpha"));
    if (MaterialiseExit != 0) return MaterialiseExit;
    const int32 StoryMaterialiseExit = RunMaterialiser(Root, EngineRoot, TEXT("DarkArisenMaterializeStory"));
    if (StoryMaterialiseExit != 0) return StoryMaterialiseExit;
    if (!HasRequiredMaterializedAlphaContent(Root)) return 1;

    return RunUbtTarget(Root, EngineRoot, TEXT("DarkArisen"), Platform, Configuration);
}

int32 FDarkArisenOps::RunPackage(const TArray<FString>& Args)
{
    const FString Root = DarkArisenOpsCommon::ResolveRepoRoot(Args);
    FString EngineRoot;
    FString Error;
    if (!DarkArisenOpsCommon::ResolveEngineRoot(Args, EngineRoot, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    if (!HasRequiredMaterializedAlphaContent(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Package refused: run DarkArisenOps build so authored Alpha source is materialized before cook."));
        return 1;
    }

    FString Platform = DarkArisenOpsCommon::GetArgValue(Args, TEXT("--platform"));
    if (Platform.IsEmpty()) Platform = TEXT("Win64");
    const FString RunUAT = FPaths::Combine(EngineRoot, TEXT("Engine/Build/BatchFiles/RunUAT.bat"));
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    const FString Archive = FPaths::Combine(Root, TEXT("Artifacts"), Platform);
    const FString UatPlatform = Platform.Equals(TEXT("Linux"), ESearchCase::IgnoreCase) ? TEXT("Linux") : TEXT("Win64");
    const FString Params = FString::Printf(TEXT("BuildCookRun -project=\"%s\" -noP4 -platform=%s -clientconfig=Shipping -build -cook -stage -pak -archive -archivedirectory=\"%s\""), *Project, *UatPlatform, *Archive);
    FString StdOut;
    FString StdErr;
    const int32 Exit = DarkArisenOpsCommon::RunProcess(RunUAT, Params, Root, StdOut, StdErr);
    UE_LOG(LogTemp, Display, TEXT("%s"), *StdOut);
    if (Exit != 0) UE_LOG(LogTemp, Error, TEXT("%s"), *StdErr);
    return Exit;
}
