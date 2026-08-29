// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/ThreadRuntimeCoverage.h"

#include "Missions/AuthoredQuestCatalog.h"

#include <initializer_list>

namespace
{
FThreadNativeCoverageEntry Covered(
    const TCHAR* ThreadId,
    const TCHAR* Owner,
    const TCHAR* Source)
{
    FThreadNativeCoverageEntry Entry;
    Entry.ThreadId = ThreadId;
    Entry.NativeOwner = Owner;
    Entry.GoverningSource = Source;
    Entry.State = EThreadNativeCoverageState::NativeContractPresent;
    return Entry;
}

FThreadNativeCoverageEntry CoveredWithGaps(
    const TCHAR* ThreadId,
    const TCHAR* Owner,
    const TCHAR* Source,
    std::initializer_list<const TCHAR*> GapIds)
{
    FThreadNativeCoverageEntry Entry = Covered(ThreadId, Owner, Source);
    Entry.State = EThreadNativeCoverageState::NativeContractWithAuthorshipGaps;
    for (const TCHAR* GapId : GapIds)
    {
        Entry.AuthorshipGapIds.Add(FName(GapId));
    }
    return Entry;
}
}

TArray<FThreadNativeCoverageEntry> FThreadRuntimeCoverage::Build()
{
    return {
        Covered(
            TEXT("thread.crew.ines-esperanza"),
            TEXT("UCrewPersonalThreadsComponent"),
            TEXT("side quest catalog.md Section 2.1; named_crew_deep_dives.md Section 4.4")),
        Covered(
            TEXT("thread.crew.miras-coast"),
            TEXT("UCrewPersonalThreadsComponent"),
            TEXT("side quest catalog.md Sections 2.1/3; named_crew_deep_dives.md Section 2.4")),
        Covered(
            TEXT("thread.crew.big-toms-service"),
            TEXT("UCrewPersonalThreadsComponent"),
            TEXT("side quest catalog.md Section 2.1; named_crew_deep_dives.md Section 3.4")),
        CoveredWithGaps(
            TEXT("thread.crew.father-salvios-parish"),
            TEXT("UCrewPersonalThreadsComponent"),
            TEXT("side quest catalog.md Section 2.1"),
            {TEXT("design-gap.thread.salvio-parish-stage-chain")}),
        CoveredWithGaps(
            TEXT("thread.crew.estebans-last-chart"),
            TEXT("UCrewPersonalThreadsComponent"),
            TEXT("side quest catalog.md Section 2.1"),
            {TEXT("design-gap.thread.esteban-last-chart-destination"),
             TEXT("design-gap.thread.esteban-last-chart-stage-chain")}),
        Covered(
            TEXT("thread.archipelago.assassin-network"),
            TEXT("UAssassinNetworkThreadComponent"),
            TEXT("assassin network.md; assassin network questline.md")),
        CoveredWithGaps(
            TEXT("thread.archipelago.liberation-connections"),
            TEXT("ULiberationAllianceSubsystem"),
            TEXT("indigenous liberation alliance.md Section 4"),
            {TEXT("design-gap.thread.alliance-five-full-vision-connections")}),
        Covered(
            TEXT("thread.archipelago.kesslers-investigation"),
            TEXT("UArchipelagoThreadsComponent"),
            TEXT("side quest catalog.md Sections 2.2/3; recurring_quest_givers.md Section 2.1")),
        Covered(
            TEXT("thread.archipelago.ledger-trilogy"),
            TEXT("UArchipelagoThreadsComponent"),
            TEXT("regional dungeon catalog.md Section 8.1")),
        Covered(
            TEXT("thread.archipelago.schreiber-documentation"),
            TEXT("UArchipelagoThreadsComponent"),
            TEXT("recurring_quest_givers.md Section 8.2; secret bosses.md Section 8.4")),
        CoveredWithGaps(
            TEXT("thread.archipelago.captain-vasquez"),
            TEXT("UArchipelagoThreadsComponent"),
            TEXT("side quest catalog.md Sections 2.2/3; recurring_quest_givers.md Section 7.1"),
            {TEXT("design-gap.thread.vasquez-six-hour-stage-chain")}),
        Covered(
            TEXT("thread.archipelago.old-fortresses"),
            TEXT("UArchipelagoThreadsComponent"),
            TEXT("side quest catalog.md Section 2.2; regional dungeon catalog.md Tier-D sites")),
        Covered(
            TEXT("thread.highmoore.princess"),
            TEXT("UPrincessQuestStateComponent"),
            TEXT("princess quest overview.md and movement documents")),
        Covered(
            TEXT("thread.highmoore.light-elves"),
            TEXT("ULightElvesThreadComponent"),
            TEXT("wizard questline.md")),
        CoveredWithGaps(
            TEXT("thread.highmoore.reconstruction"),
            TEXT("UHighmooreReconstructionComponent"),
            TEXT("highmoore reconstruction.md"),
            {TEXT("design-gap.thread.reconstruction-turned-west-diplomatic-content"),
             TEXT("design-gap.thread.reconstruction-mountain-supply-line")}),
        Covered(
            TEXT("thread.cross.named-dead"),
            TEXT("UMemoryThreadsSubsystem"),
            TEXT("the_named_dead.md")),
        Covered(
            TEXT("thread.cross.ethan"),
            TEXT("UMemoryThreadsSubsystem"),
            TEXT("side quest catalog.md Section 2.4; main story structure.md Section 3.9; the_named_dead.md Section 2"))
    };
}

