// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/RetaliationSubsystem.h"

bool URetaliationSubsystem::RecordFactionDamage(const EColonialFaction Faction, const int32 HeatAmount)
{
    if (!IsRetaliatingEmpire(Faction) || HeatAmount <= 0)
    {
        return false;
    }

    int32& Heat = ResolveHeatMutable(Faction);
    Heat = FMath::Clamp(Heat + HeatAmount, 0, 100);
    if (Faction == EColonialFaction::Imperial)
    {
        bImperialAttackedThisChapter = true;
    }
    else
    {
        bAlbionAttackedThisChapter = true;
    }
    return true;
}

bool URetaliationSubsystem::RecordDeEscalation(const EColonialFaction Faction, const int32 HeatReduction)
{
    if (!IsRetaliatingEmpire(Faction) || HeatReduction <= 0)
    {
        return false;
    }

    int32& Heat = ResolveHeatMutable(Faction);
    Heat = FMath::Clamp(Heat - HeatReduction, 0, 100);
    return true;
}

void URetaliationSubsystem::AdvanceChapter(const int32 NewChapter)
{
    if (NewChapter <= CurrentChapter)
    {
        return;
    }

    if (!bImperialAttackedThisChapter)
    {
        ImperialHeat = FMath::Max(0, ImperialHeat - PassiveChapterDecay);
    }
    if (!bAlbionAttackedThisChapter)
    {
        AlbionHeat = FMath::Max(0, AlbionHeat - PassiveChapterDecay);
    }

    bImperialAttackedThisChapter = false;
    bAlbionAttackedThisChapter = false;
    CurrentChapter = NewChapter;
}

ERetaliationStage URetaliationSubsystem::GetCurrentStage(const EColonialFaction Faction) const
{
    if (!IsRetaliatingEmpire(Faction))
    {
        return ERetaliationStage::Unnoticed;
    }
    return CapStageForChapter(StageFromHeat(ResolveHeat(Faction)), CurrentChapter);
}

bool URetaliationSubsystem::RegisterHostageCandidate(const FHostageCandidate& Candidate)
{
    if (Candidate.PersonId.IsNone()
        || Candidate.RegionId.IsNone()
        || Candidate.AttachmentScore < 0
        || Candidate.RippleWeight < 0
        || HostageCandidates.Contains(Candidate.PersonId))
    {
        return false;
    }

    HostageCandidates.Add(Candidate.PersonId, Candidate);
    return true;
}

TArray<FName> URetaliationSubsystem::SelectHostages(
    const EColonialFaction Faction,
    const FName ReachableRegionId,
    const int32 MaxCount) const
{
    TArray<FName> Result;
    if (!IsRetaliatingEmpire(Faction)
        || ReachableRegionId.IsNone()
        || MaxCount <= 0
        || GetCurrentStage(Faction) < ERetaliationStage::Hostages)
    {
        return Result;
    }

    struct FWeightedCandidate
    {
        FName Id;
        int32 Score = 0;
    };
    TArray<FWeightedCandidate> Weighted;

    for (const TPair<FName, FHostageCandidate>& Pair : HostageCandidates)
    {
        const FHostageCandidate& Candidate = Pair.Value;
        if (!Candidate.bAlive
            || Candidate.bChild
            || Candidate.bNarrativeProtected
            || Candidate.RegionId != ReachableRegionId)
        {
            continue;
        }

        FWeightedCandidate Entry;
        Entry.Id = Candidate.PersonId;
        Entry.Score = Candidate.AttachmentScore * 100 + Candidate.RippleWeight;
        Weighted.Add(Entry);
    }

    Weighted.Sort([](const FWeightedCandidate& A, const FWeightedCandidate& B)
    {
        if (A.Score != B.Score)
        {
            return A.Score > B.Score;
        }
        return A.Id.LexicalLess(B.Id);
    });

    const int32 Count = FMath::Min(MaxCount, Weighted.Num());
    for (int32 Index = 0; Index < Count; ++Index)
    {
        Result.Add(Weighted[Index].Id);
    }
    return Result;
}

bool URetaliationSubsystem::ResolveHostageLoss(const FName PersonId)
{
    FHostageCandidate* Candidate = HostageCandidates.Find(PersonId);
    if (!Candidate || Candidate->bChild || Candidate->bNarrativeProtected || !Candidate->bAlive)
    {
        return false;
    }

    Candidate->bAlive = false;
    return true;
}

bool URetaliationSubsystem::IsCandidateEligibleForTests(const FName PersonId) const
{
    const FHostageCandidate* Candidate = HostageCandidates.Find(PersonId);
    return Candidate
        && Candidate->bAlive
        && !Candidate->bChild
        && !Candidate->bNarrativeProtected;
}

int32 URetaliationSubsystem::GetHiddenHeatForTests(const EColonialFaction Faction) const
{
    return IsRetaliatingEmpire(Faction) ? ResolveHeat(Faction) : 0;
}

bool URetaliationSubsystem::IsRetaliatingEmpire(const EColonialFaction Faction)
{
    return Faction == EColonialFaction::Imperial || Faction == EColonialFaction::Albion;
}

ERetaliationStage URetaliationSubsystem::StageFromHeat(const int32 Heat)
{
    if (Heat >= 90) return ERetaliationStage::ScorchedEarth;
    if (Heat >= 75) return ERetaliationStage::Hostages;
    if (Heat >= 60) return ERetaliationStage::CounterAttack;
    if (Heat >= 40) return ERetaliationStage::Bounty;
    if (Heat >= 20) return ERetaliationStage::Letter;
    return ERetaliationStage::Unnoticed;
}

ERetaliationStage URetaliationSubsystem::CapStageForChapter(
    const ERetaliationStage Stage,
    const int32 Chapter)
{
    const ERetaliationStage Cap = Chapter <= 6
        ? ERetaliationStage::Bounty
        : (Chapter <= 8 ? ERetaliationStage::Hostages : ERetaliationStage::ScorchedEarth);
    return static_cast<uint8>(Stage) > static_cast<uint8>(Cap) ? Cap : Stage;
}

int32& URetaliationSubsystem::ResolveHeatMutable(const EColonialFaction Faction)
{
    return Faction == EColonialFaction::Albion ? AlbionHeat : ImperialHeat;
}

const int32& URetaliationSubsystem::ResolveHeat(const EColonialFaction Faction) const
{
    return Faction == EColonialFaction::Albion ? AlbionHeat : ImperialHeat;
}

bool URetaliationSubsystem::WasAttackedThisChapter(const EColonialFaction Faction) const
{
    return Faction == EColonialFaction::Albion ? bAlbionAttackedThisChapter : bImperialAttackedThisChapter;
}
