// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EAuthoredTravelMode : uint8
{
    SeaPassage,
    FearedWatersGuide,
    CrystalCavesFirstPassage,
    CrystalCavesRepeatPassage,
    HorseStableRelocation,
    SettlementWalking
};

struct FAuthoredTravelProductionRoute
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    FString RuntimeOwner;
    EAuthoredTravelMode Mode = EAuthoredTravelMode::SeaPassage;
    bool bMovesJake = true;
    bool bMovesHorseOnly = false;
    bool bRequiresPhysicalTraversal = true;
    bool bRequiresGuideOrTrust = false;
    bool bMenuTravelPermitted = false;
    int32 MinimumRealMinutes = INDEX_NONE;
    int32 MaximumRealMinutes = INDEX_NONE;
};

struct FAuthoredTravelDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

class DARKARISEN_API FAuthoredTravelProductionCatalog
{
public:
    static constexpr int32 RequiredCoreRouteCount = 6;
    static constexpr int32 FearedWaterRegionCount = 6;
    static constexpr int32 CrystalFirstPassageMinimumRealMinutes = 90;
    static constexpr int32 CrystalFirstPassageMaximumRealMinutes = 120;
    static constexpr int32 CrystalRepeatPassageMinimumRealMinutes = 4;
    static constexpr int32 CrystalRepeatPassageMaximumRealMinutes = 6;

    static TArray<FAuthoredTravelProductionRoute> BuildCoreRoutes();
    static TArray<FAuthoredTravelDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsInstantWaterTravel() { return false; }
    static bool AllowsMapClickMovement() { return false; }
    static bool AllowsCompass() { return false; }
    static bool AllowsMiniMapDisplay() { return false; }
    static bool AllowsPlayerDot() { return false; }
};
