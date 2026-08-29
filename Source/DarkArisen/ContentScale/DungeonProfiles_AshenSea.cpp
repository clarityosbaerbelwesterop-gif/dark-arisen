// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredDungeonProductionProfile.h"

namespace
{
FAuthoredDungeonProductionProfile Profile(
    const TCHAR* Id,
    const TCHAR* Source,
    const TCHAR* Access,
    const TCHAR* Puzzle,
    const TCHAR* Hazard,
    const TCHAR* Image,
    const TCHAR* Boss,
    const TCHAR* Reward)
{
    FAuthoredDungeonProductionProfile P;
    P.StableId = FName(Id);
    P.GoverningSource = Source;
    P.AccessAndDiscovery = Access;
    P.PuzzleLanguageDetail = Puzzle;
    P.HazardDetail = Hazard;
    P.UnforgettableImage = Image;
    P.BossOrBottomDetail = Boss;
    P.RewardDetail = Reward;
    return P;
}
}

void FAuthoredDungeonProductionProfiles::AppendAshenmoor(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    Out.Add(Profile(
        TEXT("dungeon.ashenmoor.obsidian-vault"),
        TEXT("regional dungeon catalog.md 4.1"),
        TEXT("Environmental fissure in a lava field."),
        TEXT("Ashenmoor heat-channel routing."),
        TEXT("Heat."),
        TEXT("Polished volcanic glass produces one hundred reflected Jakes and exactly one reflection moves wrong."),
        TEXT("The Reflection; no explanatory reveal is authored."),
        TEXT("Legendary obsidian required by Pak Setyo.")));

    Out.Add(Profile(
        TEXT("dungeon.ashenmoor.ash-buried-street"),
        TEXT("regional dungeon catalog.md 4.2"),
        TEXT("Environmental/documentary discovery of a town buried by the 1741 eruption."),
        TEXT("Ashenmoor ash-fall reading."),
        TEXT("Collapse and gas."),
        TEXT("A market is preserved mid-transaction exactly where people stood when the ash arrived; positioned, never posed."),
        TEXT("The Thing That Dug Down, fauna that has fed here for more than a century."),
        TEXT("The town's forgotten name; Mbah Seruni remembers it.")));

    Out.Add(Profile(
        TEXT("dungeon.ashenmoor.singing-caverns"),
        TEXT("regional dungeon catalog.md 4.3"),
        TEXT("Environmental discovery; heard before it is seen."),
        TEXT("Ashenmoor acoustic/star hybrid; the sustained chord changes with player position and functions as the map."),
        TEXT("Heat and disorientation."),
        TEXT("Wind through the cavern produces a sustained changing chord; the puzzle is the music and the music is the map."),
        TEXT("The Silence, defined by the chord stopping."),
        TEXT("A surviving Ashenmoor star-map that points onward to secret content.")));

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.ashenmoor.vent-shrine"),
            TEXT("regional dungeon catalog.md 4.4"),
            TEXT("Relational discovery at Mountain Paths Trust Tier 3."),
            TEXT("Ashenmoor shadow-hour logic."),
            TEXT("Heat and gas."),
            TEXT("A shrine is built directly over a volcanic vent and the whole structure breathes warm air on a cycle."),
            TEXT("War-reactive bottom state: no boss by default; an Imperial detachment occupies it if Jake supported Fuerte San Rafael."),
            TEXT("The shrine may be one of the sites on Cruz's list; the source deliberately leaves that identification uncertain."));
        P.bWarReactive = true;
        Out.Add(P);
    }

    Out.Add(Profile(
        TEXT("dungeon.ashenmoor.mountains-own"),
        TEXT("regional dungeon catalog.md 4.5"),
        TEXT("Relational discovery at Mountain Paths Trust Tier 4."),
        TEXT("Full Ashenmoor star/heat/ash/shadow grammar."),
        TEXT("Heat, gas and lava-flow sequence across a Tier-C complex."),
        TEXT("At the deepest accessible point, rock is warm and moving slightly; the mountain is alive in a literal, unexplained way."),
        TEXT("The Ash-Walker and The Deep Heat."),
        TEXT("Material evidence of the Ashen Wyrm's ancient passage.")));

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.ashenmoor.ash-bastion"),
            TEXT("regional dungeon catalog.md 4.6; castle catalog.md 12.3"),
            TEXT("Old Fortress reclamation rather than a normal dungeon clear."),
            TEXT("Sealed fortress-depth content exists; exact puzzle sequence remains unauthored here."),
            TEXT("Exact lower-level primary hazard remains unauthored in the cited catalog."),
            TEXT(""),
            TEXT(""),
            TEXT("Successful reclamation resolves into an Alliance Bastion."));
        P.bImageWithheld = true;
        P.bBossWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.ashenmoor.thing-wyrm-left"),
            TEXT("secret bosses.md 5"),
            TEXT("Mountain Paths Blood trust -> Singing Caverns star-map -> high volcanic route at night -> 1846 patrol reports returned to Mbah Seruni rather than used."),
            TEXT("The route and encounter use Ashenmoor star/heat knowledge; no unrelated lock is introduced."),
            TEXT("High volcanic heat and exposed mountain conditions."),
            TEXT("A chamber wall is scoured smooth in the exact shape of something enormous lying down; this is a rest, not a nest or lair."),
            TEXT("The Ashen Calf; killing it permanently changes the Ashen Wyrm's later behaviour."),
            TEXT("Combat: legendary volcanic material and a harder future Wyrm encounter. Non-combat: wait three real in-game days, leading to Mbah Seruni's long-awaited conversation."));
        P.bSupportsExplicitNonCombatResolution = true;
        Out.Add(P);
    }
}

