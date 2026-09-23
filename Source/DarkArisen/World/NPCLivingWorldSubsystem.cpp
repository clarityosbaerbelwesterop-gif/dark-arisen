#include "World/NPCLivingWorldSubsystem.h"

bool UNPCLivingWorldSubsystem::RegisterNPC(const FName NpcId,const FName CommunityId)
{
    if(NpcId.IsNone()) return false;
    FNPCLivingRecord& Record=Records.FindOrAdd(NpcId);
    if(Record.NpcId.IsNone()) Record.NpcId=NpcId;
    if(!CommunityId.IsNone()) Record.CommunityId=CommunityId;
    return Record.NpcId==NpcId;
}

bool UNPCLivingWorldSubsystem::UpdateScheduleAnchor(const FName NpcId,const FName AnchorId)
{
    FNPCLivingRecord* Record=Records.Find(NpcId);
    if(!Record || AnchorId.IsNone()) return false;
    Record->CurrentScheduleAnchorId=AnchorId;
    return true;
}

bool UNPCLivingWorldSubsystem::SetMood(const FName NpcId,const ENPCLivingMood Mood)
{
    FNPCLivingRecord* Record=Records.Find(NpcId);
    if(!Record) return false;
    Record->Mood=Mood;
    return true;
}

void UNPCLivingWorldSubsystem::UpsertConnection(FNPCLivingRecord& Record,const FName Other,const ENPCConnectionKind Kind)
{
    for(FNPCSocialConnection& Connection:Record.Connections)
    {
        if(Connection.OtherNpcId==Other){Connection.Kind=Kind;return;}
    }
    FNPCSocialConnection Connection;
    Connection.OtherNpcId=Other;
    Connection.Kind=Kind;
    Record.Connections.Add(Connection);
}

bool UNPCLivingWorldSubsystem::ConnectNPCs(const FName A,const FName B,const ENPCConnectionKind Kind)
{
    if(A.IsNone() || B.IsNone() || A==B) return false;
    FNPCLivingRecord* Left=Records.Find(A);
    FNPCLivingRecord* Right=Records.Find(B);
    if(!Left || !Right) return false;
    UpsertConnection(*Left,B,Kind);
    UpsertConnection(*Right,A,Kind);
    return true;
}

bool UNPCLivingWorldSubsystem::IsValidMemory(const FNPCLivingMemory& Memory)
{
    return !Memory.EventId.IsNone() && Memory.OriginalWeight>=1 && Memory.OriginalWeight<=100 &&
        Memory.EffectiveWeight>=0.f && Memory.EffectiveWeight<=100.f &&
        Memory.OccurredAtGameMinute>=0 && FMath::IsFinite(Memory.DecayPerGameDay) && Memory.DecayPerGameDay>=0.f &&
        static_cast<uint8>(Memory.Kind)<=static_cast<uint8>(ENPCMemoryKind::CulturalReputation) &&
        static_cast<uint8>(Memory.Emotion)<=static_cast<uint8>(ENPCMemoryEmotion::Positive);
}

int32 UNPCLivingWorldSubsystem::ReputationDelta(const FNPCLivingMemory& Memory)
{
    if(Memory.Emotion==ENPCMemoryEmotion::Neutral || Memory.EffectiveWeight<.5f) return 0;
    const int32 Magnitude=FMath::Clamp(FMath::RoundToInt(Memory.EffectiveWeight*.5f),1,50);
    return Memory.Emotion==ENPCMemoryEmotion::Positive ? Magnitude : -Magnitude;
}

bool UNPCLivingWorldSubsystem::RecordMemory(const FName NpcId,const FNPCLivingMemory& Memory)
{
    FNPCLivingRecord* Record=Records.Find(NpcId);
    if(!Record || !IsValidMemory(Memory)) return false;
    for(FNPCLivingMemory& Existing:Record->Memories)
    {
        if(Existing.EventId==Memory.EventId && Existing.Kind==Memory.Kind)
        {
            if(Memory.EffectiveWeight<=Existing.EffectiveWeight) return false;
            Record->PersonalReputation=FMath::Clamp(Record->PersonalReputation-ReputationDelta(Existing)+ReputationDelta(Memory),-100,100);
            Existing=Memory;
            return true;
        }
    }
    Record->Memories.Add(Memory);
    Record->PersonalReputation=FMath::Clamp(Record->PersonalReputation+ReputationDelta(Memory),-100,100);
    return true;
}

float UNPCLivingWorldSubsystem::ConnectionTransfer(const ENPCConnectionKind Kind)
{
    switch(Kind)
    {
        case ENPCConnectionKind::Family:return .85f;
        case ENPCConnectionKind::Friend:return .70f;
        case ENPCConnectionKind::Professional:return .55f;
        case ENPCConnectionKind::Community:return .40f;
        default:return .40f;
    }
}

