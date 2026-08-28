// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/ColonialWarStateSubsystem.h"

bool UColonialWarStateSubsystem::RegisterRegion(const FName RegionId, const EColonialFaction InitialController)
{
    if (RegionId.IsNone() || Regions.Contains(RegionId))
    {
        return false;
    }

    FColonialRegionState State;
    State.RegionId = RegionId;
    if (InitialController == EColonialFaction::Imperial)
    {
        State.ImperialControl = 70;
        State.AlbionControl = 20;
    }
    else if (InitialController == EColonialFaction::Albion)
    {
        State.ImperialControl = 20;
        State.AlbionControl = 70;
    }
    else if (InitialController == EColonialFaction::Liberation)
    {
        State.LiberationStrength = 60;
    }
    else
    {
        State.CrimsonThreat = 60;
    }

    ReevaluateRegion(State);
    Regions.Add(RegionId, State);
    LastAutonomousTickChapter.Add(RegionId, CurrentChapter - 1);
    return true;
}

bool UColonialWarStateSubsystem::RecordResolvedWarAction(
    const FName RegionId,
    const EWarActionVerb Verb,
    const EColonialFaction TargetFaction,
    const int32 ControlDelta,
    const int32 LiberationDelta,
    const int32 CrimsonDelta)
{
    FColonialRegionState* Region = Regions.Find(RegionId);
    if (!Region || ControlDelta < 0 || LiberationDelta < 0 || CrimsonDelta < 0)
    {
        return false;
    }

    int32& Target = ResolveFactionControl(*Region, TargetFaction);
    if (Verb == EWarActionVerb::Support)
    {
        Target = ClampControl(Target + ControlDelta);
    }
    else if (Verb == EWarActionVerb::Break)
    {
        Target = ClampControl(Target - ControlDelta);
    }
    else
    {
        Region->LiberationStrength = ClampControl(Region->LiberationStrength + LiberationDelta);
    }

    if (Verb != EWarActionVerb::Liberate && LiberationDelta > 0)
    {
        Region->LiberationStrength = ClampControl(Region->LiberationStrength + LiberationDelta);
    }
    Region->CrimsonThreat = ClampControl(Region->CrimsonThreat + CrimsonDelta);

    ReevaluateRegion(*Region);
    return true;
}

bool UColonialWarStateSubsystem::RecordAutonomousChapterTick(
    const FName RegionId,
    const FRegionalAutonomousWarTick& Tick)
{
    FColonialRegionState* Region = Regions.Find(RegionId);
    int32* LastTickChapter = LastAutonomousTickChapter.Find(RegionId);
    if (!Region
        || !LastTickChapter
        || *LastTickChapter >= CurrentChapter
        || !IsValidAutonomousDelta(Tick.ImperialDelta)
        || !IsValidAutonomousDelta(Tick.AlbionDelta)
        || !IsValidAutonomousDelta(Tick.LiberationDelta)
        || !IsValidAutonomousDelta(Tick.CrimsonDelta))
    {
        return false;
    }

    Region->ImperialControl = ClampControl(Region->ImperialControl + Tick.ImperialDelta);
    Region->AlbionControl = ClampControl(Region->AlbionControl + Tick.AlbionDelta);
    Region->LiberationStrength = ClampControl(Region->LiberationStrength + Tick.LiberationDelta);
    Region->CrimsonThreat = ClampControl(Region->CrimsonThreat + Tick.CrimsonDelta);
    *LastTickChapter = CurrentChapter;
    ReevaluateRegion(*Region);
    return true;
}

bool UColonialWarStateSubsystem::RecordFallAssaultCompleted(const FName RegionId)
{
    FColonialRegionState* Region = Regions.Find(RegionId);
    if (!Region)
    {
        return false;
    }

    Region->bFallAssaultCompleted = true;
    ReevaluateRegion(*Region);
    return true;
}

bool UColonialWarStateSubsystem::AdvanceChapter(const int32 Chapter)
{
    if (Chapter <= CurrentChapter)
    {
        return false;
    }

    CurrentChapter = Chapter;
    MomentumPhase = ResolveMomentumPhase(CurrentChapter);
    return true;
}

