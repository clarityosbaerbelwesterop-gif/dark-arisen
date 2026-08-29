// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/FaunaProductionCatalog.h"

namespace
{
FFaunaCorpusContract Corpus(
    const EFaunaCorpusFamily Family,
    const TCHAR* DisplayName,
    const int32 BaseCount,
    const int32 LegendaryCount,
    const bool bMinimum,
    const bool bLegendaryIncludedInBase,
    const TCHAR* Source)
{
    FFaunaCorpusContract Result;
    Result.Family = Family;
    Result.DisplayName = DisplayName;
    Result.RequiredBaseIdentityCount = BaseCount;
    Result.RequiredLegendaryCount = LegendaryCount;
    Result.bCountIsMinimum = bMinimum;
    Result.bLegendaryCountIncludedInBase = bLegendaryIncludedInBase;
    Result.GoverningSource = Source;
    return Result;
}

FRegionalFaunaIdentity Region(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* AudibleIdentity,
    const TCHAR* EcologicalRule,
    const bool bSilence = false)
{
    FRegionalFaunaIdentity Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.AudibleIdentity = AudibleIdentity;
    Result.EcologicalProductionRule = EcologicalRule;
    Result.bSilenceIsIntentional = bSilence;
    return Result;
}

FNamedFaunaProductionAnchor Anchor(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* ProductionLaw)
{
    FNamedFaunaProductionAnchor Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.ProductionLaw = ProductionLaw;
    return Result;
}
}