int32 UNPCLivingWorldSubsystem::PropagateMemory(const FName OriginNpcId,const FName EventId)
{
    FNPCLivingRecord* Origin=Records.Find(OriginNpcId);
    if(!Origin || EventId.IsNone()) return 0;
    const FNPCLivingMemory* Source=nullptr;
    for(const FNPCLivingMemory& M:Origin->Memories)
        if(M.EventId==EventId && (!Source || M.EffectiveWeight>Source->EffectiveWeight)) Source=&M;
    if(!Source || Source->EffectiveWeight<5.f) return 0;

    int32 Spread=0;
    TSet<FName> Targets;
    for(const FNPCSocialConnection& C:Origin->Connections)
    {
        FNPCLivingRecord* Target=Records.Find(C.OtherNpcId);
        if(!Target) continue;
        FNPCLivingMemory Report=*Source;
        Report.Kind=ENPCMemoryKind::ReportedEvent;
        Report.SourceNpcId=OriginNpcId;
        Report.OriginalWeight=FMath::Clamp(FMath::RoundToInt(Source->OriginalWeight*ConnectionTransfer(C.Kind)),1,100);
        Report.EffectiveWeight=FMath::Clamp(Source->EffectiveWeight*ConnectionTransfer(C.Kind),1.f,100.f);
        Report.bPermanent=Source->bPermanent && Report.EffectiveWeight>=50.f;
        if(RecordMemory(C.OtherNpcId,Report)){++Spread;Targets.Add(C.OtherNpcId);}
    }

    // Major events reach the local community even without an authored personal edge.
    if(Source->EffectiveWeight>=20.f && !Origin->CommunityId.IsNone())
    {
        for(TPair<FName,FNPCLivingRecord>& Pair:Records)
        {
            if(Pair.Key==OriginNpcId || Targets.Contains(Pair.Key) || Pair.Value.CommunityId!=Origin->CommunityId) continue;
            FNPCLivingMemory Report=*Source;
            Report.Kind=ENPCMemoryKind::ReportedEvent;
            Report.SourceNpcId=OriginNpcId;
            Report.OriginalWeight=FMath::Clamp(FMath::RoundToInt(Source->OriginalWeight*.35f),1,100);
            Report.EffectiveWeight=FMath::Clamp(Source->EffectiveWeight*.35f,1.f,100.f);
            Report.bPermanent=false;
            if(RecordMemory(Pair.Key,Report)) ++Spread;
        }
    }
    return Spread;
}

bool UNPCLivingWorldSubsystem::SimulateToGameMinute(const int64 GameMinute)
{
    if(GameMinute<LastSimulatedGameMinute) return false;
    const float Days=float(GameMinute-LastSimulatedGameMinute)/1440.f;
    if(Days<=0.f) return true;
    for(TPair<FName,FNPCLivingRecord>& Pair:Records)
    {
        FNPCLivingRecord& Record=Pair.Value;
        int32 Reputation=0;
        for(FNPCLivingMemory& Memory:Record.Memories)
        {
            if(!Memory.bPermanent && Memory.OriginalWeight<100)
                Memory.EffectiveWeight=FMath::Max(0.f,Memory.EffectiveWeight-Memory.DecayPerGameDay*Days);
            Reputation+=ReputationDelta(Memory);
        }
        Record.Memories.RemoveAll([](const FNPCLivingMemory& M){return !M.bPermanent && M.EffectiveWeight<.5f;});
        Record.PersonalReputation=FMath::Clamp(Reputation,-100,100);
    }
    LastSimulatedGameMinute=GameMinute;
    return true;
}

bool UNPCLivingWorldSubsystem::TryGetRecord(const FName NpcId,FNPCLivingRecord& OutRecord) const
{
    const FNPCLivingRecord* Record=Records.Find(NpcId);
    if(!Record) return false;
    OutRecord=*Record;
    return true;
}

FNPCLivingWorldSnapshot UNPCLivingWorldSubsystem::CaptureSnapshot() const
{
    FNPCLivingWorldSnapshot Snapshot;
    Snapshot.bValid=true;
    Snapshot.LastSimulatedGameMinute=LastSimulatedGameMinute;
    Snapshot.Records=Records;
    return Snapshot;
}

bool UNPCLivingWorldSubsystem::ValidateSnapshot(const FNPCLivingWorldSnapshot& Snapshot)
{
    if(!Snapshot.bValid || Snapshot.LastSimulatedGameMinute<0) return false;
    for(const TPair<FName,FNPCLivingRecord>& Pair:Snapshot.Records)
    {
        if(Pair.Key.IsNone() || Pair.Value.NpcId!=Pair.Key || Pair.Value.PersonalReputation<-100 ||
            Pair.Value.PersonalReputation>100 || Pair.Value.SpecificTrust<-100 || Pair.Value.SpecificTrust>100 ||
            static_cast<uint8>(Pair.Value.Mood)>static_cast<uint8>(ENPCLivingMood::Excited)) return false;
        TSet<FName> Connections;
        for (const FNPCSocialConnection& Connection : Pair.Value.Connections)
        {
            if (Connection.OtherNpcId == Pair.Key || !Snapshot.Records.Contains(Connection.OtherNpcId)
                || Connections.Contains(Connection.OtherNpcId)
                || static_cast<uint8>(Connection.Kind) > static_cast<uint8>(ENPCConnectionKind::Community)) return false;
            Connections.Add(Connection.OtherNpcId);
        }
        TSet<FName> Events;
        for(const FNPCLivingMemory& M:Pair.Value.Memories)
        {
            if(!IsValidMemory(M)) return false;
            const FName Composite(*FString::Printf(TEXT("%s.%d"),*M.EventId.ToString(),int32(M.Kind)));
            if(Events.Contains(Composite)) return false;
            Events.Add(Composite);
        }
    }
    return true;
}

bool UNPCLivingWorldSubsystem::RestoreSnapshot(const FNPCLivingWorldSnapshot& Snapshot)
{
    if (!ValidateSnapshot(Snapshot)) return false;
    Records=Snapshot.Records;
    LastSimulatedGameMinute=Snapshot.LastSimulatedGameMinute;
    return true;
}
