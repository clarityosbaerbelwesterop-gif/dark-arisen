// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ContentScale/AuthoredDungeonCatalog.h"
#include "ContentScale/AuthoredMinorDungeonCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAuthoredDungeonCatalogSpec,
    "DarkArisen.Content.Dungeons.AuthoredCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAuthoredDungeonCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Grounded dungeon catalog validates without inventing missing slots"),
        FAuthoredDungeonCatalog::ValidateKnownCatalog(Errors));
    TestEqual(TEXT("Grounded dungeon catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FAuthoredDungeonCatalogEntry> Sites = FAuthoredDungeonCatalog::BuildKnownNamedSites();
    TestEqual(TEXT("Exactly forty regional/cross-network named sites are unambiguously grounded today"), Sites.Num(), 40);

    int32 SecretSiteCount = 0;
    int32 NoBossCarveoutCount = 0;
    int32 TierConflictCount = 0;
    TSet<FName> SeenIds;

    for (const FAuthoredDungeonCatalogEntry& Entry : Sites)
    {
        TestFalse(TEXT("Stable IDs remain unique"), SeenIds.Contains(Entry.StableId));
        SeenIds.Add(Entry.StableId);

        TestTrue(TEXT("Every named Tier-B+ site carries the inside-opened return contract"), Entry.bReturnShortcutFromInside);
        TestFalse(TEXT("No dungeon marker"), Entry.bHasMapMarker);
        TestFalse(TEXT("No ambient dungeon score"), Entry.bHasAmbientDungeonMusic);
        TestFalse(TEXT("No child remains"), Entry.bContainsChildRemains);

        SecretSiteCount += Entry.bSecretSite ? 1 : 0;
        NoBossCarveoutCount += Entry.BossState == EDungeonCatalogBossState::NoBoss ? 1 : 0;
        TierConflictCount += Entry.bTierClassificationConflict ? 1 : 0;
    }

    TestEqual(TEXT("All six trust-gated secret sites are represented"), SecretSiteCount, 6);
    TestEqual(TEXT("Exactly four authored sites have no boss"), NoBossCarveoutCount, 4);
    TestEqual(TEXT("Only Fourth Fortress carries an unresolved tier conflict"), TierConflictCount, 1);

    TestTrue(TEXT("Stone Field no-boss carve-out is present"), SeenIds.Contains(TEXT("dungeon.fjordlund.stone-field-under-chamber")));
    TestTrue(TEXT("Long Barrow no-boss carve-out is present"), SeenIds.Contains(TEXT("dungeon.highmoore.long-barrow-braehead")));
    TestTrue(TEXT("First House no-boss/no-reward carve-out is present"), SeenIds.Contains(TEXT("dungeon.region06.first-house")));
    TestTrue(TEXT("Fifth Connection no-fight carve-out is present"), SeenIds.Contains(TEXT("dungeon.cross-network.fifth-connection")));

    const FAuthoredDungeonCatalogEntry Crystal = FAuthoredDungeonCatalog::BuildCrystalCavesCarveout();
    TestEqual(TEXT("Crystal Caves remains a separate category-of-one"), Crystal.Tier, EContentDungeonTier::CrystalCaves);
    TestEqual(TEXT("Crystal Caves authored ceiling remains two hours"), Crystal.ExactMaximumMinutes, 120.0f);
    TestFalse(TEXT("Crystal Caves is not folded into the forty known regional named sites"), SeenIds.Contains(Crystal.StableId));

    TArray<FString> MinorErrors;
    TestTrue(TEXT("Twenty canonical Tier-A/minor slots validate"),
        FAuthoredMinorDungeonCatalog::ValidateSlots(MinorErrors));
    TestEqual(TEXT("Minor slot catalog has no structural errors"), MinorErrors.Num(), 0);

    const TArray<FAuthoredMinorDungeonSlot> MinorSlots = FAuthoredMinorDungeonCatalog::BuildSlots();
    TestEqual(TEXT("Global Tier-A/minor count is exactly twenty"), MinorSlots.Num(), 20);

    TMap<EDungeonCatalogRegion, int32> MinorCounts;
    TSet<FName> MinorIds;
    for (const FAuthoredMinorDungeonSlot& Slot : MinorSlots)
    {
        TestFalse(TEXT("Minor slot IDs remain unique"), MinorIds.Contains(Slot.StableId));
        MinorIds.Add(Slot.StableId);
        MinorCounts.FindOrAdd(Slot.Region)++;

        TestFalse(TEXT("Unnamed minor identities remain explicitly unauthored"), Slot.bIdentityAuthored);
        TestFalse(TEXT("Minor placements remain explicitly unauthored"), Slot.bPlacementAuthored);
        TestFalse(TEXT("Minor sites are never runtime-generated/radiant"), Slot.bGeneratedAtRuntime);
    }

    TestEqual(TEXT("Rexa/Moran minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::RexaMoran), 4);
    TestEqual(TEXT("Fjordlund minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Fjordlund), 3);
    TestEqual(TEXT("Ashenmoor minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Ashenmoor), 4);
    TestEqual(TEXT("Sea minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Sea), 3);
    TestEqual(TEXT("Colonial minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Colonial), 2);
    TestEqual(TEXT("Region 06 minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Region06), 1);
    TestEqual(TEXT("Highmoore minor quota"), MinorCounts.FindRef(EDungeonCatalogRegion::Highmoore), 3);

    const TArray<FDungeonCatalogDesignGap> Gaps = FAuthoredDungeonCatalog::BuildDesignGaps();
    TestEqual(TEXT("Three explicit catalog-level design gaps remain"), Gaps.Num(), 3);

    TSet<FName> GapIds;
    for (const FDungeonCatalogDesignGap& Gap : Gaps)
    {
        GapIds.Add(Gap.StableId);
    }
    TestTrue(TEXT("Missing Region 06 Tier-E definition remains explicit"), GapIds.Contains(TEXT("design-gap.region06.second-tier-e")));
    TestTrue(TEXT("Crystal/global-count conflict remains explicit"), GapIds.Contains(TEXT("design-gap.crystal-caves-global-count")));
    TestTrue(TEXT("Minor-site identity/placement authoring remains explicit"), GapIds.Contains(TEXT("design-gap.minor-sites-authored-definitions")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
