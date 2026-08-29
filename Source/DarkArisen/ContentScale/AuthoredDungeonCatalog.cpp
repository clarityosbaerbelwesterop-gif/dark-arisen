// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredDungeonCatalog.h"

namespace
{
FAuthoredDungeonCatalogEntry MakeSite(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EDungeonCatalogRegion Region,
    const EContentDungeonTier Tier,
    const TCHAR* GoverningSource,
    const EDungeonCatalogDiscovery Discovery,
    const EDungeonCatalogPuzzleLanguage PuzzleLanguage,
    const EDungeonCatalogBossState BossState,
    const TCHAR* BossOrEncounterName = TEXT(""))
{
    FAuthoredDungeonCatalogEntry Entry;
    Entry.StableId = FName(StableId);
    Entry.DisplayName = DisplayName;
    Entry.Region = Region;
    Entry.Tier = Tier;
    Entry.GoverningSource = GoverningSource;
    Entry.Discovery = Discovery;
    Entry.PuzzleLanguage = PuzzleLanguage;
    Entry.BossState = BossState;
    Entry.BossOrEncounterName = BossOrEncounterName;
    Entry.bReturnShortcutFromInside = true;
    return Entry;
}

void RequireUniqueId(
    const FName StableId,
    TSet<FName>& Seen,
    TArray<FString>& OutErrors)
{
    if (StableId.IsNone())
    {
        OutErrors.Add(TEXT("Authored dungeon entry has no stable ID."));
        return;
    }
    if (Seen.Contains(StableId))
    {
        OutErrors.Add(FString::Printf(TEXT("Duplicate authored dungeon stable ID: %s"), *StableId.ToString()));
        return;
    }
    Seen.Add(StableId);
}
}

