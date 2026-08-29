// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class ECreatureVisualFamily : uint8
{
    LandLegendary,
    SeaLegendary,
    BirdLegendary,
    SmallLegendary,
    HighmooreMoorland
};

struct FCreatureVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    ECreatureVisualFamily Family = ECreatureVisualFamily::LandLegendary;
    FString GoverningSource;
    FString VisualFacts;
    FString BehaviorRead;
    FString AntiInventionBoundary;
    FName ExistingVisualDependency;
    bool bProviderReferenceReady = false;
    bool bCombatCreature = false;
    bool bMustRemainNonCombat = false;
};

struct FCreatureVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Visual production authority for source-grounded non-boss creatures that benefit from bespoke
 * reference work. Real-world common fauna can continue to use biological reference; this catalog
 * exists for distinctive Dark Arisen silhouettes and the Highmoore MVP ecology.
 */
class DARKARISEN_API FCreatureVisualProductionCatalog
{
public:
    static constexpr int32 RequiredBriefCount = 30;
    static constexpr int32 ProviderReadyBriefCount = 28;
    static constexpr int32 ProviderBlockedBriefCount = 2;

    static TArray<FCreatureVisualProductionBrief> BuildBriefs();
    static TArray<FCreatureVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};