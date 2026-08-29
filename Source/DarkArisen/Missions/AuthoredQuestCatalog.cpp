// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/AuthoredQuestCatalog.h"

namespace
{
FAuthoredThreadCatalogEntry Thread(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EAuthoredThreadRegion Region,
    const float MinHours,
    const float MaxHours,
    const EThreadActivationRead Activation,
    const TCHAR* ActivationDetail,
    const TCHAR* GoverningSource,
    const bool bFullySpecifiedElsewhere = false)
{
    FAuthoredThreadCatalogEntry Entry;
    Entry.StableId = StableId;
    Entry.DisplayName = DisplayName;
    Entry.Region = Region;
    Entry.AuthoredMinimumHours = MinHours;
    Entry.AuthoredMaximumHours = MaxHours;
    Entry.Activation = Activation;
    Entry.ActivationDetail = ActivationDetail;
    Entry.GoverningSource = GoverningSource;
    Entry.bFullySpecifiedElsewhere = bFullySpecifiedElsewhere;
    return Entry;
}
}

TArray<FAuthoredThreadCatalogEntry> FAuthoredQuestCatalog::BuildThreads()
{
    TArray<FAuthoredThreadCatalogEntry> Result;
    Result.Reserve(RequiredThreads);

    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.crew.ines-esperanza"), TEXT("Ines and Esperanza — the daughter"),
            EAuthoredThreadRegion::Crew, 9.0f, 9.0f, EThreadActivationRead::Conversation,
            TEXT("Chapter 5+, third meeting"), TEXT("side quest catalog.md Section 2.1"));
        Entry.MarkReward = 2;
        Result.Add(Entry);
    }
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.crew.miras-coast"), TEXT("Mira's coast"),
            EAuthoredThreadRegion::Crew, 7.0f, 7.0f, EThreadActivationRead::Overheard,
            TEXT("In Fjordlund"), TEXT("side quest catalog.md Sections 2.1 and 3"));
        Entry.MarkReward = 2;
        Entry.bExpiresByWorldState = true;
        Entry.ExpirationCondition = TEXT("Fjordlund Control resolves; no timer/warning UI");
        Result.Add(Entry);
    }
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.crew.big-toms-service"), TEXT("Big Tom's service"),
            EAuthoredThreadRegion::Crew, 6.0f, 6.0f, EThreadActivationRead::Absence,
            TEXT("Someone recognises him and leaves"), TEXT("side quest catalog.md Section 2.1; quest activation.md Section 2.6"));
        Entry.MarkReward = 2;
        Result.Add(Entry);
    }
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.crew.father-salvios-parish"), TEXT("Father Salvio's parish"),
            EAuthoredThreadRegion::Crew, 5.0f, 5.0f, EThreadActivationRead::Conversation,
            TEXT("Only after Jake has spared someone"), TEXT("side quest catalog.md Section 2.1"));
        Entry.MarkReward = 2;
        Result.Add(Entry);
    }
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.crew.estebans-last-chart"), TEXT("Esteban's last chart"),
            EAuthoredThreadRegion::Crew, 4.0f, 4.0f, EThreadActivationRead::Object,
            TEXT("A chart in his quarters"), TEXT("side quest catalog.md Section 2.1"));
        Entry.MarkReward = 2;
        Result.Add(Entry);
    }

    Result.Add(Thread(
        TEXT("thread.archipelago.assassin-network"), TEXT("The Assassin Network"),
        EAuthoredThreadRegion::Archipelago, 6.0f, 10.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Fully specified in its own questline source"),
        TEXT("side quest catalog.md Section 2.2; assassin network.md"), true));
    Result.Add(Thread(
        TEXT("thread.archipelago.liberation-connections"), TEXT("The Liberation connections"),
        EAuthoredThreadRegion::Archipelago, 12.0f, 12.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Five connection arcs as one Thread"),
        TEXT("side quest catalog.md Section 2.2; indigenous liberation alliance.md Section 4"), true));
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.archipelago.kesslers-investigation"), TEXT("Kessler's investigation"),
            EAuthoredThreadRegion::Archipelago, 7.0f, 7.0f, EThreadActivationRead::SpecifiedElsewhere,
            TEXT("Terminates at Castillo Dorado's bribe ledgers"),
            TEXT("side quest catalog.md Sections 2.2 and 3; imperial contacts.md; castle catalog.md Section 6.6"));
        Entry.bExpiresByWorldState = true;
        Entry.ExpirationCondition = TEXT("Castillo Dorado falls or de Silva dies; no timer/warning UI");
        Result.Add(Entry);
    }
    Result.Add(Thread(
        TEXT("thread.archipelago.ledger-trilogy"), TEXT("The Ledger Trilogy"),
        EAuthoredThreadRegion::Archipelago, 8.0f, 8.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Four locations, a century of families"),
        TEXT("side quest catalog.md Section 2.2; regional dungeon catalog.md Section 8.1")));
    Result.Add(Thread(
        TEXT("thread.archipelago.schreiber-documentation"), TEXT("Doctor Schreiber's documentation"),
        EAuthoredThreadRegion::Archipelago, 5.0f, 5.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Recurring quest-giver source; tied to the first landing beach"),
        TEXT("side quest catalog.md Section 2.2; recurring quest givers.md; secret bosses.md Section 8.4")));
    {
        FAuthoredThreadCatalogEntry Entry = Thread(
            TEXT("thread.archipelago.captain-vasquez"), TEXT("Captain Vasquez"),
            EAuthoredThreadRegion::Archipelago, 6.0f, 6.0f, EThreadActivationRead::SpecifiedElsewhere,
            TEXT("May intersect assassin contracts on Jake"),
            TEXT("side quest catalog.md Sections 2.2 and 3; recurring quest givers.md; assassin network.md Section 5.3"));
        Entry.bExpiresByWorldState = true;
        Entry.ExpirationCondition = TEXT("Vasquez is sunk, by anyone; gossip web mentions it once");
        Result.Add(Entry);
    }
    Result.Add(Thread(
        TEXT("thread.archipelago.old-fortresses"), TEXT("The Old Fortresses"),
        EAuthoredThreadRegion::Archipelago, 10.0f, 10.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Four Alliance-gated reclamations"),
        TEXT("side quest catalog.md Section 2.2; castle catalog.md Section 12"), true));

    Result.Add(Thread(
        TEXT("thread.highmoore.princess"), TEXT("The Princess"),
        EAuthoredThreadRegion::Highmoore, 4.0f, 4.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Four hours across an eight-hour authored journey"),
        TEXT("side quest catalog.md Section 2.3; princess quest overview.md"), true));
    Result.Add(Thread(
        TEXT("thread.highmoore.light-elves"), TEXT("The Light Elves"),
        EAuthoredThreadRegion::Highmoore, 4.0f, 8.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Fully specified; pressure rate can climb but the Thread does not fully expire"),
        TEXT("side quest catalog.md Sections 2.3 and 3; wizard questline.md"), true));
    Result.Add(Thread(
        TEXT("thread.highmoore.reconstruction"), TEXT("The Reconstruction"),
        EAuthoredThreadRegion::Highmoore, 12.0f, 0.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("12h+; no completion bonus balancing layer"),
        TEXT("side quest catalog.md Section 2.3; highmoore reconstruction.md"), true));

    Result.Add(Thread(
        TEXT("thread.cross.named-dead"), TEXT("The Named Dead"),
        EAuthoredThreadRegion::CrossRegional, 8.0f, 8.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Diffuse accumulation; deliberately has no ending"),
        TEXT("side quest catalog.md Section 2.4; the named dead.md"), true));
    Result.Add(Thread(
        TEXT("thread.cross.ethan"), TEXT("Ethan"),
        EAuthoredThreadRegion::CrossRegional, 6.0f, 6.0f, EThreadActivationRead::SpecifiedElsewhere,
        TEXT("Touches the spine and resolves in Chapter 9 whether pursued or not"),
        TEXT("side quest catalog.md Section 2.4; main story structure.md Section 3.9"), true));

    return Result;
}