TArray<FAuthoredDungeonCatalogEntry> FAuthoredDungeonCatalog::BuildKnownNamedSites()
{
    TArray<FAuthoredDungeonCatalogEntry> Sites;
    Sites.Reserve(40);

    // Rexa & Moran — regional dungeon catalog Section 2 + secret bosses Section 3.
    Sites.Add(MakeSite(
        TEXT("dungeon.rexa.cenote-first-mother"), TEXT("The Cenote of the First Mother"),
        EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 2.1"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Rexan,
        EDungeonCatalogBossState::Authored, TEXT("The Keeper Below")));
    Sites.Add(MakeSite(
        TEXT("dungeon.rexa.green-throat"), TEXT("La Garganta Verde — The Green Throat"),
        EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 2.2"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Rexan,
        EDungeonCatalogBossState::Authored, TEXT("The Survey")));
    Sites.Add(MakeSite(
        TEXT("dungeon.rexa.root-temple-ixchel"), TEXT("The Root Temple of Ix'Chel"),
        EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 2.3"),
        EDungeonCatalogDiscovery::Cartographic, EDungeonCatalogPuzzleLanguage::Rexan,
        EDungeonCatalogBossState::Authored, TEXT("The Canopy Jaguar")));
    Sites.Add(MakeSite(
        TEXT("dungeon.rexa.drowned-plantation"), TEXT("The Drowned Plantation"),
        EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 2.4"),
        EDungeonCatalogDiscovery::Hybrid, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::Authored, TEXT("The Overseer's Dogs")));
    Sites.Add(MakeSite(
        TEXT("dungeon.rexa.hollow-city"), TEXT("The Hollow City"),
        EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierC,
        TEXT("regional dungeon catalog.md Section 2.5"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Rexan,
        EDungeonCatalogBossState::MultipleAuthored, TEXT("The Watcher at the Gate; The Thing in the Reservoir")));
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.rexa.la-guardia-vieja"), TEXT("La Guardia Vieja"),
            EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierD,
            TEXT("regional dungeon catalog.md Section 2.6; castle catalog.md Section 12.3"),
            EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::Rexan,
            EDungeonCatalogBossState::Withheld);
        Entry.bOldFortress = true;
        Sites.Add(Entry);
    }
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.rexa.grove-beneath-grove"), TEXT("The Grove Beneath the Grove"),
            EDungeonCatalogRegion::RexaMoran, EContentDungeonTier::TierE,
            TEXT("secret bosses.md Section 3"),
            EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Rexan,
            EDungeonCatalogBossState::Authored, TEXT("The First Gardener"));
        Entry.bSecretSite = true;
        Sites.Add(Entry);
    }

    // Fjordlund — regional dungeon catalog Section 3 + secret bosses Section 4.
    Sites.Add(MakeSite(
        TEXT("dungeon.fjordlund.whale-bone-barrow"), TEXT("The Whale-Bone Barrow"),
        EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 3.1"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Fjordlund,
        EDungeonCatalogBossState::Authored, TEXT("The Barrow-Keeper")));
    Sites.Add(MakeSite(
        TEXT("dungeon.fjordlund.drowned-mead-hall"), TEXT("The Drowned Mead Hall"),
        EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 3.2"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Fjordlund,
        EDungeonCatalogBossState::Authored, TEXT("The Last Guest")));
    Sites.Add(MakeSite(
        TEXT("dungeon.fjordlund.ice-caves-svart-fjord"), TEXT("The Ice Caves of Svart Fjord"),
        EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 3.3"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Fjordlund,
        EDungeonCatalogBossState::Authored, TEXT("The White Below")));
    Sites.Add(MakeSite(
        TEXT("dungeon.fjordlund.stone-field-under-chamber"), TEXT("The Stone Field's Under-Chamber"),
        EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 3.4; dungeon system global.md Section 6.1"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Fjordlund,
        EDungeonCatalogBossState::NoBoss));
    Sites.Add(MakeSite(
        TEXT("dungeon.fjordlund.hall-under-glacier"), TEXT("The Hall Under the Glacier"),
        EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierC,
        TEXT("regional dungeon catalog.md Section 3.5"),
        EDungeonCatalogDiscovery::Cartographic, EDungeonCatalogPuzzleLanguage::Fjordlund,
        EDungeonCatalogBossState::MultipleAuthored, TEXT("The Hearth-Guard; The Jarl")));
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.fjordlund.drowned-hold"), TEXT("The Drowned Hold"),
            EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierD,
            TEXT("regional dungeon catalog.md Section 3.6; castle catalog.md Section 12.3"),
            EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::Fjordlund,
            EDungeonCatalogBossState::Withheld);
        Entry.bOldFortress = true;
        Sites.Add(Entry);
    }
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.fjordlund.one-that-waited"), TEXT("The One That Waited"),
            EDungeonCatalogRegion::Fjordlund, EContentDungeonTier::TierE,
            TEXT("secret bosses.md Section 4"),
            EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Fjordlund,
            EDungeonCatalogBossState::Authored, TEXT("The Steersman"));
        Entry.bSecretSite = true;
        Sites.Add(Entry);
    }

    // Ashenmoor — regional dungeon catalog Section 4 + secret bosses Section 5.
    Sites.Add(MakeSite(
        TEXT("dungeon.ashenmoor.obsidian-vault"), TEXT("The Obsidian Vault"),
        EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 4.1"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Ashenmoor,
        EDungeonCatalogBossState::Authored, TEXT("The Reflection")));
    Sites.Add(MakeSite(
        TEXT("dungeon.ashenmoor.ash-buried-street"), TEXT("The Ash-Buried Street"),
        EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 4.2"),
        EDungeonCatalogDiscovery::Hybrid, EDungeonCatalogPuzzleLanguage::Ashenmoor,
        EDungeonCatalogBossState::Authored, TEXT("The Thing That Dug Down")));
    Sites.Add(MakeSite(
        TEXT("dungeon.ashenmoor.singing-caverns"), TEXT("The Singing Caverns"),
        EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 4.3"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Ashenmoor,
        EDungeonCatalogBossState::Authored, TEXT("The Silence")));
    Sites.Add(MakeSite(
        TEXT("dungeon.ashenmoor.vent-shrine"), TEXT("The Vent-Shrine"),
        EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 4.4"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Ashenmoor,
        EDungeonCatalogBossState::WarReactive));
    Sites.Add(MakeSite(
        TEXT("dungeon.ashenmoor.mountains-own"), TEXT("The Mountain's Own"),
        EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierC,
        TEXT("regional dungeon catalog.md Section 4.5"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Ashenmoor,
        EDungeonCatalogBossState::MultipleAuthored, TEXT("The Ash-Walker; The Deep Heat")));
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.ashenmoor.ash-bastion"), TEXT("The Ash Bastion"),
            EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierD,
            TEXT("regional dungeon catalog.md Section 4.6; castle catalog.md Section 12.3"),
            EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::Ashenmoor,
            EDungeonCatalogBossState::Withheld);
        Entry.bOldFortress = true;
        Sites.Add(Entry);
    }
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.ashenmoor.thing-wyrm-left"), TEXT("The Thing the Wyrm Left"),
            EDungeonCatalogRegion::Ashenmoor, EContentDungeonTier::TierE,
            TEXT("secret bosses.md Section 5"),
            EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Ashenmoor,
            EDungeonCatalogBossState::Authored, TEXT("The Ashen Calf"));
        Entry.bSecretSite = true;
        Sites.Add(Entry);
    }

    // The Sea — regional dungeon catalog Section 5 + secret bosses Section 8.
    Sites.Add(MakeSite(
        TEXT("dungeon.sea.reef-of-nails"), TEXT("The Reef of Nails"),
        EDungeonCatalogRegion::Sea, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 5.1"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::Environmental, TEXT("The Reef Itself")));
    Sites.Add(MakeSite(
        TEXT("dungeon.sea.ghost-hull"), TEXT("The Ghost Hull"),
        EDungeonCatalogRegion::Sea, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 5.2"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::SurvivorConditional));
    Sites.Add(MakeSite(
        TEXT("dungeon.sea.sunken-chapel"), TEXT("The Sunken Chapel"),
        EDungeonCatalogRegion::Sea, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 5.3"),
        EDungeonCatalogDiscovery::Cartographic, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::Authored, TEXT("The Congregation")));
    Sites.Add(MakeSite(
        TEXT("dungeon.sea.deep-harbour"), TEXT("The Deep Harbour"),
        EDungeonCatalogRegion::Sea, EContentDungeonTier::TierC,
        TEXT("regional dungeon catalog.md Section 5.4"),
        EDungeonCatalogDiscovery::Hybrid, EDungeonCatalogPuzzleLanguage::Hybrid,
        EDungeonCatalogBossState::MultipleAuthored, TEXT("The Harbourmaster; What Came In With The Tide")));
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.sea.harbour-that-was-first"), TEXT("The Harbour That Was First"),
            EDungeonCatalogRegion::Sea, EContentDungeonTier::TierE,
            TEXT("secret bosses.md Section 8"),
            EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Hybrid,
            EDungeonCatalogBossState::Authored, TEXT("The Landing"));
        Entry.bSecretSite = true;
        Sites.Add(Entry);
    }

    // Colonial — regional dungeon catalog Section 6.
    Sites.Add(MakeSite(
        TEXT("dungeon.colonial.valdena-mine"), TEXT("The Valdeña Mine"),
        EDungeonCatalogRegion::Colonial, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 6.1"),
        EDungeonCatalogDiscovery::Hybrid, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::Authored, TEXT("The Shift")));
    Sites.Add(MakeSite(
        TEXT("dungeon.colonial.siege-tunnels-ciudadela"), TEXT("The Siege Tunnels of La Ciudadela"),
        EDungeonCatalogRegion::Colonial, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 6.2"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Hybrid,
        EDungeonCatalogBossState::Authored, TEXT("The Foundation")));
    Sites.Add(MakeSite(
        TEXT("dungeon.colonial.plague-vault"), TEXT("The Plague Vault"),
        EDungeonCatalogRegion::Colonial, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 6.3"),
        EDungeonCatalogDiscovery::Documentary, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::Authored, TEXT("The Sealed")));
    Sites.Add(MakeSite(
        TEXT("dungeon.colonial.company-works"), TEXT("The Company Works"),
        EDungeonCatalogRegion::Colonial, EContentDungeonTier::TierC,
        TEXT("regional dungeon catalog.md Section 6.4"),
        EDungeonCatalogDiscovery::Hybrid, EDungeonCatalogPuzzleLanguage::Colonial,
        EDungeonCatalogBossState::MultipleAuthored, TEXT("The Works; The Last Shift Supervisor")));

    // Region 06 + cross-network. One Region 06 Tier-E slot remains genuinely unauthored.
    Sites.Add(MakeSite(
        TEXT("dungeon.region06.first-house"), TEXT("The First House"),
        EDungeonCatalogRegion::Region06, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 7.2; dungeon system global.md Section 6.2"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::Region06Unreliable,
        EDungeonCatalogBossState::NoBoss));
    Sites.Add(MakeSite(
        TEXT("dungeon.region06.third-bell"), TEXT("The Third Bell"),
        EDungeonCatalogRegion::Region06, EContentDungeonTier::TierB,
        TEXT("regional dungeon catalog.md Section 7.3"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Region06Unreliable,
        EDungeonCatalogBossState::Unresolved));
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.region06.fourth-fortress"), TEXT("The Fourth Fortress"),
            EDungeonCatalogRegion::Region06, EContentDungeonTier::TierD,
            TEXT("regional dungeon catalog.md Section 7.4; secret bosses.md Section 7"),
            EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::Region06Unreliable,
            EDungeonCatalogBossState::Withheld, TEXT("WITHHELD"));
        Entry.bOldFortress = true;
        Entry.bSecretSite = true;
        Entry.bTierClassificationConflict = true;
        Sites.Add(Entry);
    }
    {
        FAuthoredDungeonCatalogEntry Entry = MakeSite(
            TEXT("dungeon.cross-network.fifth-connection"), TEXT("The Fifth Connection Site"),
            EDungeonCatalogRegion::CrossNetwork, EContentDungeonTier::TierE,
            TEXT("secret bosses.md Section 6; dungeon system global.md Section 6.1"),
            EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::Hybrid,
            EDungeonCatalogBossState::NoBoss, TEXT("The Agreement"));
        Entry.bSecretSite = true;
        Sites.Add(Entry);
    }

    // Highmoore — dungeon system global.md Section 3.2.
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.long-barrow-braehead"), TEXT("The Long Barrow at Braehead"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierB,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::Relational, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::NoBoss));
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.drowned-mill"), TEXT("The Drowned Mill"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierB,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::Authored, TEXT("The Miller")));
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.stone-row"), TEXT("The Stone Row"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierB,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::Withheld));
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.ashby-crossing"), TEXT("The Ashby Crossing"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierB,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::Authored, TEXT("The Third Dig")));
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.bowyers-wood"), TEXT("The Bowyer's Wood"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierB,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::Withheld));
    Sites.Add(MakeSite(
        TEXT("dungeon.highmoore.under-chamber-halford"), TEXT("The Under-Chamber at Halford"),
        EDungeonCatalogRegion::Highmoore, EContentDungeonTier::TierC,
        TEXT("dungeon system global.md Section 3.2"),
        EDungeonCatalogDiscovery::WithheldOrUnresolved, EDungeonCatalogPuzzleLanguage::HighmoorePractices,
        EDungeonCatalogBossState::Withheld));

    return Sites;
}

