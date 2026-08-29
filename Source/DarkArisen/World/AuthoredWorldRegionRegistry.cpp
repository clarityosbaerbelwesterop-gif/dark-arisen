// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/AuthoredWorldRegionRegistry.h"

namespace
{
FAuthoredWorldRegionDefinition Region(
    const EAuthoredWorldRegion RegionType,
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const bool bSettlements,
    const bool bDungeons,
    const bool bWar,
    const bool bNaval,
    const bool bHouse)
{
    FAuthoredWorldRegionDefinition Result;
    Result.Region = RegionType;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.bHasSettlements = bSettlements;
    Result.bHasDungeonContent = bDungeons;
    Result.bHasWarContent = bWar;
    Result.bHasNavalContent = bNaval;
    Result.bHasHouseContent = bHouse;
    return Result;
}
}

TArray<FAuthoredWorldRegionDefinition> FAuthoredWorldRegionRegistry::BuildAll()
{
    return {
        Region(
            EAuthoredWorldRegion::RexaMoran,
            TEXT("region.rexa-moran"), TEXT("Rexa & Moran"),
            TEXT("docs/design/region_01_moran.md; docs/design/region_02_rexa_overview.md; regional dungeon catalog.md"),
            true, true, true, true, false),
        Region(
            EAuthoredWorldRegion::Fjordlund,
            TEXT("region.fjordlund"), TEXT("Fjordlund"),
            TEXT("regional dungeon catalog.md Section 3; regional populations.md; indigenous liberation alliance.md Section 2.5"),
            true, true, true, true, false),
        Region(
            EAuthoredWorldRegion::Ashenmoor,
            TEXT("region.ashenmoor"), TEXT("Ashenmoor"),
            TEXT("docs/design/region_03_ashenmoor_overview.md; regional dungeon catalog.md Section 4"),
            true, true, true, false, false),
        Region(
            EAuthoredWorldRegion::QuietCoast,
            TEXT("region.quiet-coast"), TEXT("The Quiet Coast"),
            TEXT("docs/design/region_04_quiet_coast_overview.md; regional populations.md"),
            true, false, true, true, false),
        Region(
            EAuthoredWorldRegion::PaleIsleSilvera,
            TEXT("region.pale-isle-silvera"), TEXT("Pale Isle & Silvera"),
            TEXT("side quest catalog.md Section 4.1; imperial_colonies.md; krone_colonies.md; regional populations.md"),
            true, false, true, true, false),
        Region(
            EAuthoredWorldRegion::OpenSea,
            TEXT("region.at-sea"), TEXT("The Open Sea"),
            TEXT("docs/design/region_05_open_sea_overview.md; regional dungeon catalog.md Section 5; naval combat system.md"),
            false, true, true, true, false),
        Region(
            EAuthoredWorldRegion::Highmoore,
            TEXT("region.highmoore"), TEXT("Highmoore"),
            TEXT("highmoore overview.md; highmoore region.md; side quest catalog.md Section 2.3"),
            true, true, false, false, true),
        Region(
            EAuthoredWorldRegion::Region06,
            TEXT("region.06"), TEXT("Region 06 / The Maw"),
            TEXT("docs/design/region_06_the_maw.md; regional dungeon catalog.md Section 7"),
            false, true, false, false, false)
    };
}

bool FAuthoredWorldRegionRegistry::TryGet(
    const FName StableId,
    FAuthoredWorldRegionDefinition& OutDefinition)
{
    if (StableId.IsNone())
    {
        return false;
    }
    for (const FAuthoredWorldRegionDefinition& Definition : BuildAll())
    {
        if (Definition.StableId == StableId)
        {
            OutDefinition = Definition;
            return true;
        }
    }
    return false;
}

TArray<FWorldRegionDesignGap> FAuthoredWorldRegionRegistry::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.region-level-assets"),
            TEXT("The repository currently lacks reviewed production .umap assets for the eight authored world/quest regions. Source descriptors must not claim those assets exist."),
            TEXT("Docs/REPOSITORY_FINDINGS.md; Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md P4")
        },
        {
            TEXT("design-gap.fjordlund-single-master"),
            TEXT("Fjordlund is strongly authored across dungeon, population, liberation and NPC documents but has no single standalone region-master file in the current source register; production should cite the narrower sources until one is authored."),
            TEXT("Docs/DesignSourceRegister.md; regional dungeon catalog.md Section 3; regional populations.md")
        },
        {
            TEXT("design-gap.pale-isle-silvera-region-master"),
            TEXT("Pale Isle & Silvera are a locked Turn distribution bucket and colonial-world location family, but current source does not provide one unambiguous standalone region-master document for Alpha level production."),
            TEXT("side quest catalog.md Section 4.1; imperial_colonies.md; krone_colonies.md")
        }
    };
}

bool FAuthoredWorldRegionRegistry::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FAuthoredWorldRegionDefinition> Definitions = BuildAll();
    if (Definitions.Num() != RequiredRegionCount)
    {
        OutErrors.Add(FString::Printf(TEXT("World region registry requires exactly %d production regions; found %d."), RequiredRegionCount, Definitions.Num()));
    }

    TSet<FName> Seen;
    for (const FAuthoredWorldRegionDefinition& Definition : Definitions)
    {
        if (Definition.StableId.IsNone() || Definition.DisplayName.IsEmpty() || Definition.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every world region requires stable id, authored display name and governing source."));
        }
        if (Seen.Contains(Definition.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate world region stable id: %s"), *Definition.StableId.ToString()));
        }
        Seen.Add(Definition.StableId);

        if (Definition.bRuntimeMapAuthored || !Definition.RuntimeMapPackage.IsEmpty())
        {
            OutErrors.Add(FString::Printf(
                TEXT("Region %s may not claim a runtime map until a reviewed .umap exists."),
                *Definition.StableId.ToString()));
        }
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("World production must preserve the current three region/asset design gaps."));
    }

    return OutErrors.IsEmpty();
}
