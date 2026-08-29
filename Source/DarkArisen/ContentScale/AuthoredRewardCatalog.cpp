// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredRewardCatalog.h"

#include "ContentScale/AuthoredDungeonProductionProfile.h"

namespace
{
EAuthoredRewardFamily FamilyForDungeon(const FName DungeonId)
{
    static const TSet<FName> Documentary = {
        TEXT("dungeon.rexa.drowned-plantation"),
        TEXT("dungeon.colonial.valdena-mine"),
        TEXT("dungeon.colonial.plague-vault"),
        TEXT("dungeon.colonial.company-works")
    };
    static const TSet<FName> Navigation = {
        TEXT("dungeon.rexa.green-throat"),
        TEXT("dungeon.rexa.hollow-city"),
        TEXT("dungeon.ashenmoor.singing-caverns"),
        TEXT("dungeon.sea.deep-harbour")
    };
    static const TSet<FName> Tactical = {
        TEXT("dungeon.colonial.siege-tunnels-ciudadela")
    };
    static const TSet<FName> Cultural = {
        TEXT("dungeon.rexa.cenote-first-mother"),
        TEXT("dungeon.rexa.root-temple-ixchel"),
        TEXT("dungeon.fjordlund.whale-bone-barrow"),
        TEXT("dungeon.fjordlund.drowned-mead-hall"),
        TEXT("dungeon.fjordlund.hall-under-glacier")
    };

    if (Documentary.Contains(DungeonId)) return EAuthoredRewardFamily::DocumentaryEvidence;
    if (Navigation.Contains(DungeonId)) return EAuthoredRewardFamily::NavigationalKnowledge;
    if (Tactical.Contains(DungeonId)) return EAuthoredRewardFamily::TacticalAccess;
    if (Cultural.Contains(DungeonId)) return EAuthoredRewardFamily::CulturalArtifact;
    return EAuthoredRewardFamily::DungeonOutcome;
}

EAuthoredTreasureClass TreasureClassForFamily(const EAuthoredRewardFamily Family)
{
    switch (Family)
    {
    case EAuthoredRewardFamily::DocumentaryEvidence:
    case EAuthoredRewardFamily::StateTreasure:
        return EAuthoredTreasureClass::Document;
    case EAuthoredRewardFamily::NavigationalKnowledge:
        return EAuthoredTreasureClass::Map;
    default:
        return EAuthoredTreasureClass::Object;
    }
}

bool CarriesCommunityOwnershipQuestion(const FName DungeonId)
{
    static const TSet<FName> CommunityOwned = {
        TEXT("dungeon.rexa.cenote-first-mother"),
        TEXT("dungeon.rexa.root-temple-ixchel"),
        TEXT("dungeon.fjordlund.whale-bone-barrow"),
        TEXT("dungeon.fjordlund.drowned-mead-hall"),
        TEXT("dungeon.fjordlund.hall-under-glacier"),
        TEXT("dungeon.rexa.grove-beneath-grove"),
        TEXT("dungeon.fjordlund.one-that-waited")
    };
    return CommunityOwned.Contains(DungeonId);
}

FAuthoredRewardBinding StateTreasure(
    const TCHAR* StableId,
    const TCHAR* SourceContentId,
    const TCHAR* DisplayName,
    const TCHAR* CastleId,
    const TCHAR* SourceDetail)
{
    FAuthoredRewardBinding Result;
    Result.StableId = StableId;
    Result.SourceContentId = SourceContentId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = TEXT("treasure system.md Section 4; castle catalog.md Section 13");
    Result.AuthoredOutcome = SourceDetail;
    Result.Family = EAuthoredRewardFamily::StateTreasure;
    Result.TreasureClass = EAuthoredTreasureClass::Document;
    Result.bIdentityAuthored = true;
    Result.OriginStableId = CastleId;
    return Result;
}
}

TArray<FAuthoredRewardBinding> FAuthoredRewardCatalog::BuildDungeonRewardBindings()
{
    TArray<FAuthoredRewardBinding> Results;
    const TArray<FAuthoredDungeonProductionProfile> Profiles = FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles();
    Results.Reserve(Profiles.Num());

    for (const FAuthoredDungeonProductionProfile& Profile : Profiles)
    {
        FAuthoredRewardBinding Reward;
        Reward.StableId = FName(*FString::Printf(TEXT("reward.%s"), *Profile.StableId.ToString()));
        Reward.SourceContentId = Profile.StableId;
        Reward.DisplayName = FString::Printf(TEXT("Authored outcome — %s"), *Profile.StableId.ToString());
        Reward.GoverningSource = Profile.GoverningSource;
        Reward.AuthoredOutcome = Profile.RewardDetail;
        Reward.Family = FamilyForDungeon(Profile.StableId);
        Reward.TreasureClass = TreasureClassForFamily(Reward.Family);
        Reward.bCommunityOwnershipQuestion = CarriesCommunityOwnershipQuestion(Profile.StableId);
        Reward.bCountsTowardReturnQuestion = Reward.bCommunityOwnershipQuestion;
        Reward.bExplicitlyNothing = Profile.bRewardExplicitlyNone;
        Reward.bWithheldOrUnresolved = Profile.bRewardWithheldOrUnresolved;
        Reward.bIdentityAuthored = Reward.bExplicitlyNothing
            || (!Reward.AuthoredOutcome.IsEmpty() && !Reward.bWithheldOrUnresolved);

        if (Reward.bExplicitlyNothing)
        {
            Reward.Family = EAuthoredRewardFamily::ExplicitNothing;
            Reward.bCountsTowardReturnQuestion = false;
        }
        Results.Add(MoveTemp(Reward));
    }

    return Results;
}

