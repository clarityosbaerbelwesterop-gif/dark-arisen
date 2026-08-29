// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/AssassinNetworkThreadComponent.h"

#include "ColonialWar/ColonialWarStateSubsystem.h"
#include "Engine/World.h"

const FName UAssassinNetworkThreadComponent::ThreadId(TEXT("thread.archipelago.assassin-network"));

UAssassinNetworkThreadComponent::UAssassinNetworkThreadComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAssassinNetworkThreadComponent::RecordNamedHunterOutcome(
    const FName HunterId,
    const EAssassinHunterOrganisation Organisation,
    const EAssassinHunterOutcome Outcome)
{
    if (HunterId.IsNone() || NamedHunters.ContainsByPredicate([HunterId](const FAssassinHunterRecord& Record)
        {
            return Record.HunterId == HunterId;
        }))
    {
        return false;
    }

    FAssassinHunterRecord Record;
    Record.HunterId = HunterId;
    Record.Organisation = Organisation;
    Record.Outcome = Outcome;
    NamedHunters.Add(Record);
    return true;
}

bool UAssassinNetworkThreadComponent::RecordCapturedHunterSource(
    const FName HunterId,
    const EAssassinHunterOrganisation Organisation)
{
    const FAssassinHunterRecord* Record = NamedHunters.FindByPredicate([HunterId](const FAssassinHunterRecord& Candidate)
    {
        return Candidate.HunterId == HunterId;
    });
    if (!Record || Record->Outcome != EAssassinHunterOutcome::Spared || Record->Organisation != Organisation)
    {
        return false;
    }

    TSet<FName>& Sources = Organisation == EAssassinHunterOrganisation::Cofradia
        ? CapturedCofradiaSources
        : CapturedRegulatorSources;
    if (Sources.Contains(HunterId))
    {
        return false;
    }
    Sources.Add(HunterId);
    return true;
}

bool UAssassinNetworkThreadComponent::RefreshEntryEligibility()
{
    if (ThreadState != EAssassinThreadState::Dormant || NamedHunters.Num() < 3)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    const URetaliationSubsystem* Retaliation = World->GetSubsystem<URetaliationSubsystem>();
    const UColonialWarStateSubsystem* War = World->GetSubsystem<UColonialWarStateSubsystem>();
    if (!Retaliation || !War || War->GetMomentumPhase() == EWarMomentumPhase::Alpha)
    {
        return false;
    }

    const bool bReachedStageThree = Retaliation->HasReachedStageAtLeastOnce(
        EColonialFaction::Imperial,
        ERetaliationStage::CounterAttack)
        || Retaliation->HasReachedStageAtLeastOnce(
            EColonialFaction::Albion,
            ERetaliationStage::CounterAttack);
    if (!bReachedStageThree)
    {
        return false;
    }

    ThreadState = EAssassinThreadState::CrewScenePending;
    return true;
}

