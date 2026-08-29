// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EAuthoredWorldRegion : uint8
{
    RexaMoran,
    Fjordlund,
    Ashenmoor,
    QuietCoast,
    PaleIsleSilvera,
    OpenSea,
    Highmoore,
    Region06
};

struct FAuthoredWorldRegionDefinition
{
    EAuthoredWorldRegion Region = EAuthoredWorldRegion::RexaMoran;
    FName StableId;
    FString DisplayName;
    FString GoverningSource;

    /** Region families that must be represented in authored level/content production. */
    bool bHasSettlements = false;
    bool bHasDungeonContent = false;
    bool bHasWarContent = false;
    bool bHasNavalContent = false;
    bool bHasHouseContent = false;

    /**
     * Deliberately false until an actual UE map asset is authored/imported and reviewed. Source code
     * cannot turn a region design document into a real .umap by claiming a path that does not exist.
     */
    bool bRuntimeMapAuthored = false;
    FString RuntimeMapPackage;
};

struct FWorldRegionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/** Source-backed production identities for the eight world/quest regions used by the Alpha corpus. */
class DARKARISEN_API FAuthoredWorldRegionRegistry
{
public:
    static constexpr int32 RequiredRegionCount = 8;

    static TArray<FAuthoredWorldRegionDefinition> BuildAll();
    static bool TryGet(FName StableId, FAuthoredWorldRegionDefinition& OutDefinition);
    static TArray<FWorldRegionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
