#include "DarkArisenOps.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
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
        TEXT("Content/Alpha/Maps/L_HomewardBearing.umap"),
        TEXT("Content/Alpha/Maps/L_EthansGrove.umap"),
        TEXT("Content/Alpha/Maps/L_MemoryOfABrother.umap"),
        TEXT("Content/Alpha/Maps/L_DreamFight.umap"),
        TEXT("Content/Alpha/Maps/L_WakingCourse.umap"),
        TEXT("Content/Alpha/Maps/L_Armada.umap"),
        TEXT("Content/Alpha/Maps/L_BreakTheChain.umap"),
        TEXT("Content/Alpha/Maps/L_BlackDeck.umap"),
        TEXT("Content/Alpha/Maps/L_DravenVoss.umap"),
        TEXT("Content/Alpha/Maps/L_TheWakeAfter.umap"),
        TEXT("Content/Alpha/Maps/L_Credits.umap")};
    for (const FString& Relative : RequiredFiles)
    {
        const FString Path = FPaths::Combine(Root, Relative);
        if (IFileManager::Get().FileSize(*Path) <= 0)
        {
            UE_LOG(LogTemp, Error, TEXT("Required materialized Alpha content is missing or empty: %s"), *Relative);
            return false;
        }
    }
    return true;
}
}

bool MaterializeAlphaContent(const FString& Root, const FString& Engine)
{
#if PLATFORM_WINDOWS
    const FString Editor = FPaths::Combine(Engine, TEXT("Engine/Binaries/Win64/UnrealEditor-Cmd.exe"));
#else
    const FString Editor = FPaths::Combine(Engine, TEXT("Engine/Binaries/Linux/UnrealEditor-Cmd"));
#endif
    if (!FPaths::FileExists(Editor))
    {
        UE_LOG(LogTemp, Error, TEXT("Alpha materialisation requires UnrealEditor-Cmd: %s"), *Editor);
        return false;
    }
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    for (const TCHAR* Commandlet : {
        TEXT("DarkArisenMaterializeAlpha"), TEXT("DarkArisenMaterializeStory"), TEXT("DarkArisenMaterializeNaval")})
    {
        if (!RunProcess(Editor, {
            Project, FString::Printf(TEXT("-run=%s"), Commandlet),
            TEXT("-Unattended"), TEXT("-NoP4"), TEXT("-NoSplash"), TEXT("-NullRHI")},
            nullptr, nullptr, Root))
        {
            UE_LOG(LogTemp, Error, TEXT("Alpha materialisation failed: %s"), Commandlet);
            return false;
        }
    }
    return HasRequiredMaterializedAlphaContent(Root);
}

int32 AlphaBuildCommand(const FParsedArgs& Args)
{
    // Reuse runner checks, platform build scripts, warning policy and Unreal Automation.
    return BuildCommand(Args);
}

int32 AlphaPackageCommand(const FParsedArgs& Args)
{
    if (!HasRequiredMaterializedAlphaContent(RepoRoot(Args)))
    {
        UE_LOG(LogTemp, Error, TEXT("Package refused: run DarkArisenOps build before cooking Alpha content."));
        return 1;
    }
    // Preserve the clean-revision check, credential scan, manifest and immutable candidate hash.
    return PackageAlphaCommand(Args);
}
}
