// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/MemoryThreadsSubsystem.h"

const FName UMemoryThreadsSubsystem::NamedDeadThreadId(TEXT("thread.cross.named-dead"));
const FName UMemoryThreadsSubsystem::EthanThreadId(TEXT("thread.cross.ethan"));

bool UMemoryThreadsSubsystem::RegisterNamedDeadMemory(
    const FName PersonId,
    const FName MemorialId,
    const ENamedDeadMemorialTradition Tradition)
{
    if (PersonId.IsNone() || MemorialId.IsNone() || NamedDeadMemories.Contains(PersonId))
    {
        return false;
    }

    FNamedDeadMemoryRecord Record;
    Record.PersonId = PersonId;
    Record.MemorialId = MemorialId;
    Record.Tradition = Tradition;
    NamedDeadMemories.Add(PersonId, Record);
    return true;
}

bool UMemoryThreadsSubsystem::RecordMemorialVisited(const FName PersonId)
{
    FNamedDeadMemoryRecord* Record = NamedDeadMemories.Find(PersonId);
    if (!Record || Record->bMemorialVisited)
    {
        return false;
    }

    Record->bMemorialVisited = true;
    return true;
}

bool UMemoryThreadsSubsystem::ResolveBjornStatus(const EBjornKnownStatus Status)
{
    if (Status == EBjornKnownStatus::Unresolved || BjornStatus != EBjornKnownStatus::Unresolved)
    {
        return false;
    }

    BjornStatus = Status;
    return true;
}

bool UMemoryThreadsSubsystem::RecordEthanPhysicalTrace(const FName TraceId)
{
    if (TraceId.IsNone() || EthanPhysicalTraces.Contains(TraceId))
    {
        return false;
    }

    EthanPhysicalTraces.Add(TraceId);
    if (EthanThreadState == EEthanThreadState::Dormant)
    {
        EthanThreadState = EEthanThreadState::TracesAccumulating;
    }
    return true;
}

bool UMemoryThreadsSubsystem::RecordDreamEthanVoiceHeard()
{
    if (bDreamEthanVoiceHeard)
    {
        return false;
    }

    bDreamEthanVoiceHeard = true;
    if (EthanThreadState == EEthanThreadState::Dormant)
    {
        EthanThreadState = EEthanThreadState::TracesAccumulating;
    }
    return true;
}

bool UMemoryThreadsSubsystem::ResolveEthanThreadAtChapterNine(const int32 CurrentChapter)
{
    if (CurrentChapter < 9 || EthanThreadState == EEthanThreadState::ChapterNineStructurallyResolved)
    {
        return false;
    }

    EthanThreadState = EEthanThreadState::ChapterNineStructurallyResolved;
    return true;
}
