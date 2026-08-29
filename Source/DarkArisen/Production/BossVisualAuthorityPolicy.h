// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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

/** Fail-closed provider eligibility for the twenty-one deep-dive boss visual identities. */
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