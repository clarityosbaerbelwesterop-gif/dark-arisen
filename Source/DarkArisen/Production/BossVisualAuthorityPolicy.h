// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Authority state for the Phase-1/2 deep-dive boss visual corpus.
 * Older boss documents may remain useful for imagery, but current-story conflicts must not be
 * promoted into provider production simply because a detailed legacy visual sheet exists.
 */
enum class EBossVisualAuthorityState : uint8
{
    UncontestedLegacyReference,
    CurrentStoryConflict
};

struct FBossVisualAuthorityDecision
{
    FName BossVisualStableId;
    EBossVisualAuthorityState State = EBossVisualAuthorityState::UncontestedLegacyReference;
    FString GoverningSource;
    FString Reason;
    bool bProviderEligible = false;
};

class DARKARISEN_API FBossVisualAuthorityPolicy
{
public:
    static constexpr int32 DeepDiveVisualIdentityCount = 21;
    static constexpr int32 CurrentStoryConflictCount = 2;
    static constexpr int32 ProviderEligibleVisualIdentityCount =
        DeepDiveVisualIdentityCount - CurrentStoryConflictCount;

    static FBossVisualAuthorityDecision Evaluate(FName BossVisualStableId);
    static bool IsProviderEligible(FName BossVisualStableId);
    static TArray<FName> BuildCurrentStoryConflictIds();
    static bool Validate(TArray<FString>& OutErrors);
};
