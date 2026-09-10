// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/QuestJournalComponent.h"
#include "MainStoryDirectorComponent.generated.h"

class UQuestJournalComponent;

/**
 * Save-backed owner for the finite 34-mission critical path.
 * It advances only the exact next authored mission and never manufactures recovery quests.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UMainStoryDirectorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMainStoryDirectorComponent();

    UFUNCTION(BlueprintPure, Category = "Story")
    FName GetCurrentMissionId() const;

    UFUNCTION(BlueprintPure, Category = "Story")
    int32 GetCurrentChapter() const;

    UFUNCTION(BlueprintPure, Category = "Story")
    bool IsStoryComplete() const;

    UFUNCTION(BlueprintPure, Category = "Story")
    bool IsStoryFactSet(FName FactId) const;

    UFUNCTION(BlueprintPure, Category = "Story")
    bool HasValidStoryState() const;

    UFUNCTION(BlueprintCallable, Category = "Story")
    bool TryActivateCurrentMission(
        UQuestJournalComponent* Journal,
        EQuestActivationTrigger Trigger,
        int64 GameMinute,
        bool bSpokenAgreementConfirmed = false);

    UFUNCTION(BlueprintCallable, Category = "Story")
    bool TryResolveCurrentMission(
        UQuestJournalComponent* Journal,
        FName OutcomeId,
        int64 GameMinute);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Story")
    int32 CurrentMissionIndex = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Story")
    TSet<FName> StoryFacts;
};
