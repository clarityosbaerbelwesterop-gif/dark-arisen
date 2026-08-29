// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/PopulationProductionCatalog.h"

namespace
{
FPopulationDensityContract Density(
    const EPopulationDensityArchetype Archetype,
    const TCHAR* DisplayName,
    const int32 CentreMin,
    const int32 CentreMax,
    const int32 StreetMin,
    const int32 StreetMax)
{
    FPopulationDensityContract Result;
    Result.Archetype = Archetype;
    Result.DisplayName = DisplayName;
    Result.CentreVisibleMinimum = CentreMin;
    Result.CentreVisibleMaximum = CentreMax;
    Result.StreetVisibleMinimum = StreetMin;
    Result.StreetVisibleMaximum = StreetMax;
    Result.GoverningSource = TEXT("docs/design/npcs/regional_populations.md Section 2.2");
    return Result;
}

FPopulationDemographicShare Share(const TCHAR* Group, const int32 Percentage)
{
    FPopulationDemographicShare Result;
    Result.Group = Group;
    Result.Percentage = Percentage;
    return Result;
}

FRegionalPopulationProductionProfile Profile(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    std::initializer_list<FPopulationDemographicShare> Demographics,
    const TCHAR* DailyRhythm,
    const TCHAR* JakeReactionRule)
{
    FRegionalPopulationProductionProfile Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    for (const FPopulationDemographicShare& Entry : Demographics)
    {
        Result.Demographics.Add(Entry);
    }
    Result.DailyRhythm = DailyRhythm;
    Result.JakeReactionRule = JakeReactionRule;
    return Result;
}
}

TArray<FPopulationDensityContract> FPopulationProductionCatalog::BuildDensityContracts()
{
    return {
        Density(EPopulationDensityArchetype::MajorImperialCity, TEXT("Major Imperial City"), 80, 150, 20, 50),
        Density(EPopulationDensityArchetype::RegionalTown, TEXT("Regional Town"), 30, 60, 10, 25),
        Density(EPopulationDensityArchetype::Village, TEXT("Village"), 10, 30, INDEX_NONE, INDEX_NONE),
        Density(EPopulationDensityArchetype::FrontierWilderness, TEXT("Frontier / Wilderness"), 0, 10, INDEX_NONE, INDEX_NONE),
        Density(EPopulationDensityArchetype::PirateHaven, TEXT("Pirate Haven"), 40, 100, INDEX_NONE, INDEX_NONE),
        Density(EPopulationDensityArchetype::Absence, TEXT("Region 06 Absence"), INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE)
    };
}

