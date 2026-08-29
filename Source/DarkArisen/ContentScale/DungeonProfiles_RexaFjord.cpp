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

void FAuthoredDungeonProductionProfiles::AppendRexaMoran(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    Out.Add(Profile(
        TEXT("dungeon.rexa.cenote-first-mother"),
        TEXT("regional dungeon catalog.md 2.1; dungeon design philosophy.md; indigenous liberation alliance.md 2.2"),
        TEXT("Environmental sinkhole discovery; Mama Jacinta network knowledge is withheld until Trust Tier 3."),
        TEXT("Rexan water-routing logic."),
        TEXT("Water and drowning."),
        TEXT("Flooded chamber turns green-gold when the sinkhole sunlight reaches it for the authored one-hour world window."),
        TEXT("The Keeper Below, an ancient caiman that simply lives in the chamber."),
        TEXT("Centuries of offerings whose ownership matters; the site is not a generic loot chest.")));

    Out.Add(Profile(
        TEXT("dungeon.rexa.green-throat"),
        TEXT("regional dungeon catalog.md 2.2"),
        TEXT("Environmental cave mouth behind a waterfall."),
        TEXT("Rexan root-circuit logic."),
        TEXT("Vegetation and rot."),
        TEXT("A forty-year-old colonial survey camp remains in place while the jungle has grown through it, including a tree through a tent."),
        TEXT("The Survey; short remnant encounter, deliberately unexplained."),
        TEXT("Survey maps that point to two other sites, with one map being wrong in an informative way.")));

    Out.Add(Profile(
        TEXT("dungeon.rexa.root-temple-ixchel"),
        TEXT("regional dungeon catalog.md 2.3"),
        TEXT("Cartographic discovery; requires Rexan map-language literacy."),
        TEXT("Rexan flowering-sequence logic and vine traversal."),
        TEXT("Vegetation and vertical falls."),
        TEXT("A pyramid and a single strangler fig have become one structural object; traversal passes inside the tree inside the temple."),
        TEXT("The Canopy Jaguar, fought vertically."),
        TEXT("A ceremonial piece that Doña Carmen Ruiz would refuse to sell.")));

    Out.Add(Profile(
        TEXT("dungeon.rexa.drowned-plantation"),
        TEXT("regional dungeon catalog.md 2.4"),
        TEXT("Environmental/documentary discovery of a failed 1780s Imperial plantation flooded by a changed river."),
        TEXT("Colonial clockwork through functioning sluice mechanisms."),
        TEXT("Water and structural danger."),
        TEXT("The great-house ballroom is waist-deep, chandelier still hanging, with fish moving through the room."),
        TEXT("The Overseer's Dogs, descendants of the plantation dogs gone feral."),
        TEXT("Plantation ledgers older than Nueva Esperanza's that identify families whose descendants remain in the cane.")));

    Out.Add(Profile(
        TEXT("dungeon.rexa.hollow-city"),
        TEXT("regional dungeon catalog.md 2.5; dungeon system global.md 7.1"),
        TEXT("Relational discovery at Cultural Web Trust Tier 3; Mama Jacinta tells Jake it exists but does not accompany him."),
        TEXT("Escalating Rexan water, root and flowering grammar across four sub-areas."),
        TEXT("Vegetation, water and darkness in authored sequence across a Tier-C complex."),
        TEXT("Eleven square kilometres of pre-colonial city exist under a canopy so complete the Empire has never found it despite the city lying thirty miles from Puerto de la Corona."),
        TEXT("The Watcher at the Gate and The Thing in the Reservoir."),
        TEXT("The city's own records plus an ancient map that opens two Tier-E sites.")));

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.rexa.la-guardia-vieja"),
            TEXT("regional dungeon catalog.md 2.6; castle catalog.md 12.3"),
            TEXT("Alliance reclamation of the Old Fortress rather than a normal dungeon clear."),
            TEXT("Sealed lower levels are dungeon content; exact lower-level puzzle sequence remains unauthored in the cited catalog."),
            TEXT("Exact lower-level primary hazard remains unauthored in the cited catalog."),
            TEXT(""),
            TEXT(""),
            TEXT("Successful reclamation resolves the fortress into Alliance Bastion ownership rather than an abandoned cleared site."));
        P.bImageWithheld = true;
        P.bBossWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.rexa.grove-beneath-grove"),
            TEXT("secret bosses.md 3"),
            TEXT("Cultural Web Blood trust -> Rexan map -> deep-canopy route -> ceremonial artifact previously returned rather than sold."),
            TEXT("The cavern and living root system are the spatial puzzle; no generic lock or marker is introduced."),
            TEXT("Living root/cavern environment; the room itself becomes the combat space if Jake attacks."),
            TEXT("Roots from the Sacred Grove above descend in deliberate arrangements, revealing that something below has cultivated the grove for generations."),
            TEXT("The First Gardener continues tending and does not stop for Jake; combat begins only if provoked."),
            TEXT("Combat: legendary material for Bu Wira/Pak Setyo. Non-combat: Jake leaves everything and a third Mama Jacinta ceremony becomes the reward."));
        P.bSupportsExplicitNonCombatResolution = true;
        Out.Add(P);
    }
}

