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

void FAuthoredDungeonProductionProfiles::AppendColonial(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    Out.Add(Profile(
        TEXT("dungeon.colonial.valdena-mine"),
        TEXT("regional dungeon catalog.md 6.1"),
        TEXT("Environmental/documentary discovery of an abandoned Imperial silver mine."),
        TEXT("Colonial clockwork drainage."),
        TEXT("Structural danger, flooding and gas."),
        TEXT("The deepest gallery still holds tools dropped exactly where workers held them, with the shift bell still hanging."),
        TEXT("The Shift, a Colonial-Dead encounter."),
        TEXT("Labour rolls that combine with other ledgers to trace families across a century.")));

    Out.Add(Profile(
        TEXT("dungeon.colonial.siege-tunnels-ciudadela"),
        TEXT("regional dungeon catalog.md 6.2; castle catalog.md 3.3; castle sieges.md 4.2"),
        TEXT("Relational discovery at Cultural Web Trust Tier 3; the undercity beneath La Ciudadela is both tactical route and dungeon."),
        TEXT("Hybrid Colonial engineering over older pre-colonial foundations."),
        TEXT("Flooding and structural danger."),
        TEXT("A chamber exposes 1651 Imperial engineering physically load-bearing on a much older architecture."),
        TEXT("The Foundation, a guardian older than the fort above it."),
        TEXT("The undercity route into La Ciudadela, the strongest authored tactical asset for that siege.")));

    Out.Add(Profile(
        TEXT("dungeon.colonial.plague-vault"),
        TEXT("regional dungeon catalog.md 6.3"),
        TEXT("Documentary discovery through a sealed Imperial site referenced in the Conquest Archives."),
        TEXT("Colonial lock/key logic; the Empire locks things."),
        TEXT("Structural danger, darkness and contamination."),
        TEXT("The Empire sealed the vault in 1782 with people still inside and the seal held."),
        TEXT("The Sealed; restrained Style-D treatment."),
        TEXT("The signed sealing order, absent from the official Conquest Archives.")));

    Out.Add(Profile(
        TEXT("dungeon.colonial.company-works"),
        TEXT("regional dungeon catalog.md 6.4"),
        TEXT("Environmental/documentary discovery of an abandoned HAC industrial works that stopped paying nine years earlier."),
        TEXT("Colonial industrial clockwork at Tier-C scale."),
        TEXT("Structural danger and machinery."),
        TEXT("The waterwheel and looms still run unattended, weaving nothing into a room already full of it."),
        TEXT("The Works as environmental encounter plus The Last Shift Supervisor as survivor variant."),
        TEXT("Company books proving withdrawn operations left indentured contracts enforceable for people still nearby.")));
}

void FAuthoredDungeonProductionProfiles::AppendRegion06AndCrossNetwork(TArray<FAuthoredDungeonProductionProfile>& Out)
{
    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.region06.first-house"),
            TEXT("regional dungeon catalog.md 7.2; dungeon system global.md 6.2"),
            TEXT("Environmental discovery: the settlement's first building; the door is simply open."),
            TEXT("Fjordlund grammar used incorrectly; the tide table describes a tide that does not occur here."),
            TEXT("No hazard. The absence of a hazard is explicitly part of the authored unease."),
            TEXT("A meal for six sits half-eaten and has not spoiled."),
            TEXT("No boss; the empty bottom state is deliberate."),
            TEXT(""));
        P.bRewardExplicitlyNone = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.region06.third-bell"),
            TEXT("regional dungeon catalog.md 7.3"),
            TEXT("Relational discovery from the warning not to stay past a third bell that supposedly does not exist."),
            TEXT("Acoustic Region-06 logic that deliberately does not resolve."),
            TEXT("Disorientation."),
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
            TEXT("dungeon.region06.fourth-fortress"),
            TEXT("regional dungeon catalog.md 7.4; secret bosses.md 7; castle catalog.md 12.3"),
            TEXT("Late-game Region 06; all five networks know it exists and none will confirm it. Any Blood-tier relationship can lead only to the warning, not a map marker."),
            TEXT("Region-06 unreliable grammar; exact internal puzzle sequence is deliberately not authored in Phase 7."),
            TEXT("The fortress was sited against something; exact internal hazard is deliberately withheld."),
            TEXT("The only pre-colonial fortress whose walls face inward."),
            TEXT("WITHHELD by the governing source because the answer belongs to the main-story Region-06 question."),
            TEXT(""));
        P.bBossWithheldOrUnresolved = true;
        P.bRewardWithheldOrUnresolved = true;
        Out.Add(P);
    }

    {
        FAuthoredDungeonProductionProfile P = Profile(
            TEXT("dungeon.cross-network.fifth-connection"),
            TEXT("secret bosses.md 6; dungeon system global.md 6.1"),
            TEXT("Blood trust with all five networks plus the fifth-connection gathering; five partial map traditions combine into one route."),
            TEXT("All five map languages contribute; no single culture's grammar is sufficient."),
            TEXT("Exact environmental hazard is not separately locked; the site is reached by assembled cross-network knowledge."),
            TEXT(""),
            TEXT("No fight in any branch, under any circumstances. 'The Agreement' is the authored bottom state, not a combat boss."),
            TEXT("Raises the Alliance connection multiplier beyond x1.60 and ends in a five-network scene whose camera does not find Jake."));
        P.bImageWithheld = true;
        P.bSupportsExplicitNonCombatResolution = true;
        Out.Add(P);
    }
}
