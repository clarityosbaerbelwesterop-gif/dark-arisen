// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/QuestJournalComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RexaM2MissionCatalog.generated.h"

UENUM(BlueprintType)
enum class EQuestStructuralTier : uint8
{
    Turn,
    Standing
};

UENUM(BlueprintType)
enum class EStandingMissionType : uint8
{
    None,
    Escort,
    ConvoyRaid,
    Recovery,
    Champion,
    Transport,
    PrivateerCommission,
    RoadWork,
    Hunt,
    Salvage
};

USTRUCT(BlueprintType)
struct FLocalDirectionVariant
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions")
    FName SpeakerKnowledgeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions")
    FName EntryId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions", meta = (MultiLine = true))
    FText JakeText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions", meta = (MultiLine = true))
    FText SpokenDirections;

    /** Local knowledge may be incomplete without being malicious. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions")
    bool bDistorted = false;
};

USTRUCT(BlueprintType)
struct FRexaM2MissionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    FQuestActivationDefinition Activation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    EQuestStructuralTier Tier = EQuestStructuralTier::Turn;

    /** Stable authored pool member. A Standing variant is consumed; it is never generated. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    FName AuthoredVariantId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    EStandingMissionType StandingType = EStandingMissionType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Directions")
    TArray<FLocalDirectionVariant> LocalDirections;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Outcome")
    TArray<FName> AuthoredOutcomeIds;
};

/**
 * The four finite authored missions required by the M2 Rexa proof slice.
 * This catalog owns no markers, objectives, notifications, procedural variants or disk writes.
 */
UCLASS()
class DARKARISEN_API URexaM2MissionCatalog : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Quest|Rexa")
    static TArray<FRexaM2MissionDefinition> GetAuthoredMissions();

    UFUNCTION(BlueprintCallable, Category = "Quest|Rexa")
    static bool RegisterAuthoredMissions(UQuestJournalComponent* Journal);

    /** Appends what a specific local actually said; it never resolves navigation for the player. */
    UFUNCTION(BlueprintCallable, Category = "Quest|Rexa|Directions")
    static bool AppendLocalDirection(
        UQuestJournalComponent* Journal,
        FName QuestId,
        FName SpeakerKnowledgeId,
        int64 GameMinute);

    static bool IsCatalogValid(const TArray<FRexaM2MissionDefinition>& Missions);
};
