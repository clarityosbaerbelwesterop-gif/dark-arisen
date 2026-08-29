// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "World/AuthoredWorldRegionRegistry.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAuthoredWorldRegionRegistrySpec,
    "DarkArisen.Content.World.AuthoredRegionRegistry",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAuthoredWorldRegionRegistrySpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Authored world region registry validates"), FAuthoredWorldRegionRegistry::Validate(Errors));
    TestEqual(TEXT("World region registry has no structural errors"), Errors.Num(), 0);

    const TArray<FAuthoredWorldRegionDefinition> Regions = FAuthoredWorldRegionRegistry::BuildAll();
    TestEqual(TEXT("Exactly eight production/world regions are registered"), Regions.Num(), 8);

    TSet<FName> RegionIds;
    for (const FAuthoredWorldRegionDefinition& Region : Regions)
    {
        RegionIds.Add(Region.StableId);
        TestFalse(TEXT("Source does not falsely claim reviewed region .umap exists"), Region.bRuntimeMapAuthored);
        TestTrue(TEXT("Absent production map keeps package path empty"), Region.RuntimeMapPackage.IsEmpty());
    }

    TestTrue(TEXT("Rexa & Moran present"), RegionIds.Contains(TEXT("region.rexa-moran")));
    TestTrue(TEXT("Fjordlund present"), RegionIds.Contains(TEXT("region.fjordlund")));
    TestTrue(TEXT("Ashenmoor present"), RegionIds.Contains(TEXT("region.ashenmoor")));
    TestTrue(TEXT("Quiet Coast present"), RegionIds.Contains(TEXT("region.quiet-coast")));
    TestTrue(TEXT("Pale Isle & Silvera present"), RegionIds.Contains(TEXT("region.pale-isle-silvera")));
    TestTrue(TEXT("Open Sea present"), RegionIds.Contains(TEXT("region.at-sea")));
    TestTrue(TEXT("Highmoore present"), RegionIds.Contains(TEXT("region.highmoore")));
    TestTrue(TEXT("Region 06 present"), RegionIds.Contains(TEXT("region.06")));

    TestEqual(TEXT("Three current region-production gaps remain explicit"), FAuthoredWorldRegionRegistry::BuildDesignGaps().Num(), 3);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
