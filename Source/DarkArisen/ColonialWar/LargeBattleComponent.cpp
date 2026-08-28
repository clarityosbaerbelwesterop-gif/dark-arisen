// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/LargeBattleComponent.h"

ULargeBattleComponent::ULargeBattleComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool ULargeBattleComponent::BeginBattle(const FName InBattleId, const ELargeBattleType InBattleType)
{
    if (InBattleId.IsNone() || bBattleActive)
    {
        return false;
    }

    BattleId = InBattleId;
    BattleType = InBattleType;
    bBattleActive = true;
    bJakeFallen = false;
    BattleOutcome = ELargeBattleOutcome::Unresolved;
    Segments.Reset();
    return true;
}

bool ULargeBattleComponent::RegisterSegment(const FBattleSegmentDefinition& Definition)
{
    if (!bBattleActive
        || Definition.SegmentId.IsNone()
        || Definition.FrontMetres <= 0.0f
        || Segments.Contains(Definition.SegmentId)
        || Segments.Num() >= MaximumSegments)
    {
        return false;
    }

    FBattleSegmentRuntimeState State;
    State.SegmentId = Definition.SegmentId;
    State.FrontMetres = Definition.FrontMetres;
    State.bOfficerPresent = Definition.bOfficerPresent;
    Segments.Add(State.SegmentId, State);
    return true;
}

bool ULargeBattleComponent::ApplySegmentCohesionDelta(const FName SegmentId, const int32 Delta)
{
    FBattleSegmentRuntimeState* Segment = Segments.Find(SegmentId);
    if (!bBattleActive || !Segment || Segment->bResolved || Delta == 0)
    {
        return false;
    }

    Segment->Cohesion = FMath::Clamp(Segment->Cohesion + Delta, 0, 100);
    if (Segment->Cohesion == 0)
    {
        Segment->bBroken = true;
        Segment->bResolved = true;
        Segment->CurrentOrder = EArmyOrder::BreakOff;
        Segment->bJakePresent = false;
    }
    return true;
}

bool ULargeBattleComponent::SetJakeSegment(const FName SegmentId)
{
    if (!bBattleActive || bJakeFallen)
    {
        return false;
    }

    FBattleSegmentRuntimeState* Target = Segments.Find(SegmentId);
    if (!Target || Target->bResolved)
    {
        return false;
    }

    for (TPair<FName, FBattleSegmentRuntimeState>& Pair : Segments)
    {
        Pair.Value.bJakePresent = false;
    }
    Target->bJakePresent = true;
    return true;
}

bool ULargeBattleComponent::SetOfficerPresent(const FName SegmentId, const bool bPresent)
{
    FBattleSegmentRuntimeState* Segment = Segments.Find(SegmentId);
    if (!bBattleActive || !Segment || Segment->bResolved)
    {
        return false;
    }
    Segment->bOfficerPresent = bPresent;
    return true;
}

bool ULargeBattleComponent::IssueSegmentOrder(const FName SegmentId, const EArmyOrder Order)
{
    FBattleSegmentRuntimeState* Segment = Segments.Find(SegmentId);
    if (!bBattleActive
        || BattleType == ELargeBattleType::Rising
        || !Segment
        || Segment->bResolved)
    {
        return false;
    }

    Segment->CurrentOrder = Order;
    return true;
}

bool ULargeBattleComponent::RecordSegmentResolved(const FName SegmentId, const bool bBroken)
{
    FBattleSegmentRuntimeState* Segment = Segments.Find(SegmentId);
    if (!bBattleActive || !Segment || Segment->bResolved)
    {
        return false;
    }

    Segment->bResolved = true;
    Segment->bBroken = bBroken;
    if (bBroken)
    {
        Segment->Cohesion = 0;
        Segment->CurrentOrder = EArmyOrder::BreakOff;
    }
    Segment->bJakePresent = false;
    return true;
}

bool ULargeBattleComponent::RecordJakeFallen()
{
    if (!bBattleActive || bJakeFallen)
    {
        return false;
    }

    bJakeFallen = true;
    for (TPair<FName, FBattleSegmentRuntimeState>& Pair : Segments)
    {
        Pair.Value.bJakePresent = false;
    }
    return true;
}

bool ULargeBattleComponent::ResolveBattle(const ELargeBattleOutcome Outcome)
{
    if (!bBattleActive
        || Outcome == ELargeBattleOutcome::Unresolved
        || Segments.Num() < MinimumSegments
        || !AreAllSegmentsResolved())
    {
        return false;
    }

    BattleOutcome = Outcome;
    bBattleActive = false;
    return true;
}

int32 ULargeBattleComponent::GetResolvedSegmentCount() const
{
    int32 Count = 0;
    for (const TPair<FName, FBattleSegmentRuntimeState>& Pair : Segments)
    {
        if (Pair.Value.bResolved)
        {
            ++Count;
        }
    }
    return Count;
}

int32 ULargeBattleComponent::GetHiddenCohesionForTests(const FName SegmentId) const
{
    const FBattleSegmentRuntimeState* Segment = Segments.Find(SegmentId);
    return Segment ? Segment->Cohesion : -1;
}

bool ULargeBattleComponent::AreAllSegmentsResolved() const
{
    if (Segments.Num() < MinimumSegments)
    {
        return false;
    }

    for (const TPair<FName, FBattleSegmentRuntimeState>& Pair : Segments)
    {
        if (!Pair.Value.bResolved)
        {
            return false;
        }
    }
    return true;
}
