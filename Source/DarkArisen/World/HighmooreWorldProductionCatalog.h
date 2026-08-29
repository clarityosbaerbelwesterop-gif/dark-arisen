// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EHighmooreWorldAnchorKind : uint8
{
    Castle,
    Village,
    SettlementGroup,
    MarketTown,
    Abbey,
    PublicHouse
};

struct FHighmooreWorldProductionAnchor
{
    FName StableId;
    FString DisplayName;
    EHighmooreWorldAnchorKind Kind = EHighmooreWorldAnchorKind::Village;
    FString SubRegion;
    FString GoverningSource;
    FString ProductionRead;

    /** False until a reviewed Unreal placement/asset exists. */
    bool bRuntimePlacementAuthored = false;
    FString RuntimeActorPath;
};

struct FHighmooreWorldDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/** Source-backed Highmoore world anchors; no coordinates or generic settlement filler are invented. */
class DARKARISEN_API FHighmooreWorldProductionCatalog
{
public:
    static constexpr int32 RequiredNamedAnchorCount = 12;
    static constexpr int32 MinimumVisibleCastlesFromEmergence = 6;
    static constexpr int32 EmergenceSwitchbackMinimumMinutesOnFoot = 8;
    static constexpr int32 EmergenceSwitchbackMaximumMinutesOnFoot = 10;
    static constexpr int32 ApproximateEndToEndCanterMinutes = 25;

    static TArray<FHighmooreWorldProductionAnchor> BuildNamedAnchors();
    static TArray<FHighmooreWorldDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsQuestionMarkDiscoveryMarkers() { return false; }
    static bool AllowsGenericCastleSubstitution() { return false; }
};
