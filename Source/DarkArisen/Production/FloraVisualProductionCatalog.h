// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EFloraVisualProductionKind : uint8
{
    LegendaryPlant,
    LegendaryTree,
    Region06ImpossibleTree,
    StoryGrove
};

struct FFloraVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    EFloraVisualProductionKind Kind = EFloraVisualProductionKind::LegendaryPlant;
    FString GoverningSource;
    FString AuthoredVisualFacts;
    FString BehaviorOrPhenomenonRead;
    FString EnvironmentRead;
    FString AntiInventionBoundary;
    bool bProviderReferenceReady = false;
    bool bApprovedReferenceExists = false;
    FString ApprovedReferencePath;
};

struct FFloraVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-only visual authority for the individually described legendary plants/trees and the
 * story-grove record. It does not synthesize the broad Phase-4 flora target counts or runtime
 * vegetation assets.
 */
class DARKARISEN_API FFloraVisualProductionCatalog
{
public:
    static constexpr int32 RequiredBriefCount = 9;
    static constexpr int32 ProviderReadyBriefCount = 8;
    static constexpr int32 ProviderBlockedBriefCount = 1;

    static TArray<FFloraVisualProductionBrief> BuildBriefs();
    static TArray<FFloraVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
