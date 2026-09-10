// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FTier1BossVisualReadiness
{
    FName BossStableId;
    FString DisplayName;
    FString GoverningSource;
    FString AuthoredAgeRole;
    FString AuthoredWeaponSilhouette;
    FString AuthoredPerformanceRead;
    FString AuthoredArenaRead;
    FString MissingPhysicalAuthority;
    bool bCurrentTier1Authority = true;
    bool bProviderFullLookReady = false;
};

struct FTier1BossVisualDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Current Phase-7 Tier-1 visual readiness. This deliberately does not reuse old Phase-1/2 boss
 * faces as current Nine-Who-Hold identities and does not change combat/resolution ownership.
 */
class DARKARISEN_API FTier1BossVisualReadinessCatalog
{
public:
    static constexpr int32 RequiredTier1BossCount = 9;
    static constexpr int32 ProviderFullLookReadyCount = 0;
    static constexpr int32 ProviderBlockedFullLookCount = RequiredTier1BossCount;

    static TArray<FTier1BossVisualReadiness> BuildAll();
    static TArray<FTier1BossVisualDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};