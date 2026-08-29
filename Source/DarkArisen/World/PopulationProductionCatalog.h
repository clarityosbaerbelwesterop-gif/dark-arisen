// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EPopulationDensityArchetype : uint8
{
    MajorImperialCity,
    RegionalTown,
    Village,
    FrontierWilderness,
    PirateHaven,
    Absence
};

struct FPopulationDensityContract
{
    EPopulationDensityArchetype Archetype = EPopulationDensityArchetype::Village;
    FString DisplayName;
    int32 CentreVisibleMinimum = INDEX_NONE;
    int32 CentreVisibleMaximum = INDEX_NONE;
    int32 StreetVisibleMinimum = INDEX_NONE;
    int32 StreetVisibleMaximum = INDEX_NONE;
    FString GoverningSource;
};

struct FPopulationDemographicShare
{
    FString Group;
    int32 Percentage = INDEX_NONE;
};

struct FRegionalPopulationProductionProfile
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    TArray<FPopulationDemographicShare> Demographics;
    FString DailyRhythm;
    FString JakeReactionRule;
    bool bPopulationDefinedByAbsence = false;
    bool bDemographicPercentagesUnspecified = false;
    bool bHighDensityCity = false;
    bool bRuntimeCrowdAssetsAuthored = false;
    FString RuntimeCrowdAssetRoot;
};

struct FPopulationProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-backed production authority for civilian population presentation. It does not create NPCs,
 * procedural crowds, dialogue, quests or world-state outcomes. Existing settlement/war/social owners
 * remain authoritative; this catalog only tells production what a region must visibly communicate.
 */
class DARKARISEN_API FPopulationProductionCatalog
{
public:
    static constexpr int32 RequiredRegionalProfiles = 8;
    static constexpr int32 MinimumAmbientLinesPerMajorRegion = 200;
    static constexpr int32 CrowdReturnMinimumMinutes = 2;
    static constexpr int32 CrowdReturnMaximumMinutes = 5;
    static constexpr int32 PopulationTierCount = 3;
    static constexpr int32 HighmooreClassCount = 4;

    static TArray<FPopulationDensityContract> BuildDensityContracts();
    static TArray<FRegionalPopulationProductionProfile> BuildRegionalProfiles();
    static TArray<FPopulationProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsRandomAuthoredPopulationGeneration() { return false; }
    static bool RequiresPurposeDrivenPathing() { return true; }
    static bool RequiresChildEngineProtection() { return true; }
    static bool RequiresChapterEvolution() { return true; }
    static bool AllowsHighmooreReputationMeter() { return false; }
};