TArray<FAuthoredRewardBinding> FAuthoredRewardCatalog::BuildStateTreasureSlots()
{
    return {
        StateTreasure(
            TEXT("reward.state-treasure.conquest-archives-1651"),
            TEXT("state-treasure.conquest-archives-1651"),
            TEXT("The Conquest Archives, 1651"),
            TEXT("castle.la-ciudadela"),
            TEXT("The Conquest Archives, 1651, physically recovered from La Ciudadela.")),
        StateTreasure(
            TEXT("reward.state-treasure.labor-ledgers"),
            TEXT("state-treasure.labor-ledgers"),
            TEXT("The Labor Ledgers"),
            TEXT("castle.fuerte-esperanza"),
            TEXT("The Labor Ledgers, physically recovered from Fuerte Esperanza.")),
        StateTreasure(
            TEXT("reward.state-treasure.1846-patrol-reports"),
            TEXT("state-treasure.1846-patrol-reports"),
            TEXT("The 1846 Patrol Reports"),
            TEXT("castle.fuerte-san-rafael"),
            TEXT("The 1846 Patrol Reports, physically recovered from Fuerte San Rafael.")),
        StateTreasure(
            TEXT("reward.state-treasure.bribe-ledgers"),
            TEXT("state-treasure.bribe-ledgers"),
            TEXT("The Bribe Ledgers"),
            TEXT("castle.castillo-dorado"),
            TEXT("The Bribe Ledgers, physically recovered from Castillo Dorado.")),
        StateTreasure(
            TEXT("reward.state-treasure.vegas-eleven-years"),
            TEXT("state-treasure.vegas-eleven-years"),
            TEXT("Vega's Eleven Years"),
            TEXT("castle.puesto-del-norte"),
            TEXT("Vega's Eleven Years, physically recovered from Puesto del Norte.")),
        StateTreasure(
            TEXT("reward.state-treasure.master-ledger"),
            TEXT("state-treasure.master-ledger"),
            TEXT("The Master Ledger"),
            TEXT("castle.coventry-house"),
            TEXT("The Master Ledger, physically recovered from Coventry House.")),
        StateTreasure(
            TEXT("reward.state-treasure.sterlings-correspondence"),
            TEXT("state-treasure.sterlings-correspondence"),
            TEXT("Sterling's Correspondence"),
            TEXT("castle.sterling-bastion"),
            TEXT("Sterling's Correspondence from the Sterling Bastion. On the alliance path Sterling may hand it over herself; taking and being given it remain distinct authored acquisition routes.")),
        StateTreasure(
            TEXT("reward.state-treasure.four-thousand-contracts"),
            TEXT("state-treasure.four-thousand-contracts"),
            TEXT("The Four Thousand Contracts"),
            TEXT("castle.ashcroft-hall"),
            TEXT("The Four Thousand Contracts, physically recovered from Ashcroft Hall.")),
        StateTreasure(
            TEXT("reward.state-treasure.thornes-dispatches"),
            TEXT("state-treasure.thornes-dispatches"),
            TEXT("Thorne's Dispatches"),
            TEXT("castle.fort-resolute"),
            TEXT("Thorne's Dispatches, physically recovered from Fort Resolute."))
    };
}

TArray<FAuthoredRewardBinding> FAuthoredRewardCatalog::BuildNamedUniqueRewards()
{
    FAuthoredRewardBinding Katana;
    Katana.StableId = TEXT("reward.unique.crystal-katana");
    Katana.SourceContentId = TEXT("highmoore.crystal-guardian");
    Katana.DisplayName = TEXT("Crystal Katana");
    Katana.GoverningSource = TEXT("crystal guardian.md Section 7.3; crystal katana.md; CrystalKatanaComponent");
    Katana.AuthoredOutcome = TEXT("Physical 74 cm, 1.1 kg unique weapon obtained only after defeating the Crystal Guardian; bypassing the Guardian yields no Katana.");
    Katana.Family = EAuthoredRewardFamily::UniqueWeapon;
    Katana.TreasureClass = EAuthoredTreasureClass::Object;
    Katana.bIdentityAuthored = true;

    return { Katana };
}