FAuthoredDungeonCatalogEntry FAuthoredDungeonCatalog::BuildCrystalCavesCarveout()
{
    FAuthoredDungeonCatalogEntry Entry = MakeSite(
        TEXT("passage.highmoore.crystal-caves"), TEXT("The Crystal Caves"),
        EDungeonCatalogRegion::CrystalPassage, EContentDungeonTier::CrystalCaves,
        TEXT("dungeon system global.md Section 4; crystal caves.md"),
        EDungeonCatalogDiscovery::Environmental, EDungeonCatalogPuzzleLanguage::WithheldOrUnresolved,
        EDungeonCatalogBossState::Authored, TEXT("The Crystal Guardian"));
    Entry.SecondarySource = TEXT("highmoore overview.md; crystal guardian.md");
    Entry.ExactMaximumMinutes = 120.0f;
    return Entry;
}

TArray<FDungeonCatalogDesignGap> FAuthoredDungeonCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.region06.second-tier-e"),
            TEXT("The regional/global tables require two Region 06 Tier-E slots. The corpus names the Fourth Fortress and the cross-network Fifth Connection Site, but does not unambiguously author a second distinct Region 06 Tier-E site without double-counting the Fourth Fortress."),
            TEXT("regional dungeon catalog.md Sections 7.4-7.5; secret bosses.md Sections 6-7; dungeon system global.md Section 2")
        },
        {
            TEXT("design-gap.crystal-caves-global-count"),
            TEXT("The global table locks 41 named + 20 minor = 61 while Section 4 calls Crystal Caves a separate category of one. The previous M7 implementation counted Crystal Caves inside the 41. This catalog keeps the carve-out separate until the count is explicitly reconciled."),
            TEXT("dungeon system global.md Sections 2, 4 and 7")
        },
        {
            TEXT("design-gap.minor-sites-authored-definitions"),
            TEXT("Twenty Tier-A minor sites are required globally but are intentionally unnamed in the archipelago catalog. AI/procedural filler is not permitted; authored placements and identities remain outstanding."),
            TEXT("dungeon system global.md Sections 2 and 7; regional dungeon catalog.md Section 1.2; Docs/AGENT_RULES.md Section 4")
        }
    };
}

