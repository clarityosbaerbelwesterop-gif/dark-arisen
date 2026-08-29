#include "DarkArisenOps.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative))) Errors.Add(FString::Printf(TEXT("missing world-content file: %s"), *Relative));
}

void RequireFragments(const FString& Root, const FString& Relative, std::initializer_list<const TCHAR*> Fragments, TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read world-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments) if (!Text.Contains(Fragment, ESearchCase::CaseSensitive)) Errors.Add(FString::Printf(TEXT("%s missing world contract: %s"), *Relative, Fragment));
}

void ScanWorldSourceForForbiddenGeneration(const FString& Root, TArray<FString>& Errors)
{
    const FString ScanRoot = FPaths::Combine(Root, TEXT("Source/DarkArisen/World"));
    const TArray<FString> Forbidden = {TEXT("FMath::Rand"), TEXT("FRandomStream"), TEXT("GenerateRandomPopulation"), TEXT("GenerateRandomWildlife"), TEXT("SpawnGenericWildlife"), TEXT("ProceduralQuestNPC"), TEXT("TeleportToWaypoint(")};
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);
    for (const FString& File : Files)
    {
        FString Text;
        if (!ReadText(File, Text)) { Errors.Add(FString::Printf(TEXT("cannot read world source during generation scan: %s"), *File)); continue; }
        for (const FString& Token : Forbidden) if (Text.Contains(Token, ESearchCase::CaseSensitive)) Errors.Add(FString::Printf(TEXT("forbidden world-generation token %s in %s"), *Token, *File));
    }
}
}