TArray<FRegionalPopulationProductionProfile> FPopulationProductionCatalog::BuildRegionalProfiles()
{
    TArray<FRegionalPopulationProductionProfile> Result;
    Result.Reserve(RequiredRegionalProfiles);

    {
        FRegionalPopulationProductionProfile Entry = Profile(
            TEXT("population.pale-isle"), TEXT("Pale Isle Imperial"),
            TEXT("docs/design/npcs/regional_populations.md Section 3"),
            {Share(TEXT("Imperial Citizens"), 60), Share(TEXT("Imperial Immigrants"), 20),
             Share(TEXT("Regional Migrants"), 15), Share(TEXT("Sailors / Transient"), 5)},
            TEXT("Dawn deliveries; market growth through morning; midday peak; fashionable afternoon; tavern transition evening; streets thin dramatically at night."),
            TEXT("Unknown pirate wariness becomes path-specific recognition; honorable, feared and mixed reactions remain visibly distinct."));
        Entry.bHighDensityCity = true;
        Result.Add(Entry);
    }

    Result.Add(Profile(
        TEXT("population.rexa-moran"), TEXT("Rexa / Moran Tropical"),
        TEXT("docs/design/npcs/regional_populations.md Section 4"),
        {Share(TEXT("Indigenous Rexan"), 40), Share(TEXT("Mixed Heritage"), 30),
         Share(TEXT("Imperial Colonists"), 20), Share(TEXT("Sailors / Traders"), 10)},
        TEXT("Dawn is a primary market/fishing peak; 11:00-15:00 siesta empties streets; 17:00-22:00 is a second social peak; warm nights stay active."),
        TEXT("Indigenous access changes through Mama Jacinta's network; Imperial colonists follow Imperial patterns; mixed communities judge deeds pragmatically.")));

    Result.Add(Profile(
        TEXT("population.fjordlund"), TEXT("Fjordlund Northern"),
        TEXT("docs/design/npcs/regional_populations.md Section 5"),
        {Share(TEXT("Fjordlund Native"), 85), Share(TEXT("Imperial Presence"), 10),
         Share(TEXT("Traders / Transient"), 5)},
        TEXT("Long summer days extend work; short winter days compress outside activity and move community life into the mead hall."),
        TEXT("Trust is slow and visit-based; Mira's connection helps but never grants instant belonging; earned trust becomes community-wide loyalty.")));

    Result.Add(Profile(
        TEXT("population.ashenmoor"), TEXT("Ashenmoor Volcanic"),
        TEXT("docs/design/npcs/regional_populations.md Section 6"),
        {Share(TEXT("Indigenous Ashenmoor"), 75), Share(TEXT("Imperial Presence"), 5),
         Share(TEXT("Traders / Nomads"), 20)},
        TEXT("04:00-07:00 is a primary activity window; daytime heat drives shade/indoor craft; evening and night reopen social life and travel."),
        TEXT("Mbah Seruni's network and demonstrated survival competence shape acceptance; the cultural framing is that the mountain decides.")));

    Result.Add(Profile(
        TEXT("population.quiet-coast"), TEXT("Quiet Coast Frontier"),
        TEXT("docs/design/npcs/regional_populations.md Section 7"),
        {Share(TEXT("Settler Families"), 70), Share(TEXT("Drifters / Seekers"), 20),
         Share(TEXT("Traders"), 10)},
        TEXT("Frontier activity follows practical work and supply needs; no separate invented clock schedule is added beyond the source."),
        TEXT("Reputation labels matter less than witnessed deeds; help and harm are remembered locally.")));

    Result.Add(Profile(
        TEXT("population.pirate-havens"), TEXT("Pirate Havens"),
        TEXT("docs/design/npcs/regional_populations.md Section 8"),
        {Share(TEXT("Active Crews"), 40), Share(TEXT("Haven Residents"), 35),
         Share(TEXT("Fugitives / Seekers"), 15), Share(TEXT("Traders / Fences"), 10)},
        TEXT("Density and activity are intentionally variable around docks and taverns rather than normalised into an Imperial daily rhythm."),
        TEXT("Captain status establishes baseline respect; ship reputation and treatment of crew are watched continuously.")));

    {
        FRegionalPopulationProductionProfile Entry = Profile(
            TEXT("population.highmoore"), TEXT("Highmoore — The Fifth Register"),
            TEXT("highmoore populations.md Sections 1-8; highmoore settlements.md; highmoore region.md"),
            {},
            TEXT("Population life follows House war, levy/burning seasons, fortified farms, villages, three market towns and roads. Ambient layers cover base culture, current events, Jake reaction, weather and time; Belos/reconstruction alter the world without turning gossip into a morality meter."),
            TEXT("Every interaction begins from the problem of 'whose man are you'; yeomen may respect Jake but never become deferential. Standing is read through gates, names, road wardens and repeated local recognition, never a reputation meter."));
        Entry.bDemographicPercentagesUnspecified = true;
        Result.Add(Entry);
    }

    {
        FRegionalPopulationProductionProfile Entry = Profile(
            TEXT("population.region-06"), TEXT("Region 06 — The Absence"),
            TEXT("docs/design/npcs/regional_populations.md Section 9"),
            {},
            TEXT("No conventional crowd rhythm: maintained settlements with missing people and rare, notable remaining inhabitants create the horror through absence."),
            TEXT("No normal reputation crowd response is authored; every remaining person is individually notable."));
        Entry.bPopulationDefinedByAbsence = true;
        Result.Add(Entry);
    }

    return Result;
}

TArray<FPopulationProductionDesignGap> FPopulationProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.population.highmoore-percentages"),
            TEXT("Highmoore's later population master explicitly defines four classes — Houses, Yeomanry, Villages and Roads — but does not assign demographic percentages. Production must preserve the classes without inventing a 100-percent split."),
            TEXT("highmoore populations.md Section 2")
        },
        {
            TEXT("design-gap.population.open-sea"),
            TEXT("The regional population master defines civilian settlements and Pirate Havens, not an Open Sea civilian crowd profile. Ship crews and naval actors stay under ship/naval authorities."),
            TEXT("docs/design/npcs/regional_populations.md; naval combat system.md; ship management.md")
        },
        {
            TEXT("design-gap.population.runtime-assets"),
            TEXT("Crowd meshes, culturally reviewed dress sets, routines, audio beds, ambient-line assets and actual level placements are not evidenced as reviewed Unreal assets."),
            TEXT("docs/design/npcs/regional_populations.md Sections 10-15; highmoore populations.md; repository asset evidence")
        }
    };
}

