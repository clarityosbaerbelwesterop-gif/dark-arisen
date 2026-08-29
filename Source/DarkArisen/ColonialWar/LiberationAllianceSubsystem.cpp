// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/LiberationAllianceSubsystem.h"

const FName ULiberationAllianceSubsystem::ThreadId(TEXT("thread.archipelago.liberation-connections"));

namespace
{
FAllianceConnectionDefinition Connection(
    const TCHAR* StableId,
    const ELiberationNetwork A,
    const ELiberationNetwork B,
    const EAllianceConnectionAuthorship Authorship,
    const TCHAR* Source)
{
    FAllianceConnectionDefinition Definition;
    Definition.StableId = StableId;
    Definition.A = A;
    Definition.B = B;
    Definition.Authorship = Authorship;
    Definition.GoverningSource = Source;
    return Definition;
}

constexpr int32 NetworkCount = 5;

int32 NetworkIndex(const ELiberationNetwork Network)
{
    return static_cast<int32>(Network);
}
}

void ULiberationAllianceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    for (int32 Index = 0; Index < NetworkCount; ++Index)
    {
        const ELiberationNetwork Network = static_cast<ELiberationNetwork>(Index);
        if (!Networks.Contains(Network))
        {
            FLiberationNetworkState State;
            State.Network = Network;
            State.Strength = StartingStrength(Network);
            State.Trust = EAllianceTrustTier::Watched;
            Networks.Add(Network, State);
        }
    }
}

FLiberationNetworkState ULiberationAllianceSubsystem::GetNetworkState(const ELiberationNetwork Network) const
{
    if (const FLiberationNetworkState* State = Networks.Find(Network))
    {
        return *State;
    }

    FLiberationNetworkState Fallback;
    Fallback.Network = Network;
    Fallback.Strength = StartingStrength(Network);
    Fallback.Trust = EAllianceTrustTier::Watched;
    return Fallback;
}

bool ULiberationAllianceSubsystem::RecordStrengthAction(
    const ELiberationNetwork Network,
    const EAllianceStrengthAction Action)
{
    FLiberationNetworkState* State = Networks.Find(Network);
    if (!State)
    {
        return false;
    }

    const int32 Gain = StrengthGain(Action);
    if (Gain <= 0)
    {
        return false;
    }
    State->Strength = FMath::Clamp(State->Strength + Gain, 0, 100);
    return true;
}

bool ULiberationAllianceSubsystem::RecordCounterAttackLoss(
    const ELiberationNetwork Network,
    const int32 AuthoredLoss)
{
    FLiberationNetworkState* State = Networks.Find(Network);
    if (!State || AuthoredLoss < 5 || AuthoredLoss > 10)
    {
        return false;
    }
    State->Strength = FMath::Max(0, State->Strength - AuthoredLoss);
    return true;
}

bool ULiberationAllianceSubsystem::RecordScorchedEarthLoss(const ELiberationNetwork Network)
{
    FLiberationNetworkState* State = Networks.Find(Network);
    if (!State)
    {
        return false;
    }
    State->Strength = FMath::Max(0, State->Strength - 20);
    return true;
}

bool ULiberationAllianceSubsystem::RecordTrustTierReached(
    const ELiberationNetwork Network,
    const EAllianceTrustTier NewTier)
{
    FLiberationNetworkState* State = Networks.Find(Network);
    if (!State || static_cast<uint8>(NewTier) <= static_cast<uint8>(State->Trust))
    {
        return false;
    }
    State->Trust = NewTier;
    return true;
}

bool ULiberationAllianceSubsystem::RecordBetrayal(
    const ELiberationNetwork Network,
    const int32 SourceApprovedStrengthLoss)
{
    FLiberationNetworkState* State = Networks.Find(Network);
    if (!State || SourceApprovedStrengthLoss <= 0 || SourceApprovedStrengthLoss > 100)
    {
        return false;
    }

    State->Strength = FMath::Max(0, State->Strength - SourceApprovedStrengthLoss);
    State->Trust = EAllianceTrustTier::Watched;
    return true;
}

