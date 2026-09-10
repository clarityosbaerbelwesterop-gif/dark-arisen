// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EProgressionContentReadiness : uint8
{
    Authored,
    Partial,
    BlockedByAuthority
};

struct FProgressionContentReadinessRecord
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    int32 RequiredCount = 0;
    int32 GroundedCount = 0;
    int32 ExplicitGapCount = 0;
    EProgressionContentReadiness Readiness = EProgressionContentReadiness::Partial;
    FString Boundary;
};

struct FProgressionContentDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-only readiness authority for the finite BODY/CRAFT/Marks content described by
 * skill tree.md. This catalog records what is actually authored and leaves missing
 * identities/placements unresolved rather than manufacturing progression filler.
 */
class DARKARISEN_API FProgressionContentReadinessCatalog
{
public:
    static constexpr int32 RequiredSkillNodeCount = 68;
    static constexpr int32 GroundedSkillNodeCount = 19;
    static constexpr int32 SkillNodeIdentityGapCount = 49;

    static constexpr int32 RequiredTeacherCount = 23;
    static constexpr int32 GroundedTeacherCount = 23;

    static constexpr int32 RequiredPhysiciansDraughtCount = 12;
    static constexpr int32 GroundedPhysiciansDraughtPlacementCount = 9;
    static constexpr int32 PhysiciansDraughtPlacementGapCount = 3;

    static constexpr int32 RequiredDeepWaterPearlCount = 16;
    static constexpr int32 GroundedDeepWaterPearlPlacementCount = 0;
    static constexpr int32 DeepWaterPearlPlacementGapCount = 16;

    static constexpr int32 RequiredPostureDeflectionSourceCount = 15;
    static constexpr int32 UncontestedTier1DeflectionSourceCount = 9;
    static constexpr int32 PostureDeflectionSelectionGapCount = 6;

    static constexpr int32 RequiredSpecificMarkActCount = 12;
    static constexpr int32 GroundedSpecificMarkActCount = 6;
    static constexpr int32 SpecificMarkActGapCount = 6;

    static constexpr int32 RequiredReadinessFamilyCount = 6;

    static TArray<FProgressionContentReadinessRecord> BuildRecords();
    static TArray<FProgressionContentDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