bool FThreadRuntimeCoverage::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FThreadNativeCoverageEntry> Coverage = Build();
    if (Coverage.Num() != RequiredThreadCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Thread runtime coverage requires exactly %d entries; found %d."),
            RequiredThreadCount,
            Coverage.Num()));
    }

    TSet<FName> CoverageIds;
    for (const FThreadNativeCoverageEntry& Entry : Coverage)
    {
        if (Entry.ThreadId.IsNone() || Entry.NativeOwner.IsEmpty() || Entry.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every Thread coverage entry requires id, native owner and governing source."));
            continue;
        }
        if (CoverageIds.Contains(Entry.ThreadId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate Thread coverage id: %s"), *Entry.ThreadId.ToString()));
        }
        CoverageIds.Add(Entry.ThreadId);

        if (Entry.State == EThreadNativeCoverageState::NativeContractPresent
            && !Entry.AuthorshipGapIds.IsEmpty())
        {
            OutErrors.Add(FString::Printf(
                TEXT("Thread %s claims full native source coverage while carrying authorship gaps."),
                *Entry.ThreadId.ToString()));
        }
        if (Entry.State == EThreadNativeCoverageState::NativeContractWithAuthorshipGaps
            && Entry.AuthorshipGapIds.IsEmpty())
        {
            OutErrors.Add(FString::Printf(
                TEXT("Thread %s is marked gap-bearing but has no stable gap ids."),
                *Entry.ThreadId.ToString()));
        }

        TSet<FName> GapIds;
        for (const FName GapId : Entry.AuthorshipGapIds)
        {
            if (GapId.IsNone() || GapIds.Contains(GapId))
            {
                OutErrors.Add(FString::Printf(
                    TEXT("Thread %s contains an empty or duplicate authorship gap id."),
                    *Entry.ThreadId.ToString()));
            }
            GapIds.Add(GapId);
        }
    }

    const TArray<FAuthoredThreadCatalogEntry> Catalog = FAuthoredQuestCatalog::BuildThreads();
    if (Catalog.Num() != RequiredThreadCount)
    {
        OutErrors.Add(TEXT("Authored Thread catalog no longer matches the seventeen-Thread coverage contract."));
    }
    for (const FAuthoredThreadCatalogEntry& Entry : Catalog)
    {
        if (!CoverageIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(
                TEXT("Authored Thread has no native source-coverage owner: %s"),
                *Entry.StableId.ToString()));
        }
    }

    return OutErrors.IsEmpty();
}
