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
        Errors.Add(FString::Printf(TEXT("missing world-content file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read world-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing world contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanWorldSourceForForbiddenGeneration(const FString& Root, TArray<FString>& Errors)
{
    const FString ScanRoot = FPaths::Combine(Root, TEXT("Source/DarkArisen/World"));
    const TArray<FString> Forbidden = {
        TEXT("FMath::Rand"),
        TEXT("FRandomStream"),
        TEXT("GenerateRandomPopulation"),
        TEXT("GenerateRandomWildlife"),
        TEXT("SpawnGenericWildlife"),
        TEXT("ProceduralQuestNPC")
    };

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);
    for (const FString& File : Files)
    {
        FString Text;
        if (!ReadText(File, Text))
        {
            Errors.Add(FString::Printf(TEXT("cannot read world source during generation scan: %s"), *File));
            continue;
        }
        for (const FString& Token : Forbidden)
        {
            if (Text.Contains(Token, ESearchCase::CaseSensitive))
            {
                Errors.Add(FString::Printf(TEXT("forbidden world-generation token %s in %s"), *Token, *File));
            }
        }
    }
}
}

int32 ValidateWorldContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"),
        TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"),
        TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.h"),
        TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"),
        TEXT("Source/DarkArisen/World/PopulationProductionCatalog.h"),
        TEXT("Source/DarkArisen/World/PopulationProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/World/FaunaProductionCatalog.h"),
        TEXT("Source/DarkArisen/World/FaunaProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp"),
        TEXT("Source/DarkArisen/Tests/PopulationFaunaProductionSpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"), {
        TEXT("RequiredRegionCount = 8"),
        TEXT("bRuntimeMapAuthored = false"),
        TEXT("RuntimeMapPackage")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"), {
        TEXT("region.rexa-moran"),
        TEXT("region.fjordlund"),
        TEXT("region.ashenmoor"),
        TEXT("region.quiet-coast"),
        TEXT("region.pale-isle-silvera"),
        TEXT("region.at-sea"),
        TEXT("region.highmoore"),
        TEXT("region.06"),
        TEXT("design-gap.region-level-assets"),
        TEXT("may not claim a runtime map until a reviewed .umap exists")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"), {
        TEXT("FAuthoredWorldRegionRegistry::TryGet"),
        TEXT("RuntimeMapPackage.TrimStartAndEnd().IsEmpty()"),
        TEXT("LoadedRegion.bSourceRegistryKnown")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/PopulationProductionCatalog.h"), {
        TEXT("RequiredRegionalProfiles = 7"),
        TEXT("MinimumAmbientLinesPerMajorRegion = 200"),
        TEXT("CrowdReturnMinimumMinutes = 2"),
        TEXT("CrowdReturnMaximumMinutes = 5"),
        TEXT("AllowsRandomAuthoredPopulationGeneration() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/PopulationProductionCatalog.cpp"), {
        TEXT("Major Imperial City"), TEXT("80, 150, 20, 50"),
        TEXT("population.rexa-moran"),
        TEXT("population.fjordlund"),
        TEXT("population.ashenmoor"),
        TEXT("population.quiet-coast"),
        TEXT("population.pirate-havens"),
        TEXT("population.region-06"),
        TEXT("design-gap.population.highmoore"),
        TEXT("design-gap.population.runtime-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/FaunaProductionCatalog.h"), {
        TEXT("LandAnimalSpeciesCount = 42"),
        TEXT("BirdSpeciesCount = 52"),
        TEXT("SeaSpeciesCount = 65"),
        TEXT("SmallCreatureTypeCount = 45"),
        TEXT("PlantSpeciesMinimum = 60"),
        TEXT("AllowsRandomEncounterGeneration() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/FaunaProductionCatalog.cpp"), {
        TEXT("fauna.region.rexa-moran"),
        TEXT("fauna.region.fjordlund"),
        TEXT("fauna.region.ashenmoor"),
        TEXT("fauna.region.pale-isle"),
        TEXT("fauna.region.quiet-coast"),
        TEXT("fauna.region.at-sea"),
        TEXT("fauna.region.06"),
        TEXT("fauna.patriarch"),
        TEXT("fauna.keeper-below"),
        TEXT("fauna.canopy-jaguar"),
        TEXT("design-gap.fauna.identity-import"),
        TEXT("design-gap.fauna.runtime-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp"), {
        TEXT("Exactly eight production/world regions are registered"),
        TEXT("Source does not falsely claim reviewed region .umap exists")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/PopulationFaunaProductionSpec.cpp"), {
        TEXT("Seven source-backed population profiles are registered"),
        TEXT("Population production requires engine-level child protection"),
        TEXT("Land animal corpus remains 42 species"),
        TEXT("Sea corpus remains 65 species"),
        TEXT("Fauna never gains random encounter generation"),
        TEXT("The Patriarch remains a named ecological anchor")}, Errors);

    ScanWorldSourceForForbiddenGeneration(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Authored world-content validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored world-content source validation passed."));
    return 0;
}
}
