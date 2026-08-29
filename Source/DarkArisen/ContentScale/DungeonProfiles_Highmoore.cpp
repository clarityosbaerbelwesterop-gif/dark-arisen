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

void FAuthoredDungeonProductionProfiles::AppendHighmoore(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.long-barrow-braehead"),
            TEXT("dungeon system global.md 3.2, 6.1"),
            TEXT("Relational access through House Brae's old records; Lord Ewan permits attempts to read what the house itself cannot."),
            TEXT("Highmoore practices: customary rules about hours, places, charms, fog and light are the key rather than a generic lock."),
            TEXT("Exact primary environmental hazard is not separately specified for this barrow."),
            TEXT("The same old script that is illegible outside continues inside the barrow and becomes legible there."),
            TEXT("No boss; this is one of the four global no-boss carve-outs."),
            TEXT(""));
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.drowned-mill"),
            TEXT("dungeon system global.md 3.2"),
            TEXT("Waterlands site; exact discovery trigger is not further locked in the global catalog."),
            TEXT("Highmoore practices combined with water behaviour; exact interaction sequence remains a level-authoring gap."),
            TEXT("Water."),
            TEXT("The mill wheel still turns underwater, driven by the current, and has done so for eighty years."),
            TEXT("The Miller, a remnant encounter."),
            TEXT(""));
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.stone-row"),
            TEXT("dungeon system global.md 3.2"),
            TEXT("High Moor site tied to the local customary-practices network; exact discovery trigger is not separately locked."),
            TEXT("The fog practice is the solution: stop moving, sit down and wait."),
            TEXT("Fog."),
            TEXT(""),
            TEXT(""),
            TEXT(""));
        P.bImageWithheld = true;
        P.bBossWithheldOrUnresolved = true;
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.ashby-crossing"),
            TEXT("dungeon system global.md 3.2"),
            TEXT("Beneath the contested Ashby fields that changed hands eleven times in forty years."),
            TEXT("Highmoore practices applied to layered military tunnelling; no invented generic lock is added."),
            TEXT("Structural underground military works; exact primary hazard is not further locked."),
            TEXT("Four separate periods of military tunnelling intersect below the field, each dug without knowledge of the others."),
            TEXT("The Third Dig."),
            TEXT(""));
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.bowyers-wood"),
            TEXT("dungeon system global.md 3.2; house belos.md 3.2; highmoore smiths.md 3.2"),
            TEXT("A four-hundred-year managed yew wood tied to House Belos and Gruffydd's bowyer craft."),
            TEXT("Highmoore practices and observation of managed boundary traditions; exact puzzle sequence is not locked."),
            TEXT("Woodland traversal; no separate primary hazard is specified."),
            TEXT("The oldest yews were planted by people whose names remain on the boundary stones."),
            TEXT(""),
            TEXT(""));
        P.bBossWithheldOrUnresolved = true;
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.highmoore.under-chamber-halford"),
            TEXT("dungeon system global.md 3.2; highmoore settlements.md 5"),
            TEXT("Beneath Halford Abbey; the player may reach the record before ever meeting Elowen Arion."),
            TEXT("Highmoore practices and record-reading; exact mechanical sequence remains unauthored."),
            TEXT("Abbey under-chamber; exact primary hazard is not separately specified."),
            TEXT("Four generations of hostage-guesting records include Elowen Arion's eighteen months in a fair hand with only a date and no explanation."),
            TEXT(""),
            TEXT(""));
        P.bBossWithheldOrUnresolved = true;
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }
}