TArray<FAllianceConnectionDefinition> ULiberationAllianceSubsystem::GetConnectionDefinitions() const
{
    using N = ELiberationNetwork;
    using A = EAllianceConnectionAuthorship;

    return {
        Connection(TEXT("alliance.connection.cultural-recovery"), N::CulturalWeb, N::RecoveryUnderground,
            A::AuthoredQuest, TEXT("indigenous_liberation_alliance.md Section 4.2")),
        Connection(TEXT("alliance.connection.cultural-mountain"), N::CulturalWeb, N::MountainPaths,
            A::AuthoredQuest, TEXT("indigenous_liberation_alliance.md Section 4.3")),
        Connection(TEXT("alliance.connection.mountain-patient"), N::MountainPaths, N::PatientCoast,
            A::AuthoredQuest, TEXT("indigenous_liberation_alliance.md Section 4.4")),
        Connection(TEXT("alliance.connection.patient-deed"), N::PatientCoast, N::DeedJudgers,
            A::AuthoredQuest, TEXT("indigenous_liberation_alliance.md Section 4.5")),
        Connection(TEXT("alliance.connection.recovery-deed"), N::RecoveryUnderground, N::DeedJudgers,
            A::AuthoredQuest, TEXT("indigenous_liberation_alliance.md Section 4.6")),

        // Section 4.7 names these pairings but withholds their obstacle/quest details as Full Vision.
        Connection(TEXT("alliance.connection.cultural-patient"), N::CulturalWeb, N::PatientCoast,
            A::FullVisionGap, TEXT("indigenous_liberation_alliance.md Section 4.7")),
        Connection(TEXT("alliance.connection.cultural-deed"), N::CulturalWeb, N::DeedJudgers,
            A::FullVisionGap, TEXT("indigenous_liberation_alliance.md Section 4.7")),
        Connection(TEXT("alliance.connection.recovery-mountain"), N::RecoveryUnderground, N::MountainPaths,
            A::FullVisionGap, TEXT("indigenous_liberation_alliance.md Section 4.7")),
        Connection(TEXT("alliance.connection.mountain-deed"), N::MountainPaths, N::DeedJudgers,
            A::FullVisionGap, TEXT("indigenous_liberation_alliance.md Section 4.7")),
        Connection(TEXT("alliance.connection.recovery-patient"), N::RecoveryUnderground, N::PatientCoast,
            A::FullVisionGap, TEXT("indigenous_liberation_alliance.md Section 4.7"))
    };
}

bool ULiberationAllianceSubsystem::RecordAuthoredConnectionCompleted(const FName ConnectionId)
{
    if (ConnectionId.IsNone() || CompletedConnections.Contains(ConnectionId))
    {
        return false;
    }

    const TArray<FAllianceConnectionDefinition> Definitions = GetConnectionDefinitions();
    const FAllianceConnectionDefinition* Definition = Definitions.FindByPredicate(
        [ConnectionId](const FAllianceConnectionDefinition& Candidate)
        {
            return Candidate.StableId == ConnectionId;
        });
    if (!Definition || Definition->Authorship != EAllianceConnectionAuthorship::AuthoredQuest)
    {
        return false;
    }

    CompletedConnections.Add(ConnectionId);
    RefreshFirstGatheringState();
    return true;
}

bool ULiberationAllianceSubsystem::IsConnectionComplete(const FName ConnectionId) const
{
    return CompletedConnections.Contains(ConnectionId);
}

