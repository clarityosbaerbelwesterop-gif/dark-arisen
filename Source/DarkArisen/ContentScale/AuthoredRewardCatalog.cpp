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
        TEXT("dungeon.colonial.siege-tunnels-la-ciudadela")
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
        Reward.bCommunityOwnershipQuestion = CarriesCommunityOwnershipQuestion(Profile.StableId);
        Reward.bExplicitlyNothing = Profile.bRewardExplicitlyNone;
        Reward.bWithheldOrUnresolved = Profile.bRewardWithheldOrUnresolved;
        Reward.bIdentityAuthored = Reward.bExplicitlyNothing
            || (!Reward.AuthoredOutcome.IsEmpty() && !Reward.bWithheldOrUnresolved);

        if (Reward.bExplicitlyNothing)
        {
            Reward.Family = EAuthoredRewardFamily::ExplicitNothing;
        }
        Results.Add(MoveTemp(Reward));
    }

    return Results;
}

TArray<FAuthoredRewardBinding> FAuthoredRewardCatalog::BuildStateTreasureSlots()
{
    TArray<FAuthoredRewardBinding> Results;
    Results.Reserve(StateTreasureSlotCount);

    for (int32 Index = 0; Index < StateTreasureSlotCount; ++Index)
    {
        FAuthoredRewardBinding Slot;
        Slot.StableId = FName(*FString::Printf(TEXT("reward.state-treasure.slot-%02d"), Index + 1));
        Slot.SourceContentId = FName(*FString::Printf(TEXT("state-treasure.slot-%02d"), Index + 1));
        Slot.DisplayName = FString::Printf(TEXT("State Treasure authoring slot %02d"), Index + 1);
        Slot.GoverningSource = TEXT("cutscene catalog.md Insert I-2; ColonialWar/CastleSiegeComponent state-treasure recovery contract");
        Slot.AuthoredOutcome = TEXT("A state treasure must be a concrete physical authored object recovered through castle/siege content. This slot does not invent its identity.");
        Slot.Family = EAuthoredRewardFamily::StateTreasure;
        Slot.bIdentityAuthored = false;
        Slot.bWithheldOrUnresolved = true;
        Results.Add(MoveTemp(Slot));
    }
    return Results;
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
    Katana.bIdentityAuthored = true;

    return { Katana };
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
        OutErrors.Add(TEXT("Exactly nine state-treasure authoring slots are required."));
    }
    for (const FAuthoredRewardBinding& Treasure : StateTreasures)
    {
        if (Treasure.bIdentityAuthored || !Treasure.bWithheldOrUnresolved || Treasure.Family != EAuthoredRewardFamily::StateTreasure)
        {
            OutErrors.Add(TEXT("State-treasure slots must stay fail-closed until concrete identities are authored."));
        }
    }

    const TArray<FAuthoredRewardBinding> UniqueRewards = BuildNamedUniqueRewards();
    if (UniqueRewards.Num() != 1 || UniqueRewards[0].DisplayName != TEXT("Crystal Katana") || !UniqueRewards[0].bIdentityAuthored)
    {
        OutErrors.Add(TEXT("Crystal Katana must remain the currently grounded named unique reward in this catalog."));
    }

    return OutErrors.IsEmpty();
}
