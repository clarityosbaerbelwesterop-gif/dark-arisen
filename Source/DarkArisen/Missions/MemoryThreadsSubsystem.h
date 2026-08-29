// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MemoryThreadsSubsystem.generated.h"

UENUM(BlueprintType)
enum class ENamedDeadMemorialTradition : uint8
{
    ImperialChristian,
    RexanAncestor,
    FjordlundSeaAndSaga,
    AshenmoorMountain,
    SailorPirate
};

UENUM(BlueprintType)
enum class EBjornKnownStatus : uint8
{
    Unresolved,
    Dead,
    AliveCaptive,
    LostToDarkness
};

UENUM(BlueprintType)
enum class EEthanThreadState : uint8
{
    Dormant,
    TracesAccumulating,
    ChapterNineStructurallyResolved
};

USTRUCT(BlueprintType)
struct FNamedDeadMemoryRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName PersonId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName MemorialId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    ENamedDeadMemorialTradition Tradition = ENamedDeadMemorialTradition::SailorPirate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    bool bMemorialVisited = false;
};

/**
 * Cross-regional source owner for Thread #16 The Named Dead and Thread #17 Ethan.
 *
 * The Named Dead deliberately has no ending. Ethan's optional Thread structurally resolves in
 * Chapter 9 whether pursued or not, while the character/grief itself explicitly never receives a
 * closure mechanic. This subsystem records traces and memorial facts; it does not generate eulogies,
 * reflection lines, graves, rewards or retroactive certainty.
 */
UCLASS()
class DARKARISEN_API UMemoryThreadsSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    static const FName NamedDeadThreadId;
    static const FName EthanThreadId;

    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Named Dead")
    bool RegisterNamedDeadMemory(FName PersonId, FName MemorialId, ENamedDeadMemorialTradition Tradition);

    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Named Dead")
    bool RecordMemorialVisited(FName PersonId);

    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Named Dead|Bjorn")
    bool ResolveBjornStatus(EBjornKnownStatus Status);

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Named Dead|Bjorn")
    EBjornKnownStatus GetBjornStatus() const { return BjornStatus; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Named Dead|Bjorn")
    bool CanConfirmBjornStatus() const { return BjornStatus != EBjornKnownStatus::Unresolved; }

    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Ethan")
    bool RecordEthanPhysicalTrace(FName TraceId);

    /** Dream-Ethan is a projection; this records the one authored voice occurrence only. */
    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Ethan")
    bool RecordDreamEthanVoiceHeard();

    /** Chapter 9 resolves the Thread structurally even if optional trace collection was ignored. */
    UFUNCTION(BlueprintCallable, Category="Quest|Memory|Ethan")
    bool ResolveEthanThreadAtChapterNine(int32 CurrentChapter);

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Ethan")
    EEthanThreadState GetEthanThreadState() const { return EthanThreadState; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Ethan")
    int32 GetEthanTraceCount() const { return EthanPhysicalTraces.Num(); }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Design Law")
    bool NamedDeadThreadHasEnding() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Design Law")
    bool EthanHasClosureMechanic() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Design Law")
    bool EthanHasSecretFinalLetter() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Design Law")
    bool AllowsClearEthanFlashbackFace() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Memory|Design Law")
    bool AwardsMemorialGameplayReward() const { return false; }

private:
    UPROPERTY(SaveGame)
    TMap<FName, FNamedDeadMemoryRecord> NamedDeadMemories;

    UPROPERTY(SaveGame)
    EBjornKnownStatus BjornStatus = EBjornKnownStatus::Unresolved;

    UPROPERTY(SaveGame)
    TSet<FName> EthanPhysicalTraces;

    UPROPERTY(SaveGame)
    bool bDreamEthanVoiceHeard = false;

    UPROPERTY(SaveGame)
    EEthanThreadState EthanThreadState = EEthanThreadState::Dormant;
};