bool UAssassinNetworkThreadComponent::CompleteCrewInstigatingScene()
{
    if (ThreadState != EAssassinThreadState::CrewScenePending)
    {
        return false;
    }
    ThreadState = EAssassinThreadState::Investigation;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordCofradiaChapterMark(
    const FName EvidenceId,
    const FName HunterId,
    const FName ChapterHouseMarkId)
{
    if (EvidenceId.IsNone() || HunterId.IsNone() || ChapterHouseMarkId.IsNone())
    {
        return false;
    }

    const FAssassinHunterRecord* Hunter = NamedHunters.FindByPredicate([HunterId](const FAssassinHunterRecord& Record)
    {
        return Record.HunterId == HunterId;
    });
    if (!Hunter || Hunter->Organisation != EAssassinHunterOrganisation::Cofradia)
    {
        return false;
    }

    if (CofradiaMarks.ContainsByPredicate([EvidenceId, HunterId](const FCofradiaMarkEvidence& Evidence)
        {
            return Evidence.EvidenceId == EvidenceId || Evidence.HunterId == HunterId;
        }))
    {
        return false;
    }

    FCofradiaMarkEvidence Evidence;
    Evidence.EvidenceId = EvidenceId;
    Evidence.HunterId = HunterId;
    Evidence.ChapterHouseMarkId = ChapterHouseMarkId;
    CofradiaMarks.Add(Evidence);
    return true;
}

bool UAssassinNetworkThreadComponent::IdentifyCofradiaChapterHouses()
{
    if (!IsInvestigationOpen() || CofradiaState != ECofradiaInvestigationState::NotStarted)
    {
        return false;
    }

    bool bEvidenceSufficient = !CapturedCofradiaSources.IsEmpty();
    if (!bEvidenceSufficient)
    {
        TSet<FName> Hunters;
        TSet<FName> Marks;
        for (const FCofradiaMarkEvidence& Evidence : CofradiaMarks)
        {
            Hunters.Add(Evidence.HunterId);
            Marks.Add(Evidence.ChapterHouseMarkId);
        }
        bEvidenceSufficient = Hunters.Num() >= 3 && Marks.Num() >= 3;
    }

    if (!bEvidenceSufficient)
    {
        return false;
    }

    CofradiaState = ECofradiaInvestigationState::ChapterHousesIdentified;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordCofradiaHouseInfiltrated(const ECofradiaChapterHouse House)
{
    if (!IsInvestigationOpen()
        || static_cast<uint8>(CofradiaState) < static_cast<uint8>(ECofradiaInvestigationState::ChapterHousesIdentified)
        || static_cast<uint8>(CofradiaState) >= static_cast<uint8>(ECofradiaInvestigationState::WritLedgerRecovered)
        || InfiltratedCofradiaHouses.Contains(House))
    {
        return false;
    }

    InfiltratedCofradiaHouses.Add(House);
    if (InfiltratedCofradiaHouses.Num() == 3)
    {
        CofradiaState = ECofradiaInvestigationState::ThreeHousesInfiltrated;
    }
    return true;
}

bool UAssassinNetworkThreadComponent::RecordWritLedgerRecovered()
{
    if (!IsInvestigationOpen() || CofradiaState != ECofradiaInvestigationState::ThreeHousesInfiltrated)
    {
        return false;
    }
    CofradiaState = ECofradiaInvestigationState::WritLedgerRecovered;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordPrimeraRevealedAsVidal()
{
    if (!IsInvestigationOpen() || CofradiaState != ECofradiaInvestigationState::WritLedgerRecovered)
    {
        return false;
    }
    CofradiaState = ECofradiaInvestigationState::PrimeraRevealed;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordVidalConfessionTaken()
{
    if (bVidalConfessionTaken)
    {
        return false;
    }
    bVidalConfessionTaken = true;
    return true;
}

bool UAssassinNetworkThreadComponent::ResolveCofradiaByPublication(
    const bool bLedgerPublishedThroughReformNetwork,
    const bool bVidalFinalEncounterResolved)
{
    if (!IsInvestigationOpen()
        || CofradiaState != ECofradiaInvestigationState::PrimeraRevealed
        || !bLedgerPublishedThroughReformNetwork
        || !bVidalFinalEncounterResolved)
    {
        return false;
    }

    CofradiaState = ECofradiaInvestigationState::ResolvedDestroyed;
    RefreshOverallResolution();
    return true;
}

bool UAssassinNetworkThreadComponent::ResolveCofradiaByWithdrawal(
    const bool bVidalWithdrewWritAsUnrighteous)
{
    if (!IsInvestigationOpen()
        || CofradiaState != ECofradiaInvestigationState::PrimeraRevealed
        || !bVidalConfessionTaken
        || !bVidalWithdrewWritAsUnrighteous)
    {
        return false;
    }

    CofradiaState = ECofradiaInvestigationState::ResolvedWithdrawn;
    RefreshOverallResolution();
    return true;
}

bool UAssassinNetworkThreadComponent::RecordRegulatorFileReferencesDecoded()
{
    if (!IsInvestigationOpen() || RegulatorState != ERegulatorInvestigationState::NotStarted)
    {
        return false;
    }
    RegulatorState = ERegulatorInvestigationState::FileReferencesDecoded;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordCoventryHouseStationInfiltrated()
{
    if (!IsInvestigationOpen() || RegulatorState != ERegulatorInvestigationState::FileReferencesDecoded)
    {
        return false;
    }
    RegulatorState = ERegulatorInvestigationState::CoventryHouseInfiltrated;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordEngagementRegisterRecovered(
    const bool bUnactionedContractRecovered)
{
    if (!IsInvestigationOpen()
        || RegulatorState != ERegulatorInvestigationState::CoventryHouseInfiltrated
        || !bUnactionedContractRecovered)
    {
        return false;
    }

    bUnactionedRegulatorContractRecovered = true;
    RegulatorState = ERegulatorInvestigationState::EngagementRegisterRecovered;
    return true;
}

bool UAssassinNetworkThreadComponent::RecordKettleMeetingCompleted()
{
    if (!IsInvestigationOpen()
        || RegulatorState != ERegulatorInvestigationState::EngagementRegisterRecovered
        || !bUnactionedRegulatorContractRecovered)
    {
        return false;
    }
    RegulatorState = ERegulatorInvestigationState::KettleMeeting;
    return true;
}

bool UAssassinNetworkThreadComponent::ResolveRegulatorsByDestruction(
    const bool bStationDestroyed,
    const bool bKettleKilledOrRouted,
    const bool bReceiptsDestroyed)
{
    if (!IsInvestigationOpen()
        || RegulatorState != ERegulatorInvestigationState::KettleMeeting
        || !bStationDestroyed
        || !bKettleKilledOrRouted
        || !bReceiptsDestroyed)
    {
        return false;
    }

    bSettlementReceiptsDestroyed = true;
    RegulatorState = ERegulatorInvestigationState::ResolvedDestroyed;
    RefreshOverallResolution();
    return true;
}

bool UAssassinNetworkThreadComponent::ResolveRegulatorsBySettlement(
    const bool bAllContractsPaid,
    const bool bOperatingCostsPaid,
    const bool bRetainerPaid)
{
    if (!IsInvestigationOpen()
        || RegulatorState != ERegulatorInvestigationState::KettleMeeting
        || !bAllContractsPaid
        || !bOperatingCostsPaid
        || !bRetainerPaid)
    {
        return false;
    }

    RegulatorState = ERegulatorInvestigationState::ResolvedTurned;
    RefreshOverallResolution();
    return true;
}

bool UAssassinNetworkThreadComponent::AreCofradiaHuntersActive() const
{
    return CofradiaState != ECofradiaInvestigationState::ResolvedDestroyed
        && CofradiaState != ECofradiaInvestigationState::ResolvedWithdrawn;
}

bool UAssassinNetworkThreadComponent::AreRegulatorHuntersActive() const
{
    return RegulatorState != ERegulatorInvestigationState::ResolvedDestroyed
        && RegulatorState != ERegulatorInvestigationState::ResolvedTurned;
}

bool UAssassinNetworkThreadComponent::IsRegulatorWarningNetworkAvailable() const
{
    return RegulatorState == ERegulatorInvestigationState::ResolvedTurned;
}

bool UAssassinNetworkThreadComponent::IsInvestigationOpen() const
{
    return ThreadState == EAssassinThreadState::Investigation;
}

bool UAssassinNetworkThreadComponent::IsCofradiaResolved() const
{
    return CofradiaState == ECofradiaInvestigationState::ResolvedDestroyed
        || CofradiaState == ECofradiaInvestigationState::ResolvedWithdrawn;
}

bool UAssassinNetworkThreadComponent::IsRegulatorResolved() const
{
    return RegulatorState == ERegulatorInvestigationState::ResolvedDestroyed
        || RegulatorState == ERegulatorInvestigationState::ResolvedTurned;
}

void UAssassinNetworkThreadComponent::RefreshOverallResolution()
{
    if (IsCofradiaResolved() && IsRegulatorResolved())
    {
        ThreadState = EAssassinThreadState::Resolved;
    }
}
