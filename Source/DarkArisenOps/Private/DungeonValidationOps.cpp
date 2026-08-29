#include "DarkArisenOps.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing authored-dungeon file: %s"), *Relative));
    }
}

void RequireFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read authored-dungeon file: %s"), *Relative));
        return;
    }

    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing dungeon contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanDungeonSourceForForbiddenRuntime(const FString& Root, TArray<FString>& Errors)
{
    const TArray<FString> Roots = {
        FPaths::Combine(Root, TEXT("Source/DarkArisen/ContentScale")),
        FPaths::Combine(Root, TEXT("Source/DarkArisen/Dungeons"))
    };

    const TArray<FString> Forbidden = {
        TEXT("FMath::Rand"),
        TEXT("FRandomStream"),
        TEXT("GenerateRadiant"),
        TEXT("RandomInteger"),
        TEXT("Minimap"),
        TEXT("CompassWidget"),
        TEXT("PlayerDot"),
        TEXT("TeleportToWaypoint"),
        TEXT("OpenLevel(")
    };

    for (const FString& ScanRoot : Roots)
    {
        TArray<FString> Files;
        IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
        IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);

        for (const FString& File : Files)
        {
            FString Text;
            if (!ReadText(File, Text))
            {
                Errors.Add(FString::Printf(TEXT("cannot read dungeon source during prohibition scan: %s"), *File));
                continue;
            }

            for (const FString& Token : Forbidden)
            {
                if (Text.Contains(Token, ESearchCase::CaseSensitive))
                {
                    Errors.Add(FString::Printf(TEXT("forbidden dungeon runtime token %s in %s"), *Token, *File));
                }
            }
        }
    }
}
}

int32 ValidateDungeonContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    const TArray<FString> RequiredFiles = {
        TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonCatalog.h"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonCatalog.cpp"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonCatalogLookup.cpp"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredMinorDungeonCatalog.h"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredMinorDungeonCatalog.cpp"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonProductionProfile.h"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonProductionProfile.cpp"),
        TEXT("Source/DarkArisen/ContentScale/DungeonProfiles_RexaFjord.cpp"),
        TEXT("Source/DarkArisen/ContentScale/DungeonProfiles_AshenSea.cpp"),
        TEXT("Source/DarkArisen/ContentScale/DungeonProfiles_ColonialRegion06.cpp"),
        TEXT("Source/DarkArisen/ContentScale/DungeonProfiles_Highmoore.cpp"),
        TEXT("Source/DarkArisen/Dungeons/AuthoredDungeonSiteComponent.h"),
        TEXT("Source/DarkArisen/Dungeons/AuthoredDungeonSiteComponent.cpp"),
        TEXT("Source/DarkArisen/Dungeons/SecretDungeonAccessRules.h"),
        TEXT("Source/DarkArisen/Dungeons/SecretDungeonAccessRules.cpp"),
        TEXT("Source/DarkArisen/Dungeons/CenoteFirstMotherComponent.h"),
        TEXT("Source/DarkArisen/Dungeons/DrownedMeadHallComponent.h"),
        TEXT("Source/DarkArisen/Dungeons/DrownedMeadHallComponent.cpp"),
        TEXT("Source/DarkArisen/Dungeons/ObsidianVaultComponent.h"),
        TEXT("Source/DarkArisen/Dungeons/ObsidianVaultComponent.cpp"),
        TEXT("Source/DarkArisen/Dungeons/HollowCityComponent.h"),
        TEXT("Source/DarkArisen/Dungeons/HollowCityComponent.cpp"),
        TEXT("Source/DarkArisen/Tests/DungeonCatalogSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/DungeonProductionProfilesSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/DungeonMvpSystemsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/SecretDungeonAccessSpec.cpp")
    };
    for (const FString& Relative : RequiredFiles)
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonCatalog.h"), {
        TEXT("RequiredNamedSiteCount = 41"),
        TEXT("RequiredMinorSiteCount = 20"),
        TEXT("RequiredGlobalDungeonCount = 61"),
        TEXT("BuildKnownNamedSites"),
        TEXT("BuildDesignGaps")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonCatalog.cpp"), {
        TEXT("Sites.Reserve(40)"),
        TEXT("dungeon.rexa.cenote-first-mother"),
        TEXT("dungeon.region06.fourth-fortress"),
        TEXT("dungeon.cross-network.fifth-connection"),
        TEXT("dungeon.highmoore.under-chamber-halford"),
        TEXT("design-gap.region06.second-tier-e"),
        TEXT("design-gap.crystal-caves-global-count"),
        TEXT("design-gap.minor-sites-authored-definitions")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredMinorDungeonCatalog.cpp"), {
        TEXT("RexaMoran, TEXT(\"rexa\"), 4"),
        TEXT("Fjordlund, TEXT(\"fjordlund\"), 3"),
        TEXT("Ashenmoor, TEXT(\"ashenmoor\"), 4"),
        TEXT("Sea, TEXT(\"sea\"), 3"),
        TEXT("Colonial, TEXT(\"colonial\"), 2"),
        TEXT("Region06, TEXT(\"region06\"), 1"),
        TEXT("Highmoore, TEXT(\"highmoore\"), 3"),
        TEXT("bGeneratedAtRuntime = false")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredDungeonProductionProfile.cpp"), {
        TEXT("Profiles.Reserve(40)"),
        TEXT("AppendRexaMoran"),
        TEXT("AppendFjordlund"),
        TEXT("AppendAshenmoor"),
        TEXT("AppendSea"),
        TEXT("AppendColonial"),
        TEXT("AppendRegion06AndCrossNetwork"),
        TEXT("AppendHighmoore"),
        TEXT("Grounded named dungeon lacks a production profile")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Dungeons/AuthoredDungeonSiteComponent.cpp"), {
        TEXT("TryGetKnownSite"),
        TEXT("TryGetProfile"),
        TEXT("CachedProductionProfile.bImageWithheld"),
        TEXT("CachedProductionProfile.bBossWithheldOrUnresolved"),
        TEXT("CachedProductionProfile.bRewardExplicitlyNone"),
        TEXT("CachedProductionProfile.bRewardWithheldOrUnresolved")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Dungeons/SecretDungeonAccessRules.cpp"), {
        TEXT("dungeon.rexa.grove-beneath-grove"),
        TEXT("dungeon.fjordlund.one-that-waited"),
        TEXT("dungeon.ashenmoor.thing-wyrm-left"),
        TEXT("dungeon.cross-network.fifth-connection"),
        TEXT("dungeon.region06.fourth-fortress"),
        TEXT("dungeon.sea.harbour-that-was-first"),
        TEXT("ELiberationNetwork::DeedJudgers"),
        TEXT("Secret site is absent on the Empire path")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Dungeons/DrownedMeadHallComponent.h"), {
        TEXT("DryWindowDurationWorldMinutes = 40"),
        TEXT("DryWindowSeparationWorldMinutes = 12 * 60"),
        TEXT("AuthoredFirstDryWindowStartMinute = -1")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Dungeons/ObsidianVaultComponent.h"), {
        TEXT("CanonicalVisibleJakeReflections = 100"),
        TEXT("CanonicalWrongMovingReflections = 1")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Dungeons/HollowCityComponent.h"), {
        TEXT("RequiredSubAreaCount = 4"),
        TEXT("RequiredSubAreaMask = 0x0F")}, Errors);

    ScanDungeonSourceForForbiddenRuntime(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Authored dungeon validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored dungeon source validation passed."));
    return 0;
}
}
