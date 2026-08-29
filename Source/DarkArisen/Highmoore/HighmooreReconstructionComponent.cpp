// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/HighmooreReconstructionComponent.h"

const FName UHighmooreReconstructionComponent::ThreadId(TEXT("thread.highmoore.reconstruction"));

UHighmooreReconstructionComponent::UHighmooreReconstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

EHighmooreReconstructionPath UHighmooreReconstructionComponent::ResolvePathFromPrincessFacts(
    const EBelosPathResolution BelosResolution,
    const bool bWentToUndercroftStair)
{
    if (bWentToUndercroftStair)
    {
        return EHighmooreReconstructionPath::PermanentlyUnavailableAfterStair;
    }

    if (BelosResolution == EBelosPathResolution::TurnedAway)
    {
        // The source says this path becomes diplomatic rather than constructive, but does not
        // provide the replacement project set. It remains a distinct fail-closed production gap.
        return EHighmooreReconstructionPath::TurnedWest;
    }

    if (BelosResolution == EBelosPathResolution::KatanaAssaultInterrupted
        || BelosResolution == EBelosPathResolution::OrdinaryWeaponOverwhelmed)
    {
        return EHighmooreReconstructionPath::StandardAfterBelos;
    }

    return EHighmooreReconstructionPath::NotAvailableYet;
}

bool UHighmooreReconstructionComponent::InitializeFromPrincessState(
    const UPrincessQuestStateComponent* PrincessState)
{
    if (bInitialized
        || !PrincessState
        || PrincessState->GetMovement() != EPrincessQuestMovement::DockReached)
    {
        return false;
    }

    const EHighmooreReconstructionPath ResolvedPath = ResolvePathFromPrincessFacts(
        PrincessState->GetBelosResolution(),
        PrincessState->DidGoToUndercroftStair());
    if (ResolvedPath == EHighmooreReconstructionPath::NotAvailableYet)
    {
        return false;
    }

    bInitialized = true;
    bRealLetterStillOnPillow = !PrincessState->DidReadRealLetter();
    Path = ResolvedPath;
    return true;
}

bool UHighmooreReconstructionComponent::IsReconstructionAvailable() const
{
    return bInitialized
        && Path == EHighmooreReconstructionPath::StandardAfterBelos;
}

bool UHighmooreReconstructionComponent::IsProjectAvailable(
    const EHighmooreReconstructionProject Project) const
{
    if (!IsReconstructionAvailable())
    {
        return false;
    }

    // Section 4.3 calls the through-mountain supply line Full Vision; its exact construction
    // contract is not authored strongly enough to expose as an executable project yet.
    if (Project == EHighmooreReconstructionProject::ConnectArchipelagoSupplyLine)
    {
        return false;
    }

    return true;
}

bool UHighmooreReconstructionComponent::RecordCadwallMasonsSecured()
{
    if (!IsReconstructionAvailable() || bCadwallMasonsSecured)
    {
        return false;
    }
    bCadwallMasonsSecured = true;
    return true;
}

bool UHighmooreReconstructionComponent::RecordProjectCompleted(
    const EHighmooreReconstructionProject Project,
    const bool bExternalConstructionProof)
{
    if (!bExternalConstructionProof
        || !IsProjectAvailable(Project)
        || CompletedProjects.Contains(Project))
    {
        return false;
    }

    if (Project == EHighmooreReconstructionProject::BuildFordBridge && !bCadwallMasonsSecured)
    {
        return false;
    }

    CompletedProjects.Add(Project);
    return true;
}

bool UHighmooreReconstructionComponent::IsProjectCompleted(
    const EHighmooreReconstructionProject Project) const
{
    return CompletedProjects.Contains(Project);
}

bool UHighmooreReconstructionComponent::RecordRealLetterReadLater()
{
    if (!bInitialized || !bRealLetterStillOnPillow)
    {
        return false;
    }

    bRealLetterStillOnPillow = false;
    return true;
}
