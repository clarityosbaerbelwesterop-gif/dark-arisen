// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestJournalComponent.generated.h"

UENUM(BlueprintType)
enum class EQuestActivationTrigger : uint8
{
    Conversation,
    Overheard,
    Object,
    Proximity,
    WorldState,
    Absence,
    Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EQuestLifecycle : uint8
{
    Dormant,
    Active,
    Resolved,
    Unavailable
};

USTRUCT(BlueprintType)
struct FQuestActivationDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EQuestActivationTrigger Trigger = EQuestActivationTrigger::Conversation;

    /** Nineteen authored quests begin without a journal entry. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bSilentAtActivation = false;

    /** Spoken contract agreement; still never represented by an accept button. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bRequiresSpokenAgreement = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Journal")
    FName InitialEntryId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Journal", meta = (MultiLine = true))
    FText InitialJakeText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Journal", meta = (MultiLine = true))
    FText InitialDirections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Journal")
    bool bInitialEntryDistorted = false;

    /** Negative means this quest does not expire; the value is never exposed as a countdown. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Time")
    int64 ExpirationDurationMinutes = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Exclusion")
    TArray<FName> MutuallyExclusiveQuestIds;
};

USTRUCT(BlueprintType)
struct FQuestRuntimeState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest")
    FName QuestId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest")
    EQuestLifecycle Lifecycle = EQuestLifecycle::Dormant;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest")
    EQuestActivationTrigger ActivationTrigger = EQuestActivationTrigger::Conversation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest|Time")
    int64 ActivationGameMinute = -1;

    /** Internal world-time deadline; deliberately unavailable to Blueprint/UI countdowns. */
    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Quest|Time")
    int64 ExpirationGameMinute = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest|Time")
    int64 ResolutionGameMinute = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Quest")
    FName OutcomeId;
};

USTRUCT(BlueprintType)
struct FQuestJournalEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    FName EntryId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    FName QuestId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    FText JakeText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    FText Directions;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    int64 GameMinute = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    int64 Sequence = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    bool bDistorted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Journal")
    bool bCorrection = false;
};

USTRUCT(BlueprintType)
struct FQuestJournalSnapshot
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) bool bValid=false;
    UPROPERTY(SaveGame) TMap<FName,FQuestRuntimeState> QuestStates;
    UPROPERTY(SaveGame) TArray<FQuestJournalEntry> JournalEntries;
    UPROPERTY(SaveGame) int64 NextJournalSequence=0;
};

/**
 * Markerless quest state and Jake's chronological notebook.
 * Activation deliberately emits no screen, sound, camera, marker, or reminder event.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UQuestJournalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UQuestJournalComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Quest|Authoring")
    bool RegisterQuestDefinition(const FQuestActivationDefinition& Definition);

    UFUNCTION(BlueprintCallable, Category = "Quest|State")
    bool ActivateQuest(FName QuestId, EQuestActivationTrigger Trigger,
        int64 GameMinute, bool bSpokenAgreementConfirmed);

    UFUNCTION(BlueprintCallable, Category = "Quest|Journal")
    bool RevealSilentQuest(FName QuestId, FName EntryId, const FText& JakeText,
        const FText& Directions, int64 GameMinute, bool bDistorted);

    UFUNCTION(BlueprintCallable, Category = "Quest|Journal")
    bool AppendJournalNote(FName QuestId, FName EntryId, const FText& JakeText,
        const FText& Directions, int64 GameMinute, bool bDistorted);

    UFUNCTION(BlueprintCallable, Category = "Quest|Journal")
    bool AppendJournalCorrection(FName QuestId, FName EntryId, const FText& JakeText,
        const FText& Directions, int64 GameMinute);

    UFUNCTION(BlueprintCallable, Category = "Quest|State")
    bool ResolveQuest(FName QuestId, FName OutcomeId, int64 GameMinute);

    /** Called by the future world-time owner; returns no player-facing warning or result card. */
    UFUNCTION(BlueprintCallable, Category = "Quest|Time")
    int32 ProcessExpirations(int64 GameMinute);

    UFUNCTION(BlueprintPure, Category = "Quest|State")
    bool TryGetQuestState(FName QuestId, FQuestRuntimeState& OutState) const;

    UFUNCTION(BlueprintPure, Category = "Quest|Journal")
    TArray<FQuestJournalEntry> GetJournalEntries() const { return JournalEntries; }

    /** The physical notebook's sole tracking concession: plain text search. */
    UFUNCTION(BlueprintPure, Category = "Quest|Journal")
    TArray<FQuestJournalEntry> SearchJournal(const FString& Query) const;

    UFUNCTION(BlueprintPure, Category = "Quest|Journal")
    bool HasJournalEntryForQuest(FName QuestId) const;

    UFUNCTION(BlueprintCallable, Category = "Quest|Persistence")
    FQuestJournalSnapshot CaptureSnapshot() const;
    UFUNCTION(BlueprintCallable, Category = "Quest|Persistence")
    bool RestoreSnapshot(const FQuestJournalSnapshot& Snapshot);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest|Authoring")
    TArray<FQuestActivationDefinition> AuthoredDefinitions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Quest|State")
    TMap<FName, FQuestRuntimeState> QuestStates;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Quest|Journal")
    TArray<FQuestJournalEntry> JournalEntries;

private:
    TMap<FName, FQuestActivationDefinition> DefinitionById;

    UPROPERTY(SaveGame)
    int64 NextJournalSequence = 0;

    const FQuestActivationDefinition* FindDefinition(FName QuestId) const;
    void ApplyMutualExclusions(const FQuestActivationDefinition& Definition);
    bool AppendJournalEntryInternal(FName QuestId, FName EntryId, const FText& JakeText,
        const FText& Directions, int64 GameMinute, bool bDistorted, bool bCorrection);
};