void FAuthoredDungeonProductionProfiles::AppendSea(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    Out.Add(Profile(
        TEXT("dungeon.sea.reef-of-nails"),
        TEXT("regional dungeon catalog.md 5.1"),
        TEXT("Environmental discovery of a ship graveyard on a reef."),
        TEXT("Colonial/wreck mechanism logic."),
        TEXT("Water and breath."),
        TEXT("Forty ships from two centuries are stacked across the reef; the newest is an eleven-year-old Imperial wreck."),
        TEXT("The Reef Itself, an environmental boss encounter."),
        TEXT("Salvage plus a ship log naming a survivor who should not have survived.")));

    Out.Add(Profile(
        TEXT("dungeon.sea.ghost-hull"),
        TEXT("regional dungeon catalog.md 5.2"),
        TEXT("Environmental discovery of an upright, adrift, crewless hull."),
        TEXT("Colonial acoustic reading through the ship's sound-carrying structure."),
        TEXT("Structural danger and darkness."),
        TEXT("The ship is intact and provisioned, and the galley fire is still warm."),
        TEXT("One of the rare survivor encounters; it is not a boss fight until the authored encounter turns into one."),
        TEXT("A Crimson log that becomes a lead into the larger Armada thread.")));

    Out.Add(Profile(
        TEXT("dungeon.sea.sunken-chapel"),
        TEXT("regional dungeon catalog.md 5.3"),
        TEXT("Cartographic discovery."),
        TEXT("Colonial cathedral acoustics applied underwater."),
        TEXT("Water and breath timing."),
        TEXT("A fully submerged colonial chapel remains intact and its bell still rings when the current moves it; sailors above have heard it for a century."),
        TEXT("The Congregation."),
        TEXT("The site connects to Father Salvio's authored perspective rather than a generic loot payout.")));

    Out.Add(Profile(
        TEXT("dungeon.sea.deep-harbour"),
        TEXT("regional dungeon catalog.md 5.4"),
        TEXT("Relational + cartographic discovery after feared waters are opened."),
        TEXT("Hybrid Rexan water logic and Colonial mechanism."),
        TEXT("Water, pressure and darkness across a Tier-C complex."),
        TEXT("A complete pre-colonial harbour lies submerged with quays, warehouses, moorings and ships still tied to them."),
        TEXT("The Harbourmaster and What Came In With The Tide."),
        TEXT("The archipelago's oldest navigational knowledge; its charts open two more feared-water regions.")));

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.sea.harbour-that-was-first"),
            TEXT("secret bosses.md 8"),
            TEXT("Recovery Underground Blood trust -> Deep Harbour charts -> deepest feared waters -> sufficient cumulative sacred-item returns rather than fencing."),
            TEXT("Submerged-site navigation and observation; the source does not add a separate abstract puzzle."),
            TEXT("Deep water, pressure and underwater traversal."),
            TEXT("Dozens of small ocean-going boats sit on a beach that has been underwater for four thousand years, positioned as if their crews just arrived."),
            TEXT("The Landing; it is non-hostile until Jake attempts to take a boat."),
            TEXT("Combat: a pre-tradition vessel component. Non-combat: document and leave the boats, producing physical evidence against colonial terra-nullius claims."));
        P.bSupportsExplicitNonCombatResolution = true;
        Out.Add(P);
    }
}
