// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/QuestJournalComponent.h"

UQuestJournalComponent::UQuestJournalComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UQuestJournalComponent::BeginPlay()
{
    Super::BeginPlay();
    DefinitionById.Empty();
    for (const FQuestActivationDefinition& Definition : AuthoredDefinitions)
    {
        RegisterQuestDefinition(Definition);
    }
}

bool UQuestJournalComponent::RegisterQuestDefinition(
    const FQuestActivationDefinition& Definition)
{
    if (Definition.QuestId.IsNone() ||
        Definition.Trigger == EQuestActivationTrigger::Count ||
        DefinitionById.Contains(Definition.QuestId) ||
        Definition.ExpirationDurationMinutes < -1 ||
        Definition.ExpirationDurationMinutes == 0 ||
        (Definition.bRequiresSpokenAgreement &&
            Definition.Trigger != EQuestActivationTrigger::Conversation) ||
        (!Definition.bSilentAtActivation &&
            (Definition.InitialEntryId.IsNone() || Definition.InitialJakeText.IsEmpty())))
        return false;

    for (const FName ExcludedQuestId : Definition.MutuallyExclusiveQuestIds)
    {
        if (ExcludedQuestId.IsNone() || ExcludedQuestId == Definition.QuestId) return false;
    }
    for (const TPair<FName, FQuestActivationDefinition>& Existing : DefinitionById)
    {
        if (!Definition.InitialEntryId.IsNone() &&
            Definition.InitialEntryId == Existing.Value.InitialEntryId) return false;
    }

    DefinitionById.Add(Definition.QuestId, Definition);
    if (!QuestStates.Contains(Definition.QuestId))
    {
        FQuestRuntimeState RuntimeState;
        RuntimeState.QuestId = Definition.QuestId;
        QuestStates.Add(Definition.QuestId, RuntimeState);
    }
    return true;
}

bool UQuestJournalComponent::ActivateQuest(
    const FName QuestId,
    const EQuestActivationTrigger Trigger,
    const int64 GameMinute,
    const bool bSpokenAgreementConfirmed)
{
    const FQuestActivationDefinition* Definition = FindDefinition(QuestId);
    FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!Definition || !RuntimeState || RuntimeState->Lifecycle != EQuestLifecycle::Dormant ||
        Trigger != Definition->Trigger || Trigger == EQuestActivationTrigger::Count ||
        GameMinute < 0 ||
        (Definition->bRequiresSpokenAgreement && !bSpokenAgreementConfirmed)) return false;

    if (Definition->ExpirationDurationMinutes > 0 &&
        Definition->ExpirationDurationMinutes > MAX_int64 - GameMinute) return false;

    RuntimeState->Lifecycle = EQuestLifecycle::Active;
    RuntimeState->ActivationTrigger = Trigger;
    RuntimeState->ActivationGameMinute = GameMinute;
    RuntimeState->ExpirationGameMinute = Definition->ExpirationDurationMinutes > 0
        ? GameMinute + Definition->ExpirationDurationMinutes : -1;
    RuntimeState->ResolutionGameMinute = -1;
    RuntimeState->OutcomeId = NAME_None;

    if (!Definition->bSilentAtActivation && !AppendJournalEntryInternal(
        QuestId,
        Definition->InitialEntryId,
        Definition->InitialJakeText,
        Definition->InitialDirections,
        GameMinute,
        Definition->bInitialEntryDistorted,
        false))
    {
        *RuntimeState = FQuestRuntimeState();
        RuntimeState->QuestId = QuestId;
        return false;
    }

    ApplyMutualExclusions(*Definition);
    return true;
}

bool UQuestJournalComponent::RevealSilentQuest(
    const FName QuestId,
    const FName EntryId,
    const FText& JakeText,
    const FText& Directions,
    const int64 GameMinute,
    const bool bDistorted)
{
    const FQuestActivationDefinition* Definition = FindDefinition(QuestId);
    const FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!Definition || !Definition->bSilentAtActivation || !RuntimeState ||
        RuntimeState->Lifecycle != EQuestLifecycle::Active ||
        HasJournalEntryForQuest(QuestId)) return false;
    return AppendJournalEntryInternal(
        QuestId, EntryId, JakeText, Directions, GameMinute, bDistorted, false);
}

