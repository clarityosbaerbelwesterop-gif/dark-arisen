// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContentScale/AuthoredDungeonCatalog.h"

/**
 * Production-facing content packet for a named dungeon whose identity already exists in canon.
 * Strings are authored data summaries, not generated runtime narrative.
 */
struct FAuthoredDungeonProductionProfile
{
    FName StableId;
    FString GoverningSource;

    FString AccessAndDiscovery;
    FString PuzzleLanguageDetail;
    FString HazardDetail;
    FString UnforgettableImage;
    FString BossOrBottomDetail;
    FString RewardDetail;

    bool bImageWithheld = false;
    bool bBossWithheldOrUnresolved = false;
    bool bRewardWithheldOrUnresolved = false;
    bool bRewardExplicitlyNone = false;
    bool bWarReactive = false;
    bool bSupportsExplicitNonCombatResolution = false;
};

/**
 * Detailed C++ production authority for the forty currently grounded named sites.
 *
 * This layer does not synthesize the unresolved Region-06 slot, minor-site identities or Crystal
 * Caves. Crystal Caves remains governed by its dedicated Highmoore C++ components.
 */
class DARKARISEN_API FAuthoredDungeonProductionProfiles
{
public:
    static TArray<FAuthoredDungeonProductionProfile> BuildAllKnownProfiles();
    static bool TryGetProfile(FName StableId, FAuthoredDungeonProductionProfile& OutProfile);
    static bool ValidateProfiles(TArray<FString>& OutErrors);

private:
    static void AppendRexaMoran(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendFjordlund(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendAshenmoor(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendSea(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendColonial(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendRegion06AndCrossNetwork(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
    static void AppendHighmoore(TArray<FAuthoredDungeonProductionProfile>& OutProfiles);
};
