// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/AuthoredTravelProductionCatalog.h"

namespace
{
FAuthoredTravelProductionRoute Route(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* RuntimeOwner,
    const EAuthoredTravelMode Mode)
{
    FAuthoredTravelProductionRoute Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.RuntimeOwner = RuntimeOwner;
    Result.Mode = Mode;
    return Result;
}
}

TArray<FAuthoredTravelProductionRoute> FAuthoredTravelProductionCatalog::BuildCoreRoutes()
{
    TArray<FAuthoredTravelProductionRoute> Result;
    Result.Reserve(RequiredCoreRouteCount);

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.archipelago.physical-sea-passage"),
            TEXT("Archipelago physical sea passage"),
            TEXT("travel system.md Sections 1-3 and 5; ship physics.md; ship navigation.md"),
            TEXT("SeaPassageComponent / LaLiberacionShip"),
            EAuthoredTravelMode::SeaPassage);
        Entry.bRequiresPhysicalTraversal = true;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.archipelago.feared-waters-guide"),
            TEXT("Feared waters opened by guide"),
            TEXT("travel system.md Section 3.4; indigenous liberation alliance.md Section 5.3"),
            TEXT("Alliance trust/guide authority + SeaPassageComponent"),
            EAuthoredTravelMode::FearedWatersGuide);
        Entry.bRequiresGuideOrTrust = true;
        Entry.bRequiresPhysicalTraversal = true;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.highmoore.crystal-caves.first"),
            TEXT("Crystal Caves first continental passage"),
            TEXT("travel system.md Section 7; crystal caves.md; dungeon system global.md Crystal Caves carve-out"),
            TEXT("CrystalCavesPassageComponent"),
            EAuthoredTravelMode::CrystalCavesFirstPassage);
        Entry.MinimumRealMinutes = CrystalFirstPassageMinimumRealMinutes;
        Entry.MaximumRealMinutes = CrystalFirstPassageMaximumRealMinutes;
        Entry.bRequiresPhysicalTraversal = true;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.highmoore.crystal-caves.repeat"),
            TEXT("Crystal Caves repeat continental passage"),
            TEXT("travel system.md Sections 5.2 and 7.1; crystal caves.md Section 9.1"),
            TEXT("CrystalCavesPassageComponent"),
            EAuthoredTravelMode::CrystalCavesRepeatPassage);
        Entry.MinimumRealMinutes = CrystalRepeatPassageMinimumRealMinutes;
        Entry.MaximumRealMinutes = CrystalRepeatPassageMaximumRealMinutes;
        Entry.bRequiresPhysicalTraversal = true;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.highmoore.stable-horse-relocation"),
            TEXT("Highmoore stable horse relocation"),
            TEXT("travel system.md Section 5.2; mounted travel.md Section 6.3"),
            TEXT("HighmooreHorseComponent / authored stable owner"),
            EAuthoredTravelMode::HorseStableRelocation);
        Entry.bMovesJake = false;
        Entry.bMovesHorseOnly = true;
        Entry.bRequiresPhysicalTraversal = false;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    {
        FAuthoredTravelProductionRoute Entry = Route(
            TEXT("travel.settlement.walk-it"),
            TEXT("Within-settlement walking"),
            TEXT("travel system.md Section 5.2 exception 3; interaction system.md"),
            TEXT("JakeCharacter"),
            EAuthoredTravelMode::SettlementWalking);
        Entry.bRequiresPhysicalTraversal = true;
        Entry.bMenuTravelPermitted = false;
        Result.Add(Entry);
    }

    return Result;
}

TArray<FAuthoredTravelDesignGap> FAuthoredTravelProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.travel.sea-route-duration-distance-table"),
            TEXT("The travel law requires real sailing and SeaPassageComponent supports authored time/distance floors, but the repository does not supply one canonical pairwise table of minimum minutes/metres for every port-to-port route. Generic values are not invented."),
            TEXT("travel system.md; ship navigation.md; SeaPassageComponent")
        },
        {
            TEXT("design-gap.travel.feared-water-route-identities"),
            TEXT("Six feared-water regions are locked as guide/trust-opened knowledge, but this source boundary does not manufacture six route names or coordinates where the governing network documents do not individually provide them."),
            TEXT("travel system.md Section 3.4; indigenous liberation alliance.md Section 5.3")
        },
        {
            TEXT("design-gap.travel.runtime-world-links"),
            TEXT("Actual streaming boundaries, level portals, nav routes, stable actors, sea lanes and world-placement assets remain Unreal production work and may not be claimed from source contracts."),
            TEXT("Docs/NEXT_ROCKSTAR_TRANCHE.md; repository asset evidence")
        }
    };
}

bool FAuthoredTravelProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredTravelProductionRoute> Routes = BuildCoreRoutes();
    if (Routes.Num() != RequiredCoreRouteCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Travel production requires exactly %d core law routes; found %d."), RequiredCoreRouteCount, Routes.Num()));
    }

    TSet<FName> Seen;
    for (const FAuthoredTravelProductionRoute& Entry : Routes)
    {
        if (Entry.StableId.IsNone() || Entry.DisplayName.IsEmpty() || Entry.GoverningSource.IsEmpty() || Entry.RuntimeOwner.IsEmpty())
        {
            OutErrors.Add(TEXT("Every travel production route requires identity, source and runtime owner."));
        }
        if (Seen.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate travel production id: %s"), *Entry.StableId.ToString()));
        }
        Seen.Add(Entry.StableId);
        if (Entry.bMenuTravelPermitted)
        {
            OutErrors.Add(FString::Printf(TEXT("Travel route %s may not become menu travel."), *Entry.StableId.ToString()));
        }
    }

    const FAuthoredTravelProductionRoute* First = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry)
    {
        return Entry.Mode == EAuthoredTravelMode::CrystalCavesFirstPassage;
    });
    if (!First || First->MinimumRealMinutes != 90 || First->MaximumRealMinutes != 120 || !First->bRequiresPhysicalTraversal)
    {
        OutErrors.Add(TEXT("Crystal Caves first passage must remain physical and 90-120 real minutes."));
    }

    const FAuthoredTravelProductionRoute* Repeat = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry)
    {
        return Entry.Mode == EAuthoredTravelMode::CrystalCavesRepeatPassage;
    });
    if (!Repeat || Repeat->MinimumRealMinutes != 4 || Repeat->MaximumRealMinutes != 6 || !Repeat->bRequiresPhysicalTraversal)
    {
        OutErrors.Add(TEXT("Crystal Caves repeat passage must remain physical and 4-6 real minutes."));
    }

    const FAuthoredTravelProductionRoute* Stable = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry)
    {
        return Entry.Mode == EAuthoredTravelMode::HorseStableRelocation;
    });
    if (!Stable || Stable->bMovesJake || !Stable->bMovesHorseOnly)
    {
        OutErrors.Add(TEXT("Highmoore stabling may move the horse only, never Jake."));
    }

    if (FearedWaterRegionCount != 6 || AllowsFastTravelOverWater() || AllowsMapClickMovement()
        || AllowsCompass() || AllowsMinimap() || AllowsPlayerDot())
    {
        OutErrors.Add(TEXT("Travel laws drifted: six feared-water regions and zero fast-travel/map-click/compass/minimap/player-dot capability are required."));
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Travel production must retain the current three route/runtime design gaps."));
    }

    return OutErrors.IsEmpty();
}