bool UQuestJournalComponent::AppendJournalNote(
    const FName QuestId,
    const FName EntryId,
    const FText& JakeText,
    const FText& Directions,
    const int64 GameMinute,
    const bool bDistorted)
{
    const FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!RuntimeState || (RuntimeState->Lifecycle != EQuestLifecycle::Active &&
        RuntimeState->Lifecycle != EQuestLifecycle::Resolved)) return false;
    return AppendJournalEntryInternal(
        QuestId, EntryId, JakeText, Directions, GameMinute, bDistorted, false);
}

bool UQuestJournalComponent::AppendJournalCorrection(
    const FName QuestId,
    const FName EntryId,
    const FText& JakeText,
    const FText& Directions,
    const int64 GameMinute)
{
    bool bHasDistortedEntry = false;
    for (const FQuestJournalEntry& Entry : JournalEntries)
    {
        if (Entry.QuestId == QuestId && Entry.bDistorted)
        {
            bHasDistortedEntry = true;
            break;
        }
    }
    if (!bHasDistortedEntry) return false;

    const FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!RuntimeState || (RuntimeState->Lifecycle != EQuestLifecycle::Active &&
        RuntimeState->Lifecycle != EQuestLifecycle::Resolved)) return false;
    return AppendJournalEntryInternal(
        QuestId, EntryId, JakeText, Directions, GameMinute, false, true);
}

bool UQuestJournalComponent::ResolveQuest(
    const FName QuestId,
    const FName OutcomeId,
    const int64 GameMinute)
{
    FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!RuntimeState || RuntimeState->Lifecycle != EQuestLifecycle::Active ||
        OutcomeId.IsNone() || GameMinute < RuntimeState->ActivationGameMinute) return false;
    RuntimeState->Lifecycle = EQuestLifecycle::Resolved;
    RuntimeState->ResolutionGameMinute = GameMinute;
    RuntimeState->OutcomeId = OutcomeId;
    return true;
}

int32 UQuestJournalComponent::ProcessExpirations(const int64 GameMinute)
{
    if (GameMinute < 0) return 0;
    static const FName ExpiredOutcomeId(TEXT("Expired"));
    int32 ResolvedCount = 0;
    for (TPair<FName, FQuestRuntimeState>& Pair : QuestStates)
    {
        FQuestRuntimeState& RuntimeState = Pair.Value;
        if (RuntimeState.Lifecycle != EQuestLifecycle::Active ||
            RuntimeState.ExpirationGameMinute < 0 ||
            RuntimeState.ExpirationGameMinute > GameMinute) continue;
        RuntimeState.Lifecycle = EQuestLifecycle::Resolved;
        RuntimeState.ResolutionGameMinute = GameMinute;
        RuntimeState.OutcomeId = ExpiredOutcomeId;
        ++ResolvedCount;
    }
    return ResolvedCount;
}

bool UQuestJournalComponent::TryGetQuestState(
    const FName QuestId,
    FQuestRuntimeState& OutState) const
{
    const FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!RuntimeState) return false;
    OutState = *RuntimeState;
    return true;
}

TArray<FQuestJournalEntry> UQuestJournalComponent::SearchJournal(const FString& Query) const
{
    const FString Needle = Query.TrimStartAndEnd();
    if (Needle.IsEmpty()) return JournalEntries;

    TArray<FQuestJournalEntry> Matches;
    for (const FQuestJournalEntry& Entry : JournalEntries)
    {
        if (Entry.JakeText.ToString().Contains(Needle, ESearchCase::IgnoreCase) ||
            Entry.Directions.ToString().Contains(Needle, ESearchCase::IgnoreCase))
            Matches.Add(Entry);
    }
    return Matches;
}