FRegionalWarSnapshot UColonialWarStateSubsystem::GetRegionSnapshot(const FName RegionId) const
{
    FRegionalWarSnapshot Snapshot;
    const FColonialRegionState* Region = Regions.Find(RegionId);
    if (!Region)
    {
        return Snapshot;
    }

    Snapshot.RegionId = RegionId;
    Snapshot.Outcome = Region->Outcome;
    Snapshot.bCrisis = Region->Outcome == ERegionalWarOutcome::Crisis;
    Snapshot.bFallAssaultCompleted = Region->bFallAssaultCompleted;

    const int32 ColonialMax = FMath::Max(Region->ImperialControl, Region->AlbionControl);
    const int32 NonColonialMax = FMath::Max(Region->LiberationStrength, Region->CrimsonThreat);
    const int32 MaxControl = FMath::Max(ColonialMax, NonColonialMax);
    if (MaxControl == Region->LiberationStrength)
    {
        Snapshot.DominantFaction = EColonialFaction::Liberation;
    }
    else if (MaxControl == Region->CrimsonThreat)
    {
        Snapshot.DominantFaction = EColonialFaction::Crimson;
    }
    else if (MaxControl == Region->AlbionControl)
    {
        Snapshot.DominantFaction = EColonialFaction::Albion;
    }
    else
    {
        Snapshot.DominantFaction = EColonialFaction::Imperial;
    }
    return Snapshot;
}

bool UColonialWarStateSubsystem::IsRegionLiberated(const FName RegionId) const
{
    const FColonialRegionState* Region = Regions.Find(RegionId);
    return Region && Region->Outcome == ERegionalWarOutcome::Liberated;
}

int32 UColonialWarStateSubsystem::GetHiddenControlForTests(
    const FName RegionId,
    const EColonialFaction Faction) const
{
    const FColonialRegionState* Region = Regions.Find(RegionId);
    return Region ? ResolveFactionControl(*Region, Faction) : 0;
}

int32 UColonialWarStateSubsystem::ClampControl(const int32 Value)
{
    return FMath::Clamp(Value, 0, 100);
}

EWarMomentumPhase UColonialWarStateSubsystem::ResolveMomentumPhase(const int32 Chapter)
{
    if (Chapter <= 6)
    {
        return EWarMomentumPhase::Alpha;
    }
    if (Chapter <= 8)
    {
        return EWarMomentumPhase::Beta;
    }
    return EWarMomentumPhase::Gamma;
}

bool UColonialWarStateSubsystem::IsValidAutonomousDelta(const int32 Delta)
{
    return Delta >= -100 && Delta <= 100;
}

void UColonialWarStateSubsystem::ReevaluateRegion(FColonialRegionState& Region)
{
    const int32 ColonialControl = FMath::Max(Region.ImperialControl, Region.AlbionControl);
    if (ColonialControl < 30)
    {
        Region.Outcome = ERegionalWarOutcome::Crisis;
        if (Region.bFallAssaultCompleted)
        {
            if (Region.LiberationStrength >= 50)
            {
                Region.Outcome = ERegionalWarOutcome::Liberated;
            }
            else if (Region.CrimsonThreat >= 50)
            {
                Region.Outcome = ERegionalWarOutcome::CrimsonOccupied;
            }
            else
            {
                Region.Outcome = ERegionalWarOutcome::Vacuum;
            }
        }
        return;
    }

    Region.Outcome = Region.ImperialControl >= Region.AlbionControl
        ? ERegionalWarOutcome::ImperialControlled
        : ERegionalWarOutcome::AlbionControlled;
}

int32& UColonialWarStateSubsystem::ResolveFactionControl(
    FColonialRegionState& Region,
    const EColonialFaction Faction)
{
    switch (Faction)
    {
    case EColonialFaction::Albion:
        return Region.AlbionControl;
    case EColonialFaction::Liberation:
        return Region.LiberationStrength;
    case EColonialFaction::Crimson:
        return Region.CrimsonThreat;
    case EColonialFaction::Imperial:
    default:
        return Region.ImperialControl;
    }
}

const int32& UColonialWarStateSubsystem::ResolveFactionControl(
    const FColonialRegionState& Region,
    const EColonialFaction Faction) const
{
    switch (Faction)
    {
    case EColonialFaction::Albion:
        return Region.AlbionControl;
    case EColonialFaction::Liberation:
        return Region.LiberationStrength;
    case EColonialFaction::Crimson:
        return Region.CrimsonThreat;
    case EColonialFaction::Imperial:
    default:
        return Region.ImperialControl;
    }
}
