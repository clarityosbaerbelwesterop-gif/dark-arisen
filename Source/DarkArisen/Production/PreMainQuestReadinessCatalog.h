// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Source/readiness accounting only. A ready count here never means a UE asset, compiled build or
 * runtime acceptance exists.
 */
struct FPreMainQuestReadinessRecord
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;

    /** Finite identities/records expected for this family at the current authority layer. */
    int32 RegisteredCount = 0;

    /** Fully source-grounded for the specific production purpose named by this record. */
    int32 ProductionReadyCount = 0;

    /** Useful bounded source exists, but final production would collapse an authored ambiguity. */
    int32 ReferenceOnlyCount = 0;

    /** Missing or conflicting authority deliberately left unresolved. */
    int32 ExplicitGapCount = 0;

    FString Boundary;
};

struct FPreMainQuestStopReason
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Finite checkpoint proving that pre-main-quest source families have been reconciled without
 * fabricating content. It intentionally stops before main-story mission implementation.
 */
class DARKARISEN_API FPreMainQuestReadinessCatalog
{
public:
    static constexpr int32 RequiredFamilyCount = 19;

    static constexpr int32 RequiredDungeonSites = 61;
    static constexpr int32 GroundedDungeonSites = 40;
    static constexpr int32 DungeonIdentityGaps = 21; // 20 minor + one named Region-06 identity.

    static constexpr int32 RequiredThreads = 17;
    static constexpr int32 GroundedThreads = 17;
    static constexpr int32 RequiredTurns = 132;
    static constexpr int32 GroundedTurns = 3;
    static constexpr int32 TurnIdentityGaps = 129;
    static constexpr int32 RequiredStandingVariants = 147;
    static constexpr int32 GroundedStandingVariants = 1;
    static constexpr int32 StandingIdentityGaps = 146;
    static constexpr int32 DeliberateMissionIdentityGapCount = TurnIdentityGaps + StandingIdentityGaps;

    static constexpr int32 RequiredTier1Bosses = 9;
    static constexpr int32 CurrentTier1FullLookGaps = 9;

    static constexpr int32 WorldRegionCount = 8;
    static constexpr int32 HighmooreAnchorCount = 12;
    static constexpr int32 StateTreasureCount = 9;
    static constexpr int32 NamedUniqueRewardCount = 1;

    /** Aggregate of the six finite progression-readiness families. */
    static constexpr int32 ProgressionFiniteRecordCount = 146;
    static constexpr int32 ProgressionGroundedRecordCount = 66;
    static constexpr int32 ProgressionExplicitGapCount = 80;

    static constexpr int32 RequiredBuriedHoardChainCount = 16;
    static constexpr int32 GroundedBuriedHoardChainCount = 0;
    static constexpr int32 BuriedHoardChainIdentityGaps = 16;

    static constexpr int32 MajorCharacterRecordCount = 9;
    static constexpr int32 MajorCharacterGeometryReady = 5;
    static constexpr int32 MajorCharacterReferenceOnly = 1;
    static constexpr int32 MajorCharacterBlocked = 3;

    static constexpr int32 ShipVisualRecordCount = 6;
    static constexpr int32 ShipGeometryReady = 5;
    static constexpr int32 ShipBlocked = 1;

    static constexpr int32 FaunaRecordCount = 24;
    static constexpr int32 FaunaGeometryReady = 16;
    static constexpr int32 FaunaReferenceOnly = 3;
    static constexpr int32 FaunaBlocked = 5;

    static constexpr int32 FloraRecordCount = 9;
    static constexpr int32 FloraGeometryReady = 7;
    static constexpr int32 FloraReferenceOnly = 1;
    static constexpr int32 FloraBlocked = 1;

    static constexpr int32 ThreeDRecordCount = 140;
    static constexpr int32 ThreeDGeometryReady = 105;
    static constexpr int32 ThreeDReferenceOnly = 14;
    static constexpr int32 ThreeDBlocked = 21;

    static constexpr int32 AnimationRequirementCount = 23;
    static constexpr int32 RequiredCutsceneCount = 19;
    static constexpr int32 ResolvedCutsceneCount = 14;
    static constexpr int32 UnresolvedFinalActCutsceneCount = 5;
    static constexpr int32 HiggsfieldPrevisBriefCount = AnimationRequirementCount + ResolvedCutsceneCount;

    static TArray<FPreMainQuestReadinessRecord> BuildRecords();
    static TArray<FPreMainQuestStopReason> BuildMainQuestStopReasons();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsMainQuestFabrication() { return false; }
    static bool AllowsGapCountsToBeClosedWithGeneratedContent() { return false; }
    static bool TreatsSourceReadinessAsRuntimeEvidence() { return false; }
};