bool UQuestJournalComponent::HasJournalEntryForQuest(const FName QuestId) const
{
    return JournalEntries.ContainsByPredicate([QuestId](const FQuestJournalEntry& Entry)
    {
        return Entry.QuestId == QuestId;
    });
}

const FQuestActivationDefinition* UQuestJournalComponent::FindDefinition(
    const FName QuestId) const
{
    return DefinitionById.Find(QuestId);
}

void UQuestJournalComponent::ApplyMutualExclusions(
    const FQuestActivationDefinition& Definition)
{
    const auto MarkUnavailable = [this](const FName QuestId)
    {
        FQuestRuntimeState* State = QuestStates.Find(QuestId);
        if (State && State->Lifecycle == EQuestLifecycle::Dormant)
            State->Lifecycle = EQuestLifecycle::Unavailable;
    };
    for (const FName ExcludedQuestId : Definition.MutuallyExclusiveQuestIds)
    {
        MarkUnavailable(ExcludedQuestId);
    }
    for (const TPair<FName, FQuestActivationDefinition>& Pair : DefinitionById)
    {
        if (Pair.Value.MutuallyExclusiveQuestIds.Contains(Definition.QuestId))
            MarkUnavailable(Pair.Key);
    }
}

bool UQuestJournalComponent::AppendJournalEntryInternal(
    const FName QuestId,
    const FName EntryId,
    const FText& JakeText,
    const FText& Directions,
    const int64 GameMinute,
    const bool bDistorted,
    const bool bCorrection)
{
    const FQuestRuntimeState* RuntimeState = QuestStates.Find(QuestId);
    if (!RuntimeState || EntryId.IsNone() || JakeText.IsEmpty() || GameMinute < 0 ||
        GameMinute < RuntimeState->ActivationGameMinute || NextJournalSequence == MAX_int64 ||
        JournalEntries.ContainsByPredicate([EntryId](const FQuestJournalEntry& Entry)
        {
            return Entry.EntryId == EntryId;
        })) return false;

    FQuestJournalEntry Entry;
    Entry.EntryId = EntryId;
    Entry.QuestId = QuestId;
    Entry.JakeText = JakeText;
    Entry.Directions = Directions;
    Entry.GameMinute = GameMinute;
    Entry.Sequence = NextJournalSequence++;
    Entry.bDistorted = bDistorted;
    Entry.bCorrection = bCorrection;
    JournalEntries.Add(MoveTemp(Entry));
    return true;
}


FQuestJournalSnapshot UQuestJournalComponent::CaptureSnapshot() const
{
    FQuestJournalSnapshot Snapshot;
    Snapshot.bValid=true;
    Snapshot.QuestStates=QuestStates;
    Snapshot.JournalEntries=JournalEntries;
    Snapshot.NextJournalSequence=NextJournalSequence;
    return Snapshot;
}

bool UQuestJournalComponent::RestoreSnapshot(const FQuestJournalSnapshot& Snapshot)
{
    if(!Snapshot.bValid || Snapshot.NextJournalSequence<0) return false;
    for(const TPair<FName,FQuestRuntimeState>& Pair:Snapshot.QuestStates)
    {
        if(Pair.Key.IsNone() || Pair.Value.QuestId!=Pair.Key || !DefinitionById.Contains(Pair.Key))
            return false;
    }
    TSet<FName> EntryIds;
    int64 MaxSequence=-1;
    for(const FQuestJournalEntry& Entry:Snapshot.JournalEntries)
    {
        if(Entry.EntryId.IsNone() || Entry.QuestId.IsNone() || !Snapshot.QuestStates.Contains(Entry.QuestId) ||
            Entry.Sequence<0 || EntryIds.Contains(Entry.EntryId)) return false;
        EntryIds.Add(Entry.EntryId);
        MaxSequence=FMath::Max(MaxSequence,Entry.Sequence);
    }
    if(Snapshot.NextJournalSequence<=MaxSequence) return false;
    QuestStates=Snapshot.QuestStates;
    JournalEntries=Snapshot.JournalEntries;
    NextJournalSequence=Snapshot.NextJournalSequence;
    return true;
}