TArray<FTurnDistributionRequirement> FAuthoredQuestCatalog::BuildTurnDistribution()
{
    return {
        {TEXT("region.rexa-moran"), 22},
        {TEXT("region.fjordlund"), 18},
        {TEXT("region.ashenmoor"), 16},
        {TEXT("region.quiet-coast"), 14},
        {TEXT("region.pale-isle-silvera"), 15},
        {TEXT("region.at-sea"), 19},
        {TEXT("region.highmoore"), 24},
        {TEXT("region.06"), 4}};
}

TArray<FTurnShapeRequirement> FAuthoredQuestCatalog::BuildTurnShapeRequirements()
{
    // The source explicitly labels these counts approximate even though they sum to 132.
    return {
        {TEXT("shape.find-person-document-place"), 31},
        {TEXT("shape.settle-dispute-debt-feud"), 24},
        {TEXT("shape.lie-investigation"), 19},
        {TEXT("shape.unknown-problem"), 22},
        {TEXT("shape.get-person-somewhere"), 17},
        {TEXT("shape.death-identity-matters"), 14},
        {TEXT("shape.inspect-place"), 5}};
}

TArray<FKnownMissionIdentity> FAuthoredQuestCatalog::BuildKnownMissionIdentities()
{
    // These four are concrete source identities already implemented by URexaM2MissionCatalog.
    return {
        {TEXT("Rexa.Turn.EmptyHammock"), TEXT("Rexa.Turn.01"), TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp"), false},
        {TEXT("Rexa.Turn.ThreeCutsInStone"), TEXT("Rexa.Turn.02"), TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp"), false},
        {TEXT("Rexa.Turn.SaltLedger"), TEXT("Rexa.Turn.03"), TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp"), false},
        {TEXT("Rexa.Standing.Salvage.SanTelmoBell"), TEXT("Rexa.Standing.Salvage.01"), TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp"), true}};
}

TArray<FQuestCatalogDesignGap> FAuthoredQuestCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.turn-identities"),
            TEXT("The Phase-11 catalog locks 132 Turns and exact regional distribution, but only three concrete Turn identities are currently authored in source. The remaining 129 must be written from approved narrative material rather than generated to satisfy the count."),
            TEXT("side quest catalog.md Section 4; RexaM2MissionCatalog.cpp")
        },
        {
            TEXT("design-gap.standing-identities"),
            TEXT("mission types.md locks nine structures and 147 finite variants, but only the San Telmo Bell has a complete source identity today. The remaining 146 variants require authored people/places/outcomes; template generation is prohibited."),
            TEXT("mission types.md Sections 1-10; RexaM2MissionCatalog.cpp")
        },
        {
            TEXT("design-gap.thread-activation-detail"),
            TEXT("Several Threads are named and scoped by side quest catalog.md but defer exact activation sequencing to subsystem/NPC documents. Those triggers must be read from the cited sources before registration in UQuestJournalComponent."),
            TEXT("side quest catalog.md Section 2; quest activation.md")
        }
    };
}

bool FAuthoredQuestCatalog::ValidateCatalog(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredThreadCatalogEntry> Threads = BuildThreads();
    if (Threads.Num() != RequiredThreads)
    {
        OutErrors.Add(FString::Printf(TEXT("Thread catalog requires exactly %d entries; found %d."), RequiredThreads, Threads.Num()));
    }

    TSet<FName> ThreadIds;
    int32 CrewThreadCount = 0;
    int32 CrewMarkTotal = 0;
    for (const FAuthoredThreadCatalogEntry& Entry : Threads)
    {
        if (Entry.StableId.IsNone() || Entry.DisplayName.IsEmpty() || Entry.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every Thread requires stable id, authored name and governing source."));
            continue;
        }
        if (ThreadIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate Thread id: %s"), *Entry.StableId.ToString()));
        }
        ThreadIds.Add(Entry.StableId);

        if (Entry.AuthoredMinimumHours <= 0.0f)
        {
            OutErrors.Add(FString::Printf(TEXT("Thread %s has no authored minimum length."), *Entry.StableId.ToString()));
        }
        if (Entry.AuthoredMaximumHours > 0.0f && Entry.AuthoredMaximumHours < Entry.AuthoredMinimumHours)
        {
            OutErrors.Add(FString::Printf(TEXT("Thread %s has invalid authored duration range."), *Entry.StableId.ToString()));
        }

        if (Entry.Region == EAuthoredThreadRegion::Crew)
        {
            ++CrewThreadCount;
            CrewMarkTotal += Entry.MarkReward;
            if (Entry.MarkReward != 2)
            {
                OutErrors.Add(FString::Printf(TEXT("Every named crew Thread grants exactly two Marks: %s"), *Entry.StableId.ToString()));
            }
        }
    }
    if (CrewThreadCount != 5 || CrewMarkTotal != 10)
    {
        OutErrors.Add(TEXT("Crew Thread catalog must remain five Threads / ten Marks total."));
    }

    const TArray<FTurnDistributionRequirement> Distribution = BuildTurnDistribution();
    int32 DistributionTotal = 0;
    TSet<FName> RegionIds;
    for (const FTurnDistributionRequirement& Region : Distribution)
    {
        if (Region.RegionId.IsNone() || Region.RequiredCount <= 0 || RegionIds.Contains(Region.RegionId))
        {
            OutErrors.Add(TEXT("Turn distribution contains invalid/duplicate region requirement."));
        }
        RegionIds.Add(Region.RegionId);
        DistributionTotal += Region.RequiredCount;
    }
    if (DistributionTotal != RequiredTurns)
    {
        OutErrors.Add(FString::Printf(TEXT("Turn regional distribution must total %d; found %d."), RequiredTurns, DistributionTotal));
    }

    const TArray<FKnownMissionIdentity> Known = BuildKnownMissionIdentities();
    int32 KnownTurns = 0;
    int32 KnownStanding = 0;
    TSet<FName> QuestIds;
    TSet<FName> VariantIds;
    for (const FKnownMissionIdentity& Mission : Known)
    {
        if (Mission.QuestId.IsNone() || Mission.AuthoredVariantId.IsNone() || Mission.GoverningSource.IsEmpty()
            || QuestIds.Contains(Mission.QuestId) || VariantIds.Contains(Mission.AuthoredVariantId))
        {
            OutErrors.Add(TEXT("Known mission identity set contains invalid or duplicate entry."));
            continue;
        }
        QuestIds.Add(Mission.QuestId);
        VariantIds.Add(Mission.AuthoredVariantId);
        Mission.bStanding ? ++KnownStanding : ++KnownTurns;
    }
    if (KnownTurns != IndividuallyAuthoredTurnIdentitiesInSource
        || KnownStanding != IndividuallyAuthoredStandingIdentitiesInSource)
    {
        OutErrors.Add(TEXT("Known mission identity count drifted from source-authored coverage."));
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Quest catalog must preserve all three current authored-content gaps."));
    }

    return OutErrors.IsEmpty();
}
