// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/CrewPersonalThreadsComponent.h"

#include "Systems/ProgressionEconomyComponent.h"

UCrewPersonalThreadsComponent::UCrewPersonalThreadsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    ThreadStates.Reserve(5);
    for (const ECrewPersonalThread Thread : {
        ECrewPersonalThread::InesEsperanza,
        ECrewPersonalThread::MirasCoast,
        ECrewPersonalThread::BigTomsService,
        ECrewPersonalThread::FatherSalviosParish,
        ECrewPersonalThread::EstebansLastChart})
    {
        FCrewThreadRuntimeState State;
        State.Thread = Thread;
        ThreadStates.Add(State);
    }
}

bool UCrewPersonalThreadsComponent::ActivateInesThread(
    const int32 CurrentChapter,
    const int32 MeetingCount)
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::InesEsperanza);
    if (!State
        || State->Lifecycle != ECrewThreadLifecycle::Dormant
        || CurrentChapter < 5
        || MeetingCount < 3)
    {
        return false;
    }

    State->Lifecycle = ECrewThreadLifecycle::Active;
    InesStage = EInesThreadStage::ActivatedAfterThirdMeeting;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordInesPortraitShared()
{
    if (InesStage != EInesThreadStage::ActivatedAfterThirdMeeting)
    {
        return false;
    }
    InesStage = EInesThreadStage::PortraitShared;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordInesFirstLead()
{
    if (InesStage != EInesThreadStage::PortraitShared)
    {
        return false;
    }
    InesStage = EInesThreadStage::FirstLead;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordInesImperialRecordsFound()
{
    if (InesStage != EInesThreadStage::FirstLead)
    {
        return false;
    }
    InesStage = EInesThreadStage::ImperialRecords;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordInesCrimsonTrailFound()
{
    if (InesStage != EInesThreadStage::ImperialRecords)
    {
        return false;
    }
    InesStage = EInesThreadStage::CrimsonTrail;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordInesHouseholdLocated()
{
    if (InesStage != EInesThreadStage::CrimsonTrail)
    {
        return false;
    }
    InesStage = EInesThreadStage::HouseholdLocated;
    return true;
}

bool UCrewPersonalThreadsComponent::ResolveInesTruth(const EEsperanzaOutcome Outcome)
{
    if (InesStage != EInesThreadStage::HouseholdLocated || Outcome == EEsperanzaOutcome::None)
    {
        return false;
    }

    EsperanzaOutcome = Outcome;
    InesStage = EInesThreadStage::Resolved;
    return SetResolved(ECrewPersonalThread::InesEsperanza);
}

bool UCrewPersonalThreadsComponent::ActivateMiraThread(
    const bool bOverheardInFjordlund,
    const bool bFjordlundControlAlreadyResolved)
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::MirasCoast);
    if (!State || State->Lifecycle != ECrewThreadLifecycle::Dormant)
    {
        return false;
    }

    if (bFjordlundControlAlreadyResolved)
    {
        State->Lifecycle = ECrewThreadLifecycle::Expired;
        MiraStage = EMiraThreadStage::Expired;
        return false;
    }

    if (!bOverheardInFjordlund)
    {
        return false;
    }

    State->Lifecycle = ECrewThreadLifecycle::Active;
    MiraStage = EMiraThreadStage::ActivatedInFjordlund;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordMiraFirstLead()
{
    if (MiraStage != EMiraThreadStage::ActivatedInFjordlund)
    {
        return false;
    }
    MiraStage = EMiraThreadStage::FirstLead;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordMiraSearchUnderway()
{
    if (MiraStage != EMiraThreadStage::FirstLead)
    {
        return false;
    }
    MiraStage = EMiraThreadStage::SearchUnderway;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordMiraCrucialLead()
{
    if (MiraStage != EMiraThreadStage::SearchUnderway)
    {
        return false;
    }
    MiraStage = EMiraThreadStage::CrucialLead;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordMiraArmadaInvestigation()
{
    if (MiraStage != EMiraThreadStage::CrucialLead)
    {
        return false;
    }
    MiraStage = EMiraThreadStage::ArmadaInvestigation;
    return true;
}

bool UCrewPersonalThreadsComponent::ResolveMiraTruth(const EMiraBrotherOutcome Outcome)
{
    if (MiraStage != EMiraThreadStage::ArmadaInvestigation || Outcome == EMiraBrotherOutcome::None)
    {
        return false;
    }

    MiraOutcome = Outcome;
    MiraStage = EMiraThreadStage::Resolved;
    return SetResolved(ECrewPersonalThread::MirasCoast);
}

bool UCrewPersonalThreadsComponent::RecordFjordlundControlResolved()
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::MirasCoast);
    if (!State
        || State->Lifecycle == ECrewThreadLifecycle::Resolved
        || State->Lifecycle == ECrewThreadLifecycle::Expired)
    {
        return false;
    }

    State->Lifecycle = ECrewThreadLifecycle::Expired;
    MiraStage = EMiraThreadStage::Expired;
    return true;
}

bool UCrewPersonalThreadsComponent::ActivateBigTomThread(const bool bRecogniserSawTomAndLeft)
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::BigTomsService);
    if (!State
        || State->Lifecycle != ECrewThreadLifecycle::Dormant
        || !bRecogniserSawTomAndLeft)
    {
        return false;
    }

    State->Lifecycle = ECrewThreadLifecycle::Active;
    BigTomStage = EBigTomThreadStage::RecognisedAbsence;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordBigTomServiceStory()
{
    if (BigTomStage != EBigTomThreadStage::RecognisedAbsence)
    {
        return false;
    }
    BigTomStage = EBigTomThreadStage::ServiceStory;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordBigTomTrackingReinhardt()
{
    if (BigTomStage != EBigTomThreadStage::ServiceStory)
    {
        return false;
    }
    BigTomStage = EBigTomThreadStage::TrackingReinhardt;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordBigTomChoiceFormed()
{
    if (BigTomStage != EBigTomThreadStage::TrackingReinhardt)
    {
        return false;
    }
    BigTomStage = EBigTomThreadStage::ChoiceFormed;
    return true;
}

bool UCrewPersonalThreadsComponent::RecordBigTomConfrontationReached()
{
    if (BigTomStage != EBigTomThreadStage::ChoiceFormed)
    {
        return false;
    }
    BigTomStage = EBigTomThreadStage::Confrontation;
    return true;
}

bool UCrewPersonalThreadsComponent::ResolveBigTomService(const EBigTomServiceOutcome Outcome)
{
    if (BigTomStage != EBigTomThreadStage::Confrontation || Outcome == EBigTomServiceOutcome::None)
    {
        return false;
    }

    BigTomOutcome = Outcome;
    BigTomStage = EBigTomThreadStage::Resolved;
    return SetResolved(ECrewPersonalThread::BigTomsService);
}

bool UCrewPersonalThreadsComponent::ActivateSalvioParishThread(
    const bool bJakeHasSparedSomeone,
    const bool bConversationCompleted)
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::FatherSalviosParish);
    if (!State
        || State->Lifecycle != ECrewThreadLifecycle::Dormant
        || !bJakeHasSparedSomeone
        || !bConversationCompleted)
    {
        return false;
    }

    // Phase 11 changes the older personal arc into "Father Salvio's parish — what he was before"
    // but supplies no parish stage chain. Stop here rather than silently restoring the superseded arc.
    State->Lifecycle = ECrewThreadLifecycle::AwaitingAuthoredDetail;
    return true;
}

bool UCrewPersonalThreadsComponent::ActivateEstebanLastChartThread(
    const bool bPhysicalChartFoundInQuarters)
{
    FCrewThreadRuntimeState* State = FindStateMutable(ECrewPersonalThread::EstebansLastChart);
    if (!State
        || State->Lifecycle != ECrewThreadLifecycle::Dormant
        || !bPhysicalChartFoundInQuarters)
    {
        return false;
    }

    // Phase 11 defines a chart marking one place he never went, but not the destination or its
    // authored stage/outcome chain. The chart is an activation object, never a licence to invent it.
    State->Lifecycle = ECrewThreadLifecycle::AwaitingAuthoredDetail;
    return true;
}

bool UCrewPersonalThreadsComponent::ClaimCompletionMarks(
    const ECrewPersonalThread Thread,
    UProgressionEconomyComponent* Progression)
{
    FCrewThreadRuntimeState* State = FindStateMutable(Thread);
    if (!State
        || !Progression
        || State->Lifecycle != ECrewThreadLifecycle::Resolved
        || State->bCompletionMarksClaimed)
    {
        return false;
    }

    if (!Progression->AwardMarks(MarkSourceId(Thread), CompletionMarksPerCrewThread))
    {
        return false;
    }

    State->bCompletionMarksClaimed = true;
    return true;
}

ECrewThreadLifecycle UCrewPersonalThreadsComponent::GetLifecycle(const ECrewPersonalThread Thread) const
{
    const FCrewThreadRuntimeState* State = FindState(Thread);
    return State ? State->Lifecycle : ECrewThreadLifecycle::Dormant;
}

FCrewThreadRuntimeState* UCrewPersonalThreadsComponent::FindStateMutable(const ECrewPersonalThread Thread)
{
    return ThreadStates.FindByPredicate(
        [Thread](const FCrewThreadRuntimeState& State)
        {
            return State.Thread == Thread;
        });
}

const FCrewThreadRuntimeState* UCrewPersonalThreadsComponent::FindState(const ECrewPersonalThread Thread) const
{
    return ThreadStates.FindByPredicate(
        [Thread](const FCrewThreadRuntimeState& State)
        {
            return State.Thread == Thread;
        });
}

bool UCrewPersonalThreadsComponent::SetResolved(const ECrewPersonalThread Thread)
{
    FCrewThreadRuntimeState* State = FindStateMutable(Thread);
    if (!State || State->Lifecycle != ECrewThreadLifecycle::Active)
    {
        return false;
    }

    State->Lifecycle = ECrewThreadLifecycle::Resolved;
    return true;
}

FName UCrewPersonalThreadsComponent::MarkSourceId(const ECrewPersonalThread Thread)
{
    switch (Thread)
    {
    case ECrewPersonalThread::InesEsperanza:
        return TEXT("thread.crew.ines-esperanza");
    case ECrewPersonalThread::MirasCoast:
        return TEXT("thread.crew.miras-coast");
    case ECrewPersonalThread::BigTomsService:
        return TEXT("thread.crew.big-toms-service");
    case ECrewPersonalThread::FatherSalviosParish:
        return TEXT("thread.crew.father-salvios-parish");
    case ECrewPersonalThread::EstebansLastChart:
        return TEXT("thread.crew.estebans-last-chart");
    default:
        return NAME_None;
    }
}