int32 ULiberationAllianceSubsystem::GetConnectedNetworkCount() const
{
    bool Edges[NetworkCount][NetworkCount] = {};
    for (const FAllianceConnectionDefinition& Definition : GetConnectionDefinitions())
    {
        if (!CompletedConnections.Contains(Definition.StableId))
        {
            continue;
        }

        const int32 A = NetworkIndex(Definition.A);
        const int32 B = NetworkIndex(Definition.B);
        if (A >= 0 && A < NetworkCount && B >= 0 && B < NetworkCount)
        {
            Edges[A][B] = true;
            Edges[B][A] = true;
        }
    }

    int32 Largest = 1;
    for (int32 Start = 0; Start < NetworkCount; ++Start)
    {
        bool Visited[NetworkCount] = {};
        int32 Queue[NetworkCount] = {};
        int32 Head = 0;
        int32 Tail = 0;
        Queue[Tail++] = Start;
        Visited[Start] = true;
        int32 Size = 0;

        while (Head < Tail)
        {
            const int32 Current = Queue[Head++];
            ++Size;
            for (int32 Other = 0; Other < NetworkCount; ++Other)
            {
                if (Edges[Current][Other] && !Visited[Other])
                {
                    Visited[Other] = true;
                    Queue[Tail++] = Other;
                }
            }
        }
        Largest = FMath::Max(Largest, Size);
    }
    return Largest;
}

float ULiberationAllianceSubsystem::GetConnectionMultiplier() const
{
    switch (GetConnectedNetworkCount())
    {
    case 2: return 1.15f;
    case 3: return 1.25f;
    case 4: return 1.40f;
    case 5: return 1.60f;
    default: return 1.00f;
    }
}

bool ULiberationAllianceSubsystem::RecordFirstGatheringCompleted()
{
    if (!bFirstGatheringPending || bFirstGatheringCompleted || GetConnectedNetworkCount() != 5)
    {
        return false;
    }
    bFirstGatheringPending = false;
    bFirstGatheringCompleted = true;
    return true;
}

int32 ULiberationAllianceSubsystem::GetUnresolvedFullVisionConnectionCount() const
{
    int32 Count = 0;
    for (const FAllianceConnectionDefinition& Definition : GetConnectionDefinitions())
    {
        if (Definition.Authorship == EAllianceConnectionAuthorship::FullVisionGap)
        {
            ++Count;
        }
    }
    return Count;
}

int32 ULiberationAllianceSubsystem::StartingStrength(const ELiberationNetwork Network)
{
    switch (Network)
    {
    case ELiberationNetwork::CulturalWeb: return 12;
    case ELiberationNetwork::RecoveryUnderground: return 8;
    case ELiberationNetwork::MountainPaths: return 10;
    case ELiberationNetwork::PatientCoast: return 9;
    case ELiberationNetwork::DeedJudgers: return 6;
    default: return 0;
    }
}

int32 ULiberationAllianceSubsystem::StrengthGain(const EAllianceStrengthAction Action)
{
    switch (Action)
    {
    case EAllianceStrengthAction::FreeForcedLabourConvoy: return 3;
    case EAllianceStrengthAction::ReturnSacredItem: return 5;
    case EAllianceStrengthAction::DeliverWeapons: return 5;
    case EAllianceStrengthAction::CompleteTrainingQuestChain: return 8;
    case EAllianceStrengthAction::ExposeAtrocityWithEvidence: return 6;
    case EAllianceStrengthAction::DefendNetworkCell: return 7;
    case EAllianceStrengthAction::DestroyEnforcementRecords: return 10;
    case EAllianceStrengthAction::WinLiberationEvent: return 25;
    default: return 0;
    }
}

bool ULiberationAllianceSubsystem::SameUndirectedPair(
    const FAllianceConnectionDefinition& Definition,
    const ELiberationNetwork A,
    const ELiberationNetwork B)
{
    return (Definition.A == A && Definition.B == B)
        || (Definition.A == B && Definition.B == A);
}

void ULiberationAllianceSubsystem::RefreshFirstGatheringState()
{
    if (!bFirstGatheringCompleted && GetConnectedNetworkCount() == 5)
    {
        bFirstGatheringPending = true;
    }
}
