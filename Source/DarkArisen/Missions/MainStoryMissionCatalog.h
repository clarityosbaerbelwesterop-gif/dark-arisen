// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/QuestJournalComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainStoryMissionCatalog.generated.h"

UENUM(BlueprintType)
enum class EMainStoryControlOwnership : uint8
{
    Playable,
    BriefAuthoredTakeover,
    Hybrid
};

USTRUCT(BlueprintType)
struct FMainStoryMissionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    FQuestActivationDefinition Activation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    int32 Chapter = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    int32 SequenceInChapter = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    FName RouteId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    TArray<FName> RequiredBeatIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    TArray<FName> AllowedOutcomeIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    TArray<FName> PersistenceFlags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    EMainStoryControlOwnership ControlOwnership = EMainStoryControlOwnership::Playable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
    FName PresentationSlotId;
};

/**
 * Current finite critical-path authority. The catalog deliberately owns no radiant generation,
 * navigation markers, objective HUD, checkpoint rewind or legacy Ethan-boss branch.
 */
UCLASS()
class DARKARISEN_API UMainStoryMissionCatalog : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Story")
    static TArray<FMainStoryMissionDefinition> GetAuthoredMissions();

    UFUNCTION(BlueprintCallable, Category = "Story")
    static bool RegisterAuthoredMissions(UQuestJournalComponent* Journal);

    UFUNCTION(BlueprintPure, Category = "Story")
    static bool TryGetMission(FName QuestId, FMainStoryMissionDefinition& OutMission);

    static bool IsCatalogValid(const TArray<FMainStoryMissionDefinition>& Missions);
};