TArray<FFaunaCorpusContract> FFaunaProductionCatalog::BuildCorpusContracts()
{
    return {
        Corpus(EFaunaCorpusFamily::LandAnimals, TEXT("Land Animals"),
            LandAnimalSpeciesCount, LandLegendaryVariantCount, false, false,
            TEXT("docs/design/fauna/land_animals.md Section 1")),
        Corpus(EFaunaCorpusFamily::Birds, TEXT("Birds"),
            BirdSpeciesCount, BirdLegendaryCount, false, false,
            TEXT("docs/design/fauna/birds.md Section 1")),
        Corpus(EFaunaCorpusFamily::SeaAnimals, TEXT("Sea Animals"),
            SeaSpeciesCount, SeaLegendaryWithinSpeciesCount, false, true,
            TEXT("docs/design/fauna/sea_animals.md Section 1 — 65 total includes one legendary whale and one legendary shark")),
        Corpus(EFaunaCorpusFamily::SmallCreatures, TEXT("Insects & Small Creatures"),
            SmallCreatureTypeCount, SmallCreatureLegendaryCount, false, false,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 1")),
        Corpus(EFaunaCorpusFamily::PlantsAndHerbs, TEXT("Plants & Herbs"),
            PlantSpeciesMinimum, PlantLegendaryCount, true, false,
            TEXT("docs/design/fauna/plants_and_herbs.md Section 1"))
    };
}

TArray<FRegionalFaunaIdentity> FFaunaProductionCatalog::BuildRegionalIdentities()
{
    return {
        Region(
            TEXT("fauna.region.rexa-moran"), TEXT("Rexa / Moran"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md Section 3; sea_animals.md Section 1"),
            TEXT("Dense dawn chorus: howler calls, macaw/parrot screeches, continuous tropical chirping and dusk overlap."),
            TEXT("Tropical jungle/reef ecology must read as dense and layered; sacred/taboo species consequences remain culture-specific, never generic loot flags.")),
        Region(
            TEXT("fauna.region.fjordlund"), TEXT("Fjordlund"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md Section 4; sea_animals.md Section 1"),
            TEXT("Gulls over water, puffins on cliffs, raven calls inland, loons on lakes and authored deep-winter silence."),
            TEXT("Cold-water fish, whales, seals and arctic land species must feel season-bound rather than tropical fauna with swapped materials."),
            true),
        Region(
            TEXT("fauna.region.ashenmoor"), TEXT("Ashenmoor"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md; sea_animals.md Section 1"),
            TEXT("Vultures mostly silent, brief dawn sparrows and night owls; wind dominates the soundscape."),
            TEXT("Sparse volcanic/desert ecology is intentional; low density is not a missing-spawn bug and water/heat adaptation must govern placement."),
            true),
        Region(
            TEXT("fauna.region.pale-isle"), TEXT("Pale Isle"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md Section 1; sea_animals.md Section 1"),
            TEXT("Managed garden songbirds, pheasants and trained Imperial falcons create an ordered soundscape."),
            TEXT("Managed/regulated wildlife and fisheries must visibly differ from frontier or indigenous ecosystems.")),
        Region(
            TEXT("fauna.region.quiet-coast"), TEXT("Quiet Coast"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md Section 1; sea_animals.md Section 1"),
            TEXT("Familiar temperate dawn chorus with hawks, owls and wood warblers; deliberately home-like."),
            TEXT("Temperate game/fishing grounds should read familiar and practical rather than exoticised.")),
        Region(
            TEXT("fauna.region.at-sea"), TEXT("Open Sea / Deep Ocean"),
            TEXT("docs/design/fauna/sea_animals.md Sections 1-2; docs/design/fauna/birds.md Section 1"),
            TEXT("Long ocean passages use sparse seabird presence, including the lonely albatross motif, rather than constant crowd noise."),
            TEXT("Open-ocean large species and deep-water monster-tier creatures belong to authored waters/weather/depth contexts, never arbitrary encounter rolls.")),
        Region(
            TEXT("fauna.region.highmoore"), TEXT("Highmoore"),
            TEXT("highmoore fauna.md; highmoore region.md Sections 2.3 and 6.1"),
            TEXT("Sparse moorland sound: grouse flushes are deliberate sudden punctuation; open distance and the stopped-shaft absence are as important as animal calls."),
            TEXT("Five ecological zones remain distinct. MVP production must include grouse, hare, red deer, fox and fell wolf; hunting rights are political, horses may bolt from wolves, and nothing lives on the stopped-light bare ground."),
            true),
        Region(
            TEXT("fauna.region.06"), TEXT("Region 06"),
            TEXT("docs/design/fauna/birds.md Section 2.1; land_animals.md Section 1; sea_animals.md Section 1"),
            TEXT("Calls are wrong or absent: unknown song-language and silence where birds should be are part of the horror."),
            TEXT("The region's three unique land species and impossible sea life must remain authored mysteries; normal-region ecology must not be copied in as filler."),
            true)
    };
}

TArray<FNamedFaunaProductionAnchor> FFaunaProductionCatalog::BuildNamedAnchors()
{
    return {
        Anchor(
            TEXT("fauna.patriarch"), TEXT("The Patriarch"),
            TEXT("docs/design/fauna/land_animals.md Section 3.4"),
            TEXT("Legendary jaguar with repeated stalking encounters, chapter-6-plus gating, ecosystem consequence on death and culturally divided reactions; not a generic oversized jaguar spawn.")),
        Anchor(
            TEXT("fauna.keeper-below"), TEXT("The Keeper Below"),
            TEXT("regional dungeon catalog.md Section 2.1; docs/design/fauna/land_animals.md Caiman"),
            TEXT("Ancient caiman inhabiting the Cenote of the First Mother; it lives there rather than guarding loot, and the encounter remains tied to the water/drowning dungeon ecology.")),
        Anchor(
            TEXT("fauna.canopy-jaguar"), TEXT("The Canopy Jaguar"),
            TEXT("regional dungeon catalog.md Section 2.3; docs/design/fauna/land_animals.md Jaguar"),
            TEXT("Root Temple vertical predator from the Patriarch's line; authored vertical fight identity must not be flattened into a normal jaguar encounter."))
    };
}

TArray<FFaunaProductionDesignGap> FFaunaProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.fauna.identity-import"),
            TEXT("The Phase-4 documents contain hundreds of individually written species/plant entries, but the repository does not yet contain a reconciled machine-readable identity import for every one of them. Counts are locked; identities must be transcribed source-by-source rather than synthesized."),
            TEXT("docs/design/fauna/land_animals.md; birds.md; sea_animals.md; insects_and_small_creatures.md; plants_and_herbs.md")
        },
        {
            TEXT("design-gap.fauna.highmoore-complete-roster"),
            TEXT("Highmoore's later fauna master defines its five zones, MVP species and Full Vision interactions but does not publish a closed numeric species roster comparable to the Phase-4 archipelago summary. Do not invent a total."),
            TEXT("highmoore fauna.md Sections 1, 4, 5 and 9")
        },
        {
            TEXT("design-gap.fauna.runtime-assets"),
            TEXT("No complete reviewed Unreal creature/plant asset set, animation set, AI behavior package, audio set or authored placement map is evidenced for the full fauna corpus."),
            TEXT("Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md; repository asset evidence")
        }
    };
}

bool FFaunaProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FFaunaCorpusContract> Corpora = BuildCorpusContracts();
    if (Corpora.Num() != 5)
    {
        OutErrors.Add(TEXT("Fauna/flora production requires exactly five corpus families."));
    }

    TSet<uint8> Families;
    for (const FFaunaCorpusContract& CorpusEntry : Corpora)
    {
        if (CorpusEntry.DisplayName.IsEmpty() || CorpusEntry.RequiredBaseIdentityCount <= 0
            || CorpusEntry.RequiredLegendaryCount < 0 || CorpusEntry.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every fauna corpus family requires a valid count contract and governing source."));
        }
        const uint8 FamilyKey = static_cast<uint8>(CorpusEntry.Family);
        if (Families.Contains(FamilyKey))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate fauna corpus family: %d"), static_cast<int32>(FamilyKey)));
        }
        Families.Add(FamilyKey);
    }

    const FFaunaCorpusContract* Sea = Corpora.FindByPredicate([](const FFaunaCorpusContract& Entry)
    {
        return Entry.Family == EFaunaCorpusFamily::SeaAnimals;
    });
    if (!Sea || Sea->RequiredLegendaryCount != SeaLegendaryWithinSpeciesCount || !Sea->bLegendaryCountIncludedInBase)
    {
        OutErrors.Add(TEXT("Sea corpus must preserve the two legendary identities inside the authored 65-species total."));
    }

    if (LandAnimalSpeciesCount != 42 || LandLegendaryVariantCount != 6
        || BirdSpeciesCount != 52 || BirdLegendaryCount != 4
        || SeaSpeciesCount != 65 || SeaLegendaryWithinSpeciesCount != 2
        || SmallCreatureTypeCount != 45 || SmallCreatureLegendaryCount != 3
        || PlantSpeciesMinimum != 60 || PlantLegendaryCount != 4
        || HighmooreMvpCoreSpeciesCount != 5)
    {
        OutErrors.Add(TEXT("Fauna/flora corpus counts drifted from the Phase-4 / Highmoore source summaries."));
    }

    const TArray<FRegionalFaunaIdentity> Regional = BuildRegionalIdentities();
    if (Regional.Num() != RequiredRegionalEcologyProfiles)
    {
        OutErrors.Add(FString::Printf(TEXT("Fauna regional production requires exactly %d source-backed ecology profiles; found %d."), RequiredRegionalEcologyProfiles, Regional.Num()));
    }
    TSet<FName> RegionalIds;
    for (const FRegionalFaunaIdentity& Entry : Regional)
    {
        if (Entry.StableId.IsNone() || Entry.DisplayName.IsEmpty() || Entry.GoverningSource.IsEmpty()
            || Entry.AudibleIdentity.IsEmpty() || Entry.EcologicalProductionRule.IsEmpty())
        {
            OutErrors.Add(TEXT("Every regional fauna identity requires id, source, audible identity and ecology rule."));
        }
        if (RegionalIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate regional fauna identity: %s"), *Entry.StableId.ToString()));
        }
        RegionalIds.Add(Entry.StableId);
    }
    if (!RegionalIds.Contains(TEXT("fauna.region.highmoore")))
    {
        OutErrors.Add(TEXT("Highmoore's later narrow fauna authority must be represented separately from the Phase-4 archipelago counts."));
    }

    TSet<FName> AnchorIds;
    for (const FNamedFaunaProductionAnchor& Entry : BuildNamedAnchors())
    {
        if (Entry.StableId.IsNone() || Entry.DisplayName.IsEmpty() || Entry.GoverningSource.IsEmpty()
            || Entry.ProductionLaw.IsEmpty())
        {
            OutErrors.Add(TEXT("Every named fauna anchor requires identity, source and production law."));
        }
        if (AnchorIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate named fauna anchor: %s"), *Entry.StableId.ToString()));
        }
        AnchorIds.Add(Entry.StableId);
        if (Entry.bRuntimeCreatureAssetAuthored || !Entry.RuntimeCreatureAssetPath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Named fauna anchor %s may not claim an Unreal asset before reviewed asset evidence exists."), *Entry.StableId.ToString()));
        }
    }

    if (AllowsRandomEncounterGeneration() || AllowsGenericSpawnPointSubstitution() || AllowsHorseIntoStoppedLightShaft())
    {
        OutErrors.Add(TEXT("Fauna production may not replace authored ecology with random/generic encounters or allow the Highmoore horse into a stopped light shaft."));
    }
    if (!RequiresCulturalConsequenceWhereAuthored() || !RequiresEcologicalPersistenceWhereAuthored())
    {
        OutErrors.Add(TEXT("Fauna production must preserve authored cultural and ecological consequences."));
    }
    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Fauna production must keep the three current identity/Highmoore/runtime gaps explicit."));
    }

    return OutErrors.IsEmpty();
}