bool FPopulationProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FPopulationDensityContract> DensityContracts = BuildDensityContracts();
    if (DensityContracts.Num() != 6)
    {
        OutErrors.Add(TEXT("Population density production must preserve the six authored density/absence archetypes."));
    }

    for (const FPopulationDensityContract& DensityEntry : DensityContracts)
    {
        if (DensityEntry.DisplayName.IsEmpty() || DensityEntry.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every population density archetype requires display identity and governing source."));
        }
        if (DensityEntry.Archetype != EPopulationDensityArchetype::Absence)
        {
            if (DensityEntry.CentreVisibleMinimum == INDEX_NONE
                || DensityEntry.CentreVisibleMaximum == INDEX_NONE
                || DensityEntry.CentreVisibleMinimum > DensityEntry.CentreVisibleMaximum)
            {
                OutErrors.Add(FString::Printf(TEXT("Population density archetype %s has invalid authored centre range."), *DensityEntry.DisplayName));
            }
        }
    }

    const TArray<FRegionalPopulationProductionProfile> Profiles = BuildRegionalProfiles();
    if (Profiles.Num() != RequiredRegionalProfiles)
    {
        OutErrors.Add(FString::Printf(TEXT("Population production requires exactly %d source-backed regional/archetype profiles; found %d."), RequiredRegionalProfiles, Profiles.Num()));
    }

    TSet<FName> Seen;
    for (const FRegionalPopulationProductionProfile& ProfileEntry : Profiles)
    {
        if (ProfileEntry.StableId.IsNone() || ProfileEntry.DisplayName.IsEmpty()
            || ProfileEntry.GoverningSource.IsEmpty() || ProfileEntry.DailyRhythm.IsEmpty()
            || ProfileEntry.JakeReactionRule.IsEmpty())
        {
            OutErrors.Add(TEXT("Every regional population profile requires identity, source, rhythm and reaction law."));
        }
        if (Seen.Contains(ProfileEntry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate population profile id: %s"), *ProfileEntry.StableId.ToString()));
        }
        Seen.Add(ProfileEntry.StableId);

        if (!ProfileEntry.Demographics.IsEmpty())
        {
            int32 TotalPercentage = 0;
            for (const FPopulationDemographicShare& Demographic : ProfileEntry.Demographics)
            {
                if (Demographic.Group.IsEmpty() || Demographic.Percentage < 0)
                {
                    OutErrors.Add(FString::Printf(TEXT("Population profile %s contains invalid demographic data."), *ProfileEntry.StableId.ToString()));
                    continue;
                }
                TotalPercentage += Demographic.Percentage;
            }
            if (TotalPercentage != 100)
            {
                OutErrors.Add(FString::Printf(TEXT("Population profile %s demographic shares must total 100; found %d."), *ProfileEntry.StableId.ToString(), TotalPercentage));
            }
        }
        else if (!ProfileEntry.bPopulationDefinedByAbsence && !ProfileEntry.bDemographicPercentagesUnspecified)
        {
            OutErrors.Add(FString::Printf(TEXT("Population profile %s has no demographic split without an explicit source reason."), *ProfileEntry.StableId.ToString()));
        }

        if (ProfileEntry.bPopulationDefinedByAbsence && ProfileEntry.bDemographicPercentagesUnspecified)
        {
            OutErrors.Add(FString::Printf(TEXT("Population profile %s cannot be both absence-defined and an unspecified demographic population."), *ProfileEntry.StableId.ToString()));
        }

        if (ProfileEntry.bRuntimeCrowdAssetsAuthored || !ProfileEntry.RuntimeCrowdAssetRoot.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Population profile %s may not claim reviewed crowd assets before actual Unreal asset evidence exists."), *ProfileEntry.StableId.ToString()));
        }
    }

    const FRegionalPopulationProductionProfile* Highmoore = Profiles.FindByPredicate([](const FRegionalPopulationProductionProfile& Entry)
    {
        return Entry.StableId == FName(TEXT("population.highmoore"));
    });
    if (!Highmoore || !Highmoore->bDemographicPercentagesUnspecified || !Highmoore->Demographics.IsEmpty())
    {
        OutErrors.Add(TEXT("Highmoore must preserve its four authored classes without inventing demographic percentages."));
    }

    if (MinimumAmbientLinesPerMajorRegion != 200
        || CrowdReturnMinimumMinutes != 2
        || CrowdReturnMaximumMinutes != 5
        || PopulationTierCount != 3 || HighmooreClassCount != 4)
    {
        OutErrors.Add(TEXT("Population master constants drifted from regional_populations.md / highmoore populations.md."));
    }
    if (AllowsRandomAuthoredPopulationGeneration() || AllowsHighmooreReputationMeter())
    {
        OutErrors.Add(TEXT("Authored population production may not gain random content generation or a Highmoore reputation meter."));
    }
    if (!RequiresPurposeDrivenPathing() || !RequiresChildEngineProtection() || !RequiresChapterEvolution())
    {
        OutErrors.Add(TEXT("Population production must retain purpose-driven routines, child protection and chapter evolution."));
    }
    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Population production must keep the three current scope/asset gaps explicit."));
    }

    return OutErrors.IsEmpty();
}
