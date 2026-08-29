// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Missions/MissionScaleRequirements.h"

enum class EQuestCoverageKind : uint8
{
    Turn,
    Standing
};

enum class EQuestCoverageState : uint8
{
    Authored,
    ReservedUnauthored
};

/**
 * A coverage slot is bookkeeping only. Reserved slots deliberately carry no invented quest title,
 * person, place, dialogue, reward, outcome or narrative premise.
 */
struct FQuestCoverageSlot
{
    FName CoverageId;
    EQuestCoverageKind Kind = EQuestCoverageKind::Turn;
    EQuestCoverageState State = EQuestCoverageState::ReservedUnauthored;
    FName BucketId;
    FName AuthoredQuestId;
    FString GoverningSource;
};

/**
 * Canon-safe production ledger for the finite Phase-11 quest counts.
 *
 * Current source coverage:
 * - 132 Turns required, 3 individually authored, 129 reserved/unwritten.
 * - 147 Standing variants required, 1 individually authored, 146 reserved/unwritten.
 *
 * Reserved slots are intentionally neutral IDs. They are not quests and may not be surfaced to the
 * player or expanded by procedural/generative content merely to satisfy a count.
 */
class DARKARISEN_API FQuestCoverageRegister
{
public:
    static constexpr int32 RequiredTurns = 132;
    static constexpr int32 AuthoredTurns = 3;
    static constexpr int32 MissingTurns = RequiredTurns - AuthoredTurns;

    static constexpr int32 RequiredStanding = 147;
    static constexpr int32 AuthoredStanding = 1;
    static constexpr int32 MissingStanding = RequiredStanding - AuthoredStanding;

    static constexpr int32 TotalMissingQuestIdentities = MissingTurns + MissingStanding;

    static TArray<FQuestCoverageSlot> BuildCoverage();
    static TArray<FQuestCoverageSlot> BuildMissingCoverage();
    static bool Validate(TArray<FString>& OutErrors);
};
