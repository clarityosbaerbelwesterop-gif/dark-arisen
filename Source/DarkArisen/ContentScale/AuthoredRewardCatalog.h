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

struct FAuthoredRewardBinding
{
    FName StableId;
    FName SourceContentId;
    FString DisplayName;
    FString GoverningSource;
    FString AuthoredOutcome;
    EAuthoredRewardFamily Family = EAuthoredRewardFamily::DungeonOutcome;

    /** True only when the concrete reward identity is actually supplied by canon. */
    bool bIdentityAuthored = false;

    /** Some rewards deliberately belong to living communities and therefore carry a take/keep/sell/return question. */
    bool bCommunityOwnershipQuestion = false;

    /** True for intentionally rewardless authored outcomes such as The First House. */
    bool bExplicitlyNothing = false;

    /** True where the source explicitly withholds or leaves the reward unresolved. */
    bool bWithheldOrUnresolved = false;
};

/**
 * Finite source-backed reward/treasure authority. This does not grant inventory items or money.
 * Existing dungeon, inventory, economy, quest and siege owners remain responsible for runtime state.
 */
class DARKARISEN_API FAuthoredRewardCatalog
{
public:
    static constexpr int32 StateTreasureSlotCount = 9;

    /** One reward/outcome binding for every currently grounded named dungeon production profile. */
    static TArray<FAuthoredRewardBinding> BuildDungeonRewardBindings();

    /** Nine canon-required state-treasure slots. Unnamed slots remain identity-unauthored. */
    static TArray<FAuthoredRewardBinding> BuildStateTreasureSlots();

    /** Individually named unique rewards already locked by narrow subsystem canon. */
    static TArray<FAuthoredRewardBinding> BuildNamedUniqueRewards();

    static bool Validate(TArray<FString>& OutErrors);
};