int32 ValidateWorldContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;
    for (const FString& Relative : {
        TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"), TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"),
        TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.h"), TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"),
        TEXT("Source/DarkArisen/World/PopulationProductionCatalog.h"), TEXT("Source/DarkArisen/World/PopulationProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/World/FaunaProductionCatalog.h"), TEXT("Source/DarkArisen/World/FaunaProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/World/AuthoredTravelProductionCatalog.h"), TEXT("Source/DarkArisen/World/AuthoredTravelProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/World/HighmooreWorldProductionCatalog.h"), TEXT("Source/DarkArisen/World/HighmooreWorldProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp"), TEXT("Source/DarkArisen/Tests/PopulationFaunaProductionSpec.cpp"), TEXT("Source/DarkArisen/Tests/TravelWorldProductionSpec.cpp")}) RequireFile(Root, Relative, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"), {TEXT("RequiredRegionCount = 8"), TEXT("bRuntimeMapAuthored = false"), TEXT("RuntimeMapPackage")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"), {TEXT("region.rexa-moran"), TEXT("region.fjordlund"), TEXT("region.ashenmoor"), TEXT("region.quiet-coast"), TEXT("region.pale-isle-silvera"), TEXT("region.at-sea"), TEXT("region.highmoore"), TEXT("region.06"), TEXT("design-gap.region-level-assets"), TEXT("may not claim a runtime map until a reviewed .umap exists")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"), {TEXT("FAuthoredWorldRegionRegistry::TryGet"), TEXT("RuntimeMapPackage.TrimStartAndEnd().IsEmpty()"), TEXT("LoadedRegion.bSourceRegistryKnown")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/PopulationProductionCatalog.h"), {TEXT("RequiredRegionalProfiles = 8"), TEXT("HighmooreClassCount = 4"), TEXT("MinimumAmbientLinesPerMajorRegion = 200"), TEXT("CrowdReturnMinimumMinutes = 2"), TEXT("CrowdReturnMaximumMinutes = 5"), TEXT("AllowsRandomAuthoredPopulationGeneration() { return false; }"), TEXT("AllowsHighmooreReputationMeter() { return false; }")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/PopulationProductionCatalog.cpp"), {TEXT("Major Imperial City"), TEXT("80, 150, 20, 50"), TEXT("population.rexa-moran"), TEXT("population.fjordlund"), TEXT("population.ashenmoor"), TEXT("population.quiet-coast"), TEXT("population.pirate-havens"), TEXT("population.highmoore"), TEXT("bDemographicPercentagesUnspecified = true"), TEXT("population.region-06"), TEXT("design-gap.population.highmoore-percentages"), TEXT("design-gap.population.runtime-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/FaunaProductionCatalog.h"), {TEXT("LandAnimalSpeciesCount = 42"), TEXT("BirdSpeciesCount = 52"), TEXT("SeaSpeciesCount = 65"), TEXT("SeaLegendaryWithinSpeciesCount = 2"), TEXT("SmallCreatureTypeCount = 45"), TEXT("PlantSpeciesMinimum = 60"), TEXT("HighmooreMvpCoreSpeciesCount = 5"), TEXT("RequiredRegionalEcologyProfiles = 8"), TEXT("AllowsRandomEncounterGeneration() { return false; }"), TEXT("AllowsHorseIntoStoppedLightShaft() { return false; }")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/FaunaProductionCatalog.cpp"), {TEXT("fauna.region.rexa-moran"), TEXT("fauna.region.fjordlund"), TEXT("fauna.region.ashenmoor"), TEXT("fauna.region.pale-isle"), TEXT("fauna.region.quiet-coast"), TEXT("fauna.region.at-sea"), TEXT("fauna.region.highmoore"), TEXT("fauna.region.06"), TEXT("fauna.patriarch"), TEXT("fauna.keeper-below"), TEXT("fauna.canopy-jaguar"), TEXT("design-gap.fauna.identity-import"), TEXT("design-gap.fauna.highmoore-complete-roster"), TEXT("design-gap.fauna.runtime-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredTravelProductionCatalog.h"), {TEXT("RequiredCoreRouteCount = 6"), TEXT("FearedWaterRegionCount = 6"), TEXT("CrystalFirstPassageMinimumRealMinutes = 90"), TEXT("CrystalFirstPassageMaximumRealMinutes = 120"), TEXT("CrystalRepeatPassageMinimumRealMinutes = 4"), TEXT("CrystalRepeatPassageMaximumRealMinutes = 6"), TEXT("AllowsInstantWaterTravel() { return false; }"), TEXT("AllowsMapClickMovement() { return false; }"), TEXT("AllowsCompass() { return false; }"), TEXT("AllowsMiniMapDisplay() { return false; }"), TEXT("AllowsPlayerDot() { return false; }")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredTravelProductionCatalog.cpp"), {TEXT("travel.archipelago.physical-sea-passage"), TEXT("travel.archipelago.feared-waters-guide"), TEXT("travel.highmoore.crystal-caves.first"), TEXT("travel.highmoore.crystal-caves.repeat"), TEXT("travel.highmoore.stable-horse-relocation"), TEXT("travel.settlement.walk-it"), TEXT("design-gap.travel.sea-route-duration-distance-table"), TEXT("design-gap.travel.runtime-world-links")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/HighmooreWorldProductionCatalog.h"), {TEXT("RequiredNamedAnchorCount = 12"), TEXT("MinimumVisibleCastlesFromEmergence = 6"), TEXT("EmergenceSwitchbackMinimumMinutesOnFoot = 8"), TEXT("EmergenceSwitchbackMaximumMinutesOnFoot = 10"), TEXT("ApproximateEndToEndCanterMinutes = 25"), TEXT("AllowsQuestionMarkDiscoveryMarkers() { return false; }"), TEXT("AllowsGenericCastleSubstitution() { return false; }")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/HighmooreWorldProductionCatalog.cpp"), {TEXT("world.highmoore.castle.arion"), TEXT("world.highmoore.castle.belos"), TEXT("world.highmoore.village.ashby-under-moor"), TEXT("world.highmoore.town.coldbeck"), TEXT("Coldbeck"), TEXT("world.highmoore.town.dunford"), TEXT("world.highmoore.town.wynnes-cross"), TEXT("world.highmoore.abbey.halford-cross"), TEXT("world.highmoore.pub.drovers-rest"), TEXT("design-gap.highmoore.anchor-coordinates"), TEXT("design-gap.highmoore.runtime-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp"), {TEXT("Exactly eight production/world regions are registered"), TEXT("Source does not falsely claim reviewed region .umap exists")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/PopulationFaunaProductionSpec.cpp"), {TEXT("Eight source-backed population profiles are registered"), TEXT("Highmoore demographic percentages remain explicitly unspecified"), TEXT("Population production requires engine-level child protection"), TEXT("Land animal corpus remains 42 species"), TEXT("Sea corpus remains 65 species"), TEXT("Highmoore MVP ecology keeps five core species"), TEXT("Horse never enters a stopped Highmoore light shaft"), TEXT("Fauna never gains random encounter generation"), TEXT("The Patriarch remains a named ecological anchor")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/TravelWorldProductionSpec.cpp"), {TEXT("Water fast travel remains impossible"), TEXT("Mini-map remains absent"), TEXT("First Crystal passage minimum is ninety real minutes"), TEXT("Stable relocation never moves Jake"), TEXT("Twelve source-backed Highmoore named anchors are registered"), TEXT("At least six castles remain visible from the emergence shelf"), TEXT("Coldbeck display identity is exact")}, Errors);

    ScanWorldSourceForForbiddenGeneration(Root, Errors);
    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Authored world-content validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors) UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Authored world/travel-content source validation passed."));
    return 0;
}
}
