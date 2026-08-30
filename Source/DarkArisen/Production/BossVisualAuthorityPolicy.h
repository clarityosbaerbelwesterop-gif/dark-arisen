// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EBossVisualAuthorityState : uint8
{
    UnknownIdentity,
    UncontestedLegacyReference,
    CurrentStoryConflict
};

struct FBossVisualAuthorityDecision
{
    FName BossVisualStableId;
    EBossVisualAuthorityState State = EBossVisualAuthorityState::UnknownIdentity;
    FString GoverningSource;
    FString Reason;
    bool bProviderEligible = false;
};

/**
 * Fail-closed reference eligibility for the finite twenty-one deep-dive boss visual identities.
 * "Provider" here is generic future reference/modeling eligibility; current Higgsfield routing is
 * separately restricted to animation/performance/cinematic previs by ExternalAssetProductionCatalog.
 */
class DARKARISEN_API FBossVisualAuthorityPolicy
{
public:
    static constexpr int32 DeepDiveVisualIdentityCount = 21;
    static constexpr int32 CurrentStoryConflictCount = 2;
    static constexpr int32 ProviderEligibleVisualIdentityCount = 19;

    static FBossVisualAuthorityDecision Evaluate(FName BossVisualStableId);
    static bool IsProviderEligible(FName BossVisualStableId);
    static TArray<FName> BuildCurrentStoryConflictIds();
    static bool Validate(TArray<FString>& OutErrors);
};
