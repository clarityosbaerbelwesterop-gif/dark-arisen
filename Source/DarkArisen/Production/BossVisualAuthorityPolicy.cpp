// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/BossVisualAuthorityPolicy.h"

namespace
{
const FName EthanBossVisualId(TEXT("boss-visual.ethan-harlow"));
const FName DravenBossVisualId(TEXT("boss-visual.draven-voss"));

bool IsCurrentStoryConflictId(const FName StableId)
{
    return StableId == EthanBossVisualId || StableId == DravenBossVisualId;
}
}

FBossVisualAuthorityDecision FBossVisualAuthorityPolicy::Evaluate(const FName BossVisualStableId)
{
    FBossVisualAuthorityDecision Result;
    Result.BossVisualStableId = BossVisualStableId;
    Result.GoverningSource = TEXT("Docs/DesignAuthority.md; Docs/M7_TIER1_BOSS_REGISTER.md");

    if (IsCurrentStoryConflictId(BossVisualStableId))
    {
        Result.State = EBossVisualAuthorityState::CurrentStoryConflict;
        Result.Reason = TEXT("The older Ethan/Draven boss-story branch conflicts with current Phase 11. It may be consulted historically but cannot enter provider production until rewritten or explicitly restored by the operator.");
        Result.bProviderEligible = false;
        return Result;
    }

    Result.State = EBossVisualAuthorityState::UncontestedLegacyReference;
    Result.Reason = TEXT("The deep-dive visual may be used as a bounded reference where no higher authority conflicts. It does not alter boss count, campaign placement, gameplay ownership or resolution contracts.");
    Result.bProviderEligible = true;
    return Result;
}

bool FBossVisualAuthorityPolicy::IsProviderEligible(const FName BossVisualStableId)
{
    return Evaluate(BossVisualStableId).bProviderEligible;
}

TArray<FName> FBossVisualAuthorityPolicy::BuildCurrentStoryConflictIds()
{
    return { EthanBossVisualId, DravenBossVisualId };
}

bool FBossVisualAuthorityPolicy::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FName> ConflictIds = BuildCurrentStoryConflictIds();
    if (ConflictIds.Num() != CurrentStoryConflictCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Boss visual authority requires exactly %d current-story conflicts; found %d."), CurrentStoryConflictCount, ConflictIds.Num()));
    }

    TSet<FName> Seen;
    for (const FName ConflictId : ConflictIds)
    {
        if (ConflictId.IsNone() || Seen.Contains(ConflictId))
        {
            OutErrors.Add(FString::Printf(TEXT("Invalid or duplicate boss visual authority conflict id: %s"), *ConflictId.ToString()));
            continue;
        }
        Seen.Add(ConflictId);

        const FBossVisualAuthorityDecision Decision = Evaluate(ConflictId);
        if (Decision.State != EBossVisualAuthorityState::CurrentStoryConflict
            || Decision.bProviderEligible
            || Decision.GoverningSource.IsEmpty()
            || Decision.Reason.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Boss visual current-story conflict %s is not fail-closed."), *ConflictId.ToString()));
        }
    }

    const FBossVisualAuthorityDecision NonConflict = Evaluate(TEXT("boss-visual.ashen-wyrm"));
    if (NonConflict.State != EBossVisualAuthorityState::UncontestedLegacyReference
        || !NonConflict.bProviderEligible)
    {
        OutErrors.Add(TEXT("Uncontested deep-dive visual references must remain provider-eligible without altering current boss authority."));
    }

    if (ProviderEligibleVisualIdentityCount != 19)
    {
        OutErrors.Add(TEXT("Boss visual provider-eligible identity count must remain nineteen until authority changes."));
    }

    return OutErrors.IsEmpty();
}