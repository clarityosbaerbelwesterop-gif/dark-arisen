#include "DarkArisenOps.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
FString AlphaBuildScript(const FString& Engine)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/Build.bat"));
#else
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/Linux/Build.sh"));
#endif
}

FString AlphaEditorCommand(const FString& Engine)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Engine, TEXT("Engine/Binaries/Win64/UnrealEditor-Cmd.exe"));
#else
    return FPaths::Combine(Engine, TEXT("Engine/Binaries/Linux/UnrealEditor-Cmd"));
#endif
}

bool BuildEditorForMaterialisation(const FString& Root, const FString& Engine)
{
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    return RunProcess(AlphaBuildScript(Engine), {
        TEXT("DarkArisenEditor"),
        PlatformName(),
        TEXT("Development"),
        FString::Printf(TEXT("-Project=%s"), *Project),
        TEXT("-WaitMutex"),
        TEXT("-NoHotReloadFromIDE"),
        TEXT("-WarningsAsErrors")});
}

bool RunMaterialiser(const FString& Root, const FString& Engine, const FString& Commandlet)
{
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    const FString Editor = AlphaEditorCommand(Engine);
    return RunProcess(Editor, {
        Project,
        FString::Printf(TEXT("-run=%s"), *Commandlet),
        TEXT("-Unattended"),
        TEXT("-NoPause"),
        TEXT("-NoSplash"),
        TEXT("-NoP4"),
        TEXT("-NullRHI")}, nullptr, nullptr, Root);
}

bool RequiredAlphaContentExists(const FString& Root)
{
    const TArray<FString> Required = {
        TEXT("Content/Alpha/Maps/L_AlphaStartup.umap"),
        TEXT("Content/Alpha/Maps/L_HarlowOpening.umap"),
        TEXT("Content/Alpha/Maps/L_DriftwoodBeach.umap"),
        TEXT("Content/Alpha/Maps/L_RexaHarbor.umap"),
        TEXT("Content/Alpha/Maps/L_RexaSafeRoutes.umap"),
        TEXT("Content/Alpha/Maps/L_CrownCitadelApproach.umap"),
        TEXT("Content/Alpha/Maps/L_SaltAndIron_Blockade.umap"),
        TEXT("Content/Alpha/Maps/L_BrokenCompact.umap"),
        TEXT("Content/Alpha/Maps/L_HerrerasFall.umap"),
        TEXT("Content/Alpha/Characters/Jake/SK_Jake_Alpha.uasset"),
        TEXT("Content/Alpha/Characters/Jake/SK_Jake_Alpha_Skeleton.uasset"),
        TEXT("Content/Alpha/Characters/Boarders/SK_Boarder_Alpha.uasset"),
        TEXT("Content/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.uasset"),
        TEXT("Content/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.uasset")};
    bool bOk = true;
    for (const FString& Relative : Required)
    {
        const FString Path = FPaths::Combine(Root, Relative);
        if (!IFileManager::Get().FileExists(*Path))
        {
            UE_LOG(LogTemp, Error, TEXT("Required materialised Alpha content is missing: %s"), *Path);
            bOk = false;
        }
    }
    return bOk;
}
}

int32 AlphaBuildCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    const FString Engine = EngineRoot(Args);
    if (RunnerCheckCommand(Args) != 0)
    {
        return 1;
    }
    if (!BuildEditorForMaterialisation(Root, Engine))
    {
        UE_LOG(LogTemp, Error, TEXT("Editor compile failed before Alpha materialisation."));
        return 1;
    }
    if (!RunMaterialiser(Root, Engine, TEXT("DarkArisenMaterializeAlpha"))
        || !RunMaterialiser(Root, Engine, TEXT("DarkArisenMaterializeStory"))
        || !RequiredAlphaContentExists(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Alpha physical materialisation failed closed; game builds will not proceed."));
        return 1;
    }
    const int32 BuildResult = BuildCommand(Args);
    if (BuildResult != 0)
    {
        return BuildResult;
    }
    if (!RequiredAlphaContentExists(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Materialised Alpha content disappeared during build."));
        return 1;
    }
    return 0;
}

int32 AlphaPackageCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    if (!RequiredAlphaContentExists(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Refusing Cook/Package without materialised Alpha opening and physical story content."));
        return 1;
    }
    return PackageAlphaCommand(Args);
}
}
