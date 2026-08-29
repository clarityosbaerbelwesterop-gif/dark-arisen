// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/ArchipelagoThreadsComponent.h"

const FName UArchipelagoThreadsComponent::KesslerThreadId(TEXT("thread.archipelago.kesslers-investigation"));
const FName UArchipelagoThreadsComponent::LedgerThreadId(TEXT("thread.archipelago.ledger-trilogy"));
const FName UArchipelagoThreadsComponent::SchreiberThreadId(TEXT("thread.archipelago.schreiber-documentation"));
const FName UArchipelagoThreadsComponent::VasquezThreadId(TEXT("thread.archipelago.captain-vasquez"));
const FName UArchipelagoThreadsComponent::OldFortressesThreadId(TEXT("thread.archipelago.old-fortresses"));

UArchipelagoThreadsComponent::UArchipelagoThreadsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UArchipelagoThreadsComponent::ActivateKesslerInvestigation(const bool bInitialTrustEstablished)
{
    if (KesslerLifecycle != EArchipelagoThreadLifecycle::Dormant || !bInitialTrustEstablished)
    {
        return false;
    }

    KesslerLifecycle = EArchipelagoThreadLifecycle::Active;
    KesslerStage = EKesslerInvestigationStage::InitialTrust;
    return true;
}

bool UArchipelagoThreadsComponent::RecordKesslerBribeOfficerInvestigated()
{
    if (KesslerStage != EKesslerInvestigationStage::InitialTrust)
    {
        return false;
    }
    KesslerStage = EKesslerInvestigationStage::BribeOfficerInvestigated;
    return true;
}

bool UArchipelagoThreadsComponent::RecordKesslerNobleNetworkInvestigated()
{
    if (KesslerStage != EKesslerInvestigationStage::BribeOfficerInvestigated)
    {
        return false;
    }
    KesslerStage = EKesslerInvestigationStage::NobleNetworkInvestigated;
    return true;
}

bool UArchipelagoThreadsComponent::RecordKesslerCrimsonProtectionInvestigated()
{
    if (KesslerStage != EKesslerInvestigationStage::NobleNetworkInvestigated)
    {
        return false;
    }
    KesslerStage = EKesslerInvestigationStage::CrimsonProtectionInvestigated;
    return true;
}

bool UArchipelagoThreadsComponent::RecordCastilloDoradoBribeLedgersRecovered()
{
    if (KesslerStage != EKesslerInvestigationStage::CrimsonProtectionInvestigated
        || KesslerLifecycle != EArchipelagoThreadLifecycle::Active)
    {
        return false;
    }

    KesslerStage = EKesslerInvestigationStage::Resolved;
    KesslerLifecycle = EArchipelagoThreadLifecycle::Resolved;
    return true;
}

bool UArchipelagoThreadsComponent::RecordKesslerExpiryWorldState(
    const bool bCastilloDoradoFell,
    const bool bDeSilvaDied)
{
    if ((!bCastilloDoradoFell && !bDeSilvaDied)
        || KesslerLifecycle == EArchipelagoThreadLifecycle::Resolved
        || KesslerLifecycle == EArchipelagoThreadLifecycle::Expired)
    {
        return false;
    }

    KesslerLifecycle = EArchipelagoThreadLifecycle::Expired;
    KesslerStage = EKesslerInvestigationStage::Expired;
    return true;
}

bool UArchipelagoThreadsComponent::RecordLedgerRecovered(const ELedgerTrilogyRecord Record)
{
    if (LedgerLifecycle == EArchipelagoThreadLifecycle::Resolved || RecoveredLedgers.Contains(Record))
    {
        return false;
    }

    if (LedgerLifecycle == EArchipelagoThreadLifecycle::Dormant)
    {
        LedgerLifecycle = EArchipelagoThreadLifecycle::Active;
    }

    RecoveredLedgers.Add(Record);
    if (RecoveredLedgers.Num() == 4)
    {
        LedgerLifecycle = EArchipelagoThreadLifecycle::Resolved;
    }
    return true;
}

