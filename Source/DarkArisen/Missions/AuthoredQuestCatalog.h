// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/QuestJournalComponent.h"

enum class EAuthoredThreadRegion : uint8
{
    Crew,
    Archipelago,
    Highmoore,
    CrossRegional
};

enum class EThreadActivationRead : uint8
{
    Conversation,
    Overheard,
    Absence,
    Object,
    SpecifiedElsewhere,
    Unresolved
};

struct FAuthoredThreadCatalogEntry
{
    FName StableId;
    FString DisplayName;
    EAuthoredThreadRegion Region = EAuthoredThreadRegion::CrossRegional;
    float AuthoredMinimumHours = 0.0f;
    float AuthoredMaximumHours = 0.0f;
    EThreadActivationRead Activation = EThreadActivationRead::Unresolved;
    FString ActivationDetail;
    FString GoverningSource;
    bool bFullySpecifiedElsewhere = false;
    bool bExpiresByWorldState = false;
    FString ExpirationCondition;
    int32 MarkReward = 0;
};

struct FTurnDistributionRequirement
{
    FName RegionId;
    int32 RequiredCount = 0;
};

struct FTurnShapeRequirement
{
    FName ShapeId;
    int32 ApproximateCount = 0;
};

struct FKnownMissionIdentity
{
    FName QuestId;
    FName AuthoredVariantId;
    FString GoverningSource;
    bool bStanding = false;
};

struct FQuestCatalogDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-backed Phase-11 quest catalog. It locks the seventeen named Threads and the distribution
 * contracts without synthesizing the missing Turn/Standing stories.
 */
class DARKARISEN_API FAuthoredQuestCatalog
{
public:
    static constexpr int32 RequiredThreads = 17;
    static constexpr int32 RequiredTurns = 132;
    static constexpr int32 RequiredStandingVariants = 147;
    static constexpr int32 IndividuallyAuthoredTurnIdentitiesInSource = 3;
    static constexpr int32 IndividuallyAuthoredStandingIdentitiesInSource = 1;

    static TArray<FAuthoredThreadCatalogEntry> BuildThreads();
    static TArray<FTurnDistributionRequirement> BuildTurnDistribution();
    static TArray<FTurnShapeRequirement> BuildTurnShapeRequirements();
    static TArray<FKnownMissionIdentity> BuildKnownMissionIdentities();
    static TArray<FQuestCatalogDesignGap> BuildDesignGaps();
    static bool ValidateCatalog(TArray<FString>& OutErrors);
};