bool FAuthoredDungeonCatalog::ValidateKnownCatalog(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredDungeonCatalogEntry> KnownSites = BuildKnownNamedSites();
    if (KnownSites.Num() != 40)
    {
        OutErrors.Add(FString::Printf(TEXT("Known grounded dungeon catalog must contain exactly 40 currently named regional/cross-network sites; found %d."), KnownSites.Num()));
    }

    TSet<FName> Seen;
    int32 TierConflictCount = 0;
    for (const FAuthoredDungeonCatalogEntry& Entry : KnownSites)
    {
        RequireUniqueId(Entry.StableId, Seen, OutErrors);
        if (Entry.DisplayName.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon %s has no authored display name."), *Entry.StableId.ToString()));
        }
        if (Entry.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon %s has no governing source."), *Entry.StableId.ToString()));
        }
        if (!Entry.bReturnShortcutFromInside)
        {
            OutErrors.Add(FString::Printf(TEXT("Named Tier-B+ site lacks the mandatory inside-opened return: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bHasMapMarker)
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon marker is forbidden: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bHasAmbientDungeonMusic)
        {
            OutErrors.Add(FString::Printf(TEXT("Ambient dungeon music is forbidden: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bContainsChildRemains)
        {
            OutErrors.Add(FString::Printf(TEXT("Child remains are forbidden in dungeon content: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.ExactMaximumMinutes > 90.0f)
        {
            OutErrors.Add(FString::Printf(TEXT("No ordinary dungeon may carry an exact maximum above 90 minutes: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bTierClassificationConflict)
        {
            ++TierConflictCount;
        }
    }

    if (TierConflictCount != 1 || !Seen.Contains(TEXT("dungeon.region06.fourth-fortress")))
    {
        OutErrors.Add(TEXT("The known catalog must preserve exactly the authored Fourth Fortress tier-classification conflict."));
    }

    const FAuthoredDungeonCatalogEntry Crystal = BuildCrystalCavesCarveout();
    RequireUniqueId(Crystal.StableId, Seen, OutErrors);
    if (Crystal.Tier != EContentDungeonTier::CrystalCaves)
    {
        OutErrors.Add(TEXT("Crystal Caves must remain the category-of-one carve-out."));
    }
    if (Crystal.ExactMaximumMinutes < 90.0f || Crystal.ExactMaximumMinutes > 120.0f)
    {
        OutErrors.Add(TEXT("Crystal Caves authored maximum must remain inside 90-120 minutes."));
    }
    if (Crystal.bHasMapMarker || Crystal.bHasAmbientDungeonMusic || Crystal.bContainsChildRemains)
    {
        OutErrors.Add(TEXT("Crystal Caves inherits the global marker/music/child-remains prohibitions."));
    }

    const TArray<FDungeonCatalogDesignGap> Gaps = BuildDesignGaps();
    TSet<FName> GapIds;
    for (const FDungeonCatalogDesignGap& Gap : Gaps)
    {
        RequireUniqueId(Gap.StableId, GapIds, OutErrors);
        if (Gap.Description.TrimStartAndEnd().IsEmpty() || Gap.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon design gap %s must document both the problem and its sources."), *Gap.StableId.ToString()));
        }
    }

    return OutErrors.IsEmpty();
}