TArray<FRewardProductionDesignGap> FAuthoredRewardCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.return-question-artifact-identities"),
            TEXT("Treasure canon says approximately forty artifacts ask sell/keep/return, but it does not provide forty individually reconciled identities in one authoritative list. The total is locked; absent identities are not fabricated."),
            TEXT("treasure system.md Sections 1.4 and 7")
        },
        {
            TEXT("design-gap.buried-hoard-identities"),
            TEXT("Sixteen buried-hoard chains are required — twelve across the archipelago and four in Highmoore — but this source does not individually name all sixteen chains. No X-marks-the-spot filler is generated."),
            TEXT("treasure system.md Section 6")
        },
        {
            TEXT("design-gap.individual-treasure-class-mix"),
            TEXT("The five class percentages are approximate corpus targets, not an authored per-item classification table. Individual treasure classes stay source-driven rather than being assigned merely to hit percentages."),
            TEXT("treasure system.md Sections 1-2")
        }
    };
}

bool FAuthoredRewardCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredRewardBinding> DungeonRewards = BuildDungeonRewardBindings();
    if (DungeonRewards.Num() != 40)
    {
        OutErrors.Add(FString::Printf(TEXT("Expected one reward/outcome binding for each of 40 grounded dungeon profiles; found %d."), DungeonRewards.Num()));
    }

    TSet<FName> Seen;
    for (const FAuthoredRewardBinding& Reward : DungeonRewards)
    {
        if (Reward.StableId.IsNone() || Reward.SourceContentId.IsNone() || Reward.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every dungeon reward binding needs stable identity, source content and governing source."));
        }
        if (Seen.Contains(Reward.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate reward binding id: %s"), *Reward.StableId.ToString()));
        }
        Seen.Add(Reward.StableId);

        if (Reward.bIdentityAuthored && Reward.AuthoredOutcome.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Authored reward identity has no authored outcome: %s"), *Reward.StableId.ToString()));
        }
        if (Reward.bExplicitlyNothing && Reward.Family != EAuthoredRewardFamily::ExplicitNothing)
        {
            OutErrors.Add(FString::Printf(TEXT("Explicit-nothing reward is not classified as such: %s"), *Reward.StableId.ToString()));
        }
        if (Reward.bWithheldOrUnresolved && Reward.bIdentityAuthored)
        {
            OutErrors.Add(FString::Printf(TEXT("Withheld reward may not be marked identity-authored: %s"), *Reward.StableId.ToString()));
        }
    }

    const TArray<FAuthoredRewardBinding> StateTreasures = BuildStateTreasureSlots();
    if (StateTreasures.Num() != StateTreasureSlotCount)
    {
        OutErrors.Add(TEXT("Exactly nine state treasures are required."));
    }
    for (const FAuthoredRewardBinding& Treasure : StateTreasures)
    {
        if (!Treasure.bIdentityAuthored || Treasure.bWithheldOrUnresolved
            || Treasure.Family != EAuthoredRewardFamily::StateTreasure
            || Treasure.TreasureClass != EAuthoredTreasureClass::Document
            || Treasure.DisplayName.IsEmpty() || Treasure.OriginStableId.IsNone())
        {
            OutErrors.Add(TEXT("Every state treasure must remain a named physical document bound to its authored castle."));
        }
        if (Seen.Contains(Treasure.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate state-treasure id: %s"), *Treasure.StableId.ToString()));
        }
        Seen.Add(Treasure.StableId);
    }

    const TArray<FAuthoredRewardBinding> UniqueRewards = BuildNamedUniqueRewards();
    if (UniqueRewards.Num() != 1 || UniqueRewards[0].DisplayName != TEXT("Crystal Katana") || !UniqueRewards[0].bIdentityAuthored)
    {
        OutErrors.Add(TEXT("Crystal Katana must remain the currently grounded named unique reward in this catalog."));
    }

    if (ApproxCoinPercent + ApproxMaterialPercent + ApproxDocumentPercent + ApproxMapPercent + ApproxObjectPercent != 100)
    {
        OutErrors.Add(TEXT("Approximate five-class treasure mix must sum to 100 percent."));
    }
    if (ArchipelagoBuriedHoardCount + HighmooreBuriedHoardCount != BuriedHoardCount)
    {
        OutErrors.Add(TEXT("Buried-hoard split must remain twelve archipelago plus four Highmoore = sixteen."));
    }
    if (AllowsRandomLootTables() || AllowsRarityColourCoding() || AllowsTreasureFoundCounterUI() || AllowsReturnedCountUI())
    {
        OutErrors.Add(TEXT("Treasure production may not gain loot tables, rarity colours or visible treasure/return counters."));
    }
    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Treasure production must preserve the three current identity/classification gaps."));
    }

    return OutErrors.IsEmpty();
}