bool UArchipelagoThreadsComponent::ActivateSchreiberDocumentation(const bool bResearchRelationshipOpened)
{
    if (SchreiberLifecycle != EArchipelagoThreadLifecycle::Dormant || !bResearchRelationshipOpened)
    {
        return false;
    }

    SchreiberLifecycle = EArchipelagoThreadLifecycle::Active;
    SchreiberStage = ESchreiberDocumentationStage::ResearchRelationshipOpened;
    return true;
}

bool UArchipelagoThreadsComponent::RecordSchreiberEthicalMethodEstablished()
{
    if (SchreiberStage != ESchreiberDocumentationStage::ResearchRelationshipOpened)
    {
        return false;
    }

    SchreiberStage = ESchreiberDocumentationStage::EthicalMethodEstablished;
    return true;
}

bool UArchipelagoThreadsComponent::RecordHarbourThatWasFirstReached()
{
    if (SchreiberStage != ESchreiberDocumentationStage::EthicalMethodEstablished)
    {
        return false;
    }

    SchreiberStage = ESchreiberDocumentationStage::HarbourThatWasFirstReached;
    return true;
}

bool UArchipelagoThreadsComponent::RecordFirstHarbourBoatsDocumentedAndLeftUntaken()
{
    if (SchreiberStage != ESchreiberDocumentationStage::HarbourThatWasFirstReached)
    {
        return false;
    }

    SchreiberStage = ESchreiberDocumentationStage::BoatsDocumentedAndLeft;
    return true;
}

bool UArchipelagoThreadsComponent::RecordFirstHarbourDatableRecordRecovered()
{
    if (SchreiberStage != ESchreiberDocumentationStage::BoatsDocumentedAndLeft)
    {
        return false;
    }

    SchreiberStage = ESchreiberDocumentationStage::Resolved;
    SchreiberLifecycle = EArchipelagoThreadLifecycle::Resolved;
    return true;
}

bool UArchipelagoThreadsComponent::ActivateVasquezThread(const bool bCaptainPeerRelationshipOpened)
{
    if (VasquezLifecycle != EArchipelagoThreadLifecycle::Dormant || !bCaptainPeerRelationshipOpened)
    {
        return false;
    }

    // The source provides a captain-peer multi-stage identity and possible alliance outcomes but
    // not the actual six-hour sequence. Keep the Thread visible to production without fabricating it.
    VasquezLifecycle = EArchipelagoThreadLifecycle::AwaitingAuthoredDetail;
    return true;
}

bool UArchipelagoThreadsComponent::RecordVasquezPrivateEngagementEvidence(
    const bool bEvidenceNamesVasquez)
{
    if (VasquezLifecycle == EArchipelagoThreadLifecycle::Expired || !bEvidenceNamesVasquez)
    {
        return false;
    }

    bVasquezNamedOnPrivateEngagement = true;
    return true;
}

bool UArchipelagoThreadsComponent::RecordVasquezSunk()
{
    if (VasquezLifecycle == EArchipelagoThreadLifecycle::Expired
        || VasquezLifecycle == EArchipelagoThreadLifecycle::Resolved)
    {
        return false;
    }

    VasquezLifecycle = EArchipelagoThreadLifecycle::Expired;
    return true;
}

bool UArchipelagoThreadsComponent::RecordOldFortressReclaimed(
    const EOldFortressSite Site,
    const bool bAllianceAccessProven,
    const bool bPhysicalReclamationCompleted)
{
    if (!bAllianceAccessProven
        || !bPhysicalReclamationCompleted
        || OldFortressesLifecycle == EArchipelagoThreadLifecycle::Resolved
        || ReclaimedOldFortresses.Contains(Site))
    {
        return false;
    }

    if (OldFortressesLifecycle == EArchipelagoThreadLifecycle::Dormant)
    {
        OldFortressesLifecycle = EArchipelagoThreadLifecycle::Active;
    }

    ReclaimedOldFortresses.Add(Site);
    if (ReclaimedOldFortresses.Num() == 4)
    {
        OldFortressesLifecycle = EArchipelagoThreadLifecycle::Resolved;
    }
    return true;
}
