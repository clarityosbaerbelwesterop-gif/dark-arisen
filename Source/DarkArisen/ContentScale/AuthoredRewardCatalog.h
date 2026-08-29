// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EAuthoredRewardFamily : uint8
{
    DungeonOutcome,
    StateTreasure,
    UniqueWeapon,
    DocumentaryEvidence,
    CulturalArtifact,
    NavigationalKnowledge,
    TacticalAccess,
    ExplicitNothing
};

/** The five treasure classes from treasure system.md Section 2. */
enum class EAuthoredTreasureClass : uint8
{
    Coin,
    Material,
    Document,
    Map,
    Object
};

struct FAuthoredRewardBinding
{
    FName StableId;
    FName SourceContentId;
    FString DisplayName;
    FString GoverningSource;
    FString AuthoredOutcome;
    EAuthoredRewardFamily Family = EAuthoredRewardFamily::DungeonOutcome;
    EAuthoredTreasureClass TreasureClass = EAuthoredTreasureClass::Object;

    /** True only when the concrete reward identity is actually supplied by canon. */
    bool bIdentityAuthored = false;

    /** Some rewards deliberately belong to living communities and therefore carry a take/keep/sell/return question. */
    bool bCommunityOwnershipQuestion = false;

    /** True when this physical artifact contributes to the hidden cumulative return-vs-nonreturn lock. */
    bool bCountsTowardReturnQuestion = false;

    /** Authored place/castle/community of origin where the source provides one. */
    FName OriginStableId;

    /** True for intentionally rewardless authored outcomes such as The First House. */
    bool bExplicitlyNothing = false;

    /** True where the source explicitly withholds or leaves the reward unresolved. */
    bool bWithheldOrUnresolved = false;
};

struct FRewardProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Finite source-backed reward/treasure authority. This does not grant inventory items or money.
 * Existing dungeon, inventory, economy, quest, Alliance and siege owners remain responsible for runtime state.
 */
class DARKARISEN_API FAuthoredRewardCatalog
{
public:
    static constexpr int32 StateTreasureSlotCount = 9;
    static constexpr int32 ReturnQuestionArtifactCount = 40;
    static constexpr int32 BuriedHoardCount = 16;
    static constexpr int32 ArchipelagoBuriedHoardCount = 12;
    static constexpr int32 HighmooreBuriedHoardCount = 4;

    // Approximate corpus mix locked by treasure system.md Section 2. These are production targets,
    // not permission to fabricate missing individual treasures.
    static constexpr int32 ApproxCoinPercent = 15;
    static constexpr int32 ApproxMaterialPercent = 30;
    static constexpr int32 ApproxDocumentPercent = 20;
    static constexpr int32 ApproxMapPercent = 15;
    static constexpr int32 ApproxObjectPercent = 20;

    /** One reward/outcome binding for every currently grounded named dungeon production profile. */
    static TArray<FAuthoredRewardBinding> BuildDungeonRewardBindings();

    /** The exact nine named state treasures and their castles from treasure system.md Section 4. */
    static TArray<FAuthoredRewardBinding> BuildStateTreasureSlots();

    /** Individually named unique rewards already locked by narrow subsystem canon. */
    static TArray<FAuthoredRewardBinding> BuildNamedUniqueRewards();

    static TArray<FRewardProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsRandomLootTables() { return false; }
    static bool AllowsRarityColourCoding() { return false; }
    static bool AllowsTreasureFoundCounterUI() { return false; }
    static bool AllowsReturnedCountUI() { return false; }
};
