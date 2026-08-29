// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/TreasureStateComponent.h"

bool UTreasureStateComponent::RegisterArtifact(
    const FName ArtifactId,
    const bool bCountsTowardReturnQuestion)
{
    if (ArtifactId.IsNone())
    {
        return false;
    }

    if (!DispositionByArtifact.Contains(ArtifactId))
    {
        DispositionByArtifact.Add(ArtifactId, ETreasureDisposition::Unresolved);
    }

    if (bCountsTowardReturnQuestion)
    {
        ReturnQuestionArtifactIds.AddUnique(ArtifactId);
    }
    return true;
}

FTreasureDecisionResult UTreasureStateComponent::ResolveArtifact(
    const FName ArtifactId,
    const ETreasureDisposition Disposition)
{
    FTreasureDecisionResult Result;
    if (ArtifactId.IsNone() || Disposition == ETreasureDisposition::Unresolved)
    {
        return Result;
    }

    ETreasureDisposition* Existing = DispositionByArtifact.Find(ArtifactId);
    if (!Existing || *Existing != ETreasureDisposition::Unresolved)
    {
        return Result;
    }

    *Existing = Disposition;
    Result.bAccepted = true;
    if (Disposition == ETreasureDisposition::Returned
        && ReturnQuestionArtifactIds.Contains(ArtifactId))
    {
        Result.NetworkStrengthDelta = ReturnedArtifactNetworkStrengthDelta;
    }
    Result.bRecoveryReturnThresholdSatisfied = IsRecoveryReturnThresholdSatisfied();
    return Result;
}

bool UTreasureStateComponent::HasResolvedArtifact(const FName ArtifactId) const
{
    const ETreasureDisposition* Existing = DispositionByArtifact.Find(ArtifactId);
    return Existing && *Existing != ETreasureDisposition::Unresolved;
}

bool UTreasureStateComponent::IsRecoveryReturnThresholdSatisfied() const
{
    int32 Returned = 0;
    int32 NonReturned = 0;

    for (const FName ArtifactId : ReturnQuestionArtifactIds)
    {
        const ETreasureDisposition* Disposition = DispositionByArtifact.Find(ArtifactId);
        if (!Disposition || *Disposition == ETreasureDisposition::Unresolved)
        {
            continue;
        }

        if (*Disposition == ETreasureDisposition::Returned)
        {
            ++Returned;
        }
        else
        {
            // Kept and sold/fenced are both resolved choices where the object was not given back.
            ++NonReturned;
        }
    }

    return Returned > 0 && Returned > NonReturned;
}
