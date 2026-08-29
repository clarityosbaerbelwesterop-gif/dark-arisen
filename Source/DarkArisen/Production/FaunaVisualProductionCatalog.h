// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EFaunaVisualProductionKind : uint8
{
    LegendaryLandAnimal,
    LegendarySeaCreature,
    LegendaryBird,
    LegendarySmallCreature,
    HighmooreMvpAnimal
};

struct FFaunaVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    EFaunaVisualProductionKind Kind = EFaunaVisualProductionKind::LegendaryLandAnimal;
    FString GoverningSource;
    FString AuthoredVisualFacts;
    FString BehaviorAndMotionRead;
    FString EnvironmentRead;
    FString AntiInventionBoundary;

    /** True only when a bounded visual reference can be produced without choosing missing canon. */
    bool bProviderReferenceReady = false;

    /** External generation/review is separate evidence and starts false/empty. */
    bool bApprovedReferenceExists = false;
    FString ApprovedReferencePath;
};

struct FFaunaVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-only visual production authority for individually described fauna before Unreal assets.
 * This is not a spawn table, AI register, hunt implementation or permission to synthesize the
 * Phase-4 target species count.
 */
class DARKARISEN_API FFaunaVisualProductionCatalog
{
public:
    static constexpr int32 LegendaryCreatureBriefCount = 19;
    static constexpr int32 HighmooreMvpBriefCount = 5;
    static constexpr int32 RequiredBriefCount = LegendaryCreatureBriefCount + HighmooreMvpBriefCount;

    static constexpr int32 ProviderReadyLegendaryCount = 18;
    static constexpr int32 ProviderReadyHighmooreMvpCount = 1;
    static constexpr int32 ProviderReadyBriefCount = ProviderReadyLegendaryCount + ProviderReadyHighmooreMvpCount;
    static constexpr int32 ProviderBlockedBriefCount = RequiredBriefCount - ProviderReadyBriefCount;

    static TArray<FFaunaVisualProductionBrief> BuildLegendaryCreatureBriefs();
    static TArray<FFaunaVisualProductionBrief> BuildHighmooreMvpBriefs();
    static TArray<FFaunaVisualProductionBrief> BuildAllBriefs();
    static TArray<FFaunaVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