void FAuthoredDungeonProductionProfiles::AppendFjordlund(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    Out.Add(Profile(
        TEXT("dungeon.fjordlund.whale-bone-barrow"),
        TEXT("regional dungeon catalog.md 3.1"),
        TEXT("Environmental discovery of a headland mound that is not natural."),
        TEXT("Fjordlund saga-sequence logic."),
        TEXT("Cold and darkness."),
        TEXT("The burial chamber roof is the ribcage of a whale and has stood for six hundred years."),
        TEXT("The Barrow-Keeper, a buried remnant still keeping what it was placed to keep."),
        TEXT("Grave goods whose taking has a later conversation with Mormor Astrid.")));

    Out.Add(Profile(
        TEXT("dungeon.fjordlund.drowned-mead-hall"),
        TEXT("regional dungeon catalog.md 3.2"),
        TEXT("Environmental low-tide discovery; the hall is physically accessible only through authored tide windows."),
        TEXT("Fjordlund tide-window logic."),
        TEXT("Water and tidal timing."),
        TEXT("The long hall drains twice a day and can be walked dry for exactly forty world minutes while the benches remain in their rows."),
        TEXT("The Last Guest; the fight weaponises the same tide cycle."),
        TEXT("The hall's drinking horn; Master Halvdan can put it on Bjornshallen's wall and tell the story correctly.")));

    Out.Add(Profile(
        TEXT("dungeon.fjordlund.ice-caves-svart-fjord"),
        TEXT("regional dungeon catalog.md 3.3"),
        TEXT("Environmental discovery."),
        TEXT("Fjordlund freeze/melt routing."),
        TEXT("Cold and ice traversal."),
        TEXT("Clear ice reveals a ship forty metres beneath the player's feet."),
        TEXT("The White Below; visually allowed to evoke but not actually be the Ghost Reindeer."),
        TEXT("Cold-forged material wanted by Halvor Eriksson and Astrid Halvorsdotter without a competition mechanic.")));

    Out.Add(Profile(
        TEXT("dungeon.fjordlund.stone-field-under-chamber"),
        TEXT("regional dungeon catalog.md 3.4; dungeon system global.md 6.1"),
        TEXT("Relational discovery at Patient Coast Trust Tier 3."),
        TEXT("Fjordlund rune-genealogy reading."),
        TEXT("Darkness and cold."),
        TEXT("The sea-taken names carved on the standing stones continue down the underground walls by the thousands."),
        TEXT("No boss. The names are the bottom state."),
        TEXT("If Bjorn is dead, his name is already present, without a marker or explanatory prompt.")));

    Out.Add(Profile(
        TEXT("dungeon.fjordlund.hall-under-glacier"),
        TEXT("regional dungeon catalog.md 3.5"),
        TEXT("Cartographic discovery through a Fjordlund saga-map told at Bjornshallen."),
        TEXT("Full Fjordlund saga/tide/ice grammar."),
        TEXT("Ice, cold and structural danger."),
        TEXT("An eight-hundred-year-old jarl's hall is preserved complete inside the glacier with everything and everyone still in place."),
        TEXT("The Hearth-Guard and The Jarl."),
        TEXT("The archipelago's oldest continuous cultural artifact; Jarl Olaf's people want it returned.")));

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.fjordlund.drowned-hold"),
            TEXT("regional dungeon catalog.md 3.6; castle catalog.md 12.3"),
            TEXT("Old Fortress reclamation rather than a normal clear."),
            TEXT("Sealed dungeon content exists below the fortress; exact puzzle sequence remains unauthored here."),
            TEXT("Exact lower-level primary hazard remains unauthored in the cited catalog."),
            TEXT(""),
            TEXT(""),
            TEXT("Reclamation resolves into an Alliance Bastion rather than a disposable cleared dungeon."));
        P.bImageWithheld = true;
        P.bBossWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.fjordlund.one-that-waited"),
            TEXT("secret bosses.md 4"),
            TEXT("Patient Coast Blood trust -> correctly told saga -> winter northern-water route -> Drowned Mead Hall horn returned to Halvdan."),
            TEXT("Tide/ice spatial logic from the cave and ship; no unrelated puzzle layer is added."),
            TEXT("Tide and ice."),
            TEXT("A perfectly preserved moored ship is older than remembered Fjordlund construction, with joints Halvor cannot identify."),
            TEXT("The Steersman at the tiller; it defends the ship only if the encounter becomes combat."),
            TEXT("Combat: the finest single ship fitting. Non-combat: Jake leaves something he genuinely values; Mormor Astrid later listens to what he saw."));
        P.bSupportsExplicitNonCombatResolution = true;
        Out.Add(P);
    }
}
