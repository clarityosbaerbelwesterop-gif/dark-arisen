// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "World/FaunaProductionCatalog.h"
#include "World/PopulationProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenPopulationFaunaProductionSpec,
    "DarkArisen.Content.World.PopulationFaunaProduction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenPopulationFaunaProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> PopulationErrors;
    TestTrue(TEXT("Population production catalog validates"),
        FPopulationProductionCatalog::Validate(PopulationErrors));
    TestEqual(TEXT("Population catalog has no structural errors"), PopulationErrors.Num(), 0);

    const TArray<FPopulationDensityContract> Density =
        FPopulationProductionCatalog::BuildDensityContracts();
    TestEqual(TEXT("Six density/absence archetypes remain explicit"), Density.Num(), 6);
    TestEqual(TEXT("Major Imperial market floor remains 80 visible NPCs"),
        Density[0].CentreVisibleMinimum, 80);
    TestEqual(TEXT("Major Imperial market ceiling remains 150 visible NPCs"),
        Density[0].CentreVisibleMaximum, 150);
    TestEqual(TEXT("Regional town center floor remains 30"),
        Density[1].CentreVisibleMinimum, 30);
    TestEqual(TEXT("Village floor remains 10"), Density[2].CentreVisibleMinimum, 10);
    TestEqual(TEXT("Frontier may intentionally show zero people"),
        Density[3].CentreVisibleMinimum, 0);
    TestEqual(TEXT("Pirate Haven floor remains 40"), Density[4].CentreVisibleMinimum, 40);

    const TArray<FRegionalPopulationProductionProfile> PopulationProfiles =
        FPopulationProductionCatalog::BuildRegionalProfiles();
    TestEqual(TEXT("Eight source-backed population profiles are registered"),
        PopulationProfiles.Num(), FPopulationProductionCatalog::RequiredRegionalProfiles);
    const FRegionalPopulationProductionProfile* HighmoorePopulation = PopulationProfiles.FindByPredicate(
        [](const FRegionalPopulationProductionProfile& Entry)
        {
            return Entry.StableId == FName(TEXT("population.highmoore"));
        });
    TestNotNull(TEXT("Highmoore fifth-register population profile exists"), HighmoorePopulation);
    if (HighmoorePopulation)
    {
        TestTrue(TEXT("Highmoore demographic percentages remain explicitly unspecified"),
            HighmoorePopulation->bDemographicPercentagesUnspecified);
        TestEqual(TEXT("Highmoore does not invent a demographic percentage table"),
            HighmoorePopulation->Demographics.Num(), 0);
    }
    TestEqual(TEXT("Highmoore preserves four authored social classes"),
        FPopulationProductionCatalog::HighmooreClassCount, 4);
    TestFalse(TEXT("Highmoore never gains a reputation meter"),
        FPopulationProductionCatalog::AllowsHighmooreReputationMeter());
    TestFalse(TEXT("Authored population content never gains a random generation path"),
        FPopulationProductionCatalog::AllowsRandomAuthoredPopulationGeneration());
    TestTrue(TEXT("Crowds use purpose-driven pathing"),
        FPopulationProductionCatalog::RequiresPurposeDrivenPathing());
    TestTrue(TEXT("Population production requires engine-level child protection"),
        FPopulationProductionCatalog::RequiresChildEngineProtection());
    TestEqual(TEXT("Major-region ambient dialogue minimum remains 200 lines"),
        FPopulationProductionCatalog::MinimumAmbientLinesPerMajorRegion, 200);
    TestEqual(TEXT("Crowd calm return minimum remains two minutes"),
        FPopulationProductionCatalog::CrowdReturnMinimumMinutes, 2);
    TestEqual(TEXT("Crowd calm return maximum remains five minutes"),
        FPopulationProductionCatalog::CrowdReturnMaximumMinutes, 5);

    TArray<FString> FaunaErrors;
    TestTrue(TEXT("Fauna production catalog validates"),
        FFaunaProductionCatalog::Validate(FaunaErrors));
    TestEqual(TEXT("Fauna catalog has no structural errors"), FaunaErrors.Num(), 0);

    TestEqual(TEXT("Land animal corpus remains 42 species"),
        FFaunaProductionCatalog::LandAnimalSpeciesCount, 42);
    TestEqual(TEXT("Bird corpus remains 52 species"),
        FFaunaProductionCatalog::BirdSpeciesCount, 52);
    TestEqual(TEXT("Sea corpus remains 65 species"),
        FFaunaProductionCatalog::SeaSpeciesCount, 65);
    TestEqual(TEXT("Sea corpus keeps two legendary identities inside the 65"),
        FFaunaProductionCatalog::SeaLegendaryWithinSpeciesCount, 2);
    TestEqual(TEXT("Small-creature corpus remains 45 types"),
        FFaunaProductionCatalog::SmallCreatureTypeCount, 45);
    TestEqual(TEXT("Plant/herb corpus remains at least 60 species"),
        FFaunaProductionCatalog::PlantSpeciesMinimum, 60);
    TestEqual(TEXT("Highmoore MVP ecology keeps five core species"),
        FFaunaProductionCatalog::HighmooreMvpCoreSpeciesCount, 5);

    const TArray<FRegionalFaunaIdentity> Ecology = FFaunaProductionCatalog::BuildRegionalIdentities();
    TestEqual(TEXT("Eight regional ecology profiles include Highmoore"),
        Ecology.Num(), FFaunaProductionCatalog::RequiredRegionalEcologyProfiles);
    TestTrue(TEXT("Highmoore ecology exists as its own later narrow authority"),
        Ecology.ContainsByPredicate([](const FRegionalFaunaIdentity& Entry)
        {
            return Entry.StableId == FName(TEXT("fauna.region.highmoore"));
        }));

    TestFalse(TEXT("Fauna never gains random encounter generation"),
        FFaunaProductionCatalog::AllowsRandomEncounterGeneration());
    TestFalse(TEXT("Generic spawn points cannot substitute authored ecology"),
        FFaunaProductionCatalog::AllowsGenericSpawnPointSubstitution());
    TestFalse(TEXT("Horse never enters a stopped Highmoore light shaft"),
        FFaunaProductionCatalog::AllowsHorseIntoStoppedLightShaft());

    const TArray<FNamedFaunaProductionAnchor> Anchors =
        FFaunaProductionCatalog::BuildNamedAnchors();
    TSet<FName> AnchorIds;
    for (const FNamedFaunaProductionAnchor& Anchor : Anchors)
    {
        AnchorIds.Add(Anchor.StableId);
        TestFalse(TEXT("No fauna anchor falsely claims a reviewed creature asset"),
            Anchor.bRuntimeCreatureAssetAuthored);
        TestTrue(TEXT("Absent fauna asset path remains empty"),
            Anchor.RuntimeCreatureAssetPath.IsEmpty());
    }
    TestTrue(TEXT("The Patriarch remains a named ecological anchor"),
        AnchorIds.Contains(TEXT("fauna.patriarch")));
    TestTrue(TEXT("The Keeper Below remains bound to Cenote ecology"),
        AnchorIds.Contains(TEXT("fauna.keeper-below")));
    TestTrue(TEXT("The Canopy Jaguar remains a distinct vertical encounter anchor"),
        AnchorIds.Contains(TEXT("fauna.canopy-jaguar")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
