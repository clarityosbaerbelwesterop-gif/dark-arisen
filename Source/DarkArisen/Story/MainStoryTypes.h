#pragma once

#include "CoreMinimal.h"
#include "MainStoryTypes.generated.h"

UENUM(BlueprintType)
enum class EMainMissionState : uint8 { Locked, Available, Active, Completed, Failed };

USTRUCT(BlueprintType)
struct FMainStoryMissionDefinition
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Chapter = 1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> Prerequisites;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> CompletionFacts;
};

USTRUCT(BlueprintType)
struct FMainStoryMissionRuntime
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FName MissionId;
    UPROPERTY(SaveGame, BlueprintReadOnly) EMainMissionState State = EMainMissionState::Locked;
};

USTRUCT(BlueprintType)
struct FCharacterProgressionState
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadWrite) int32 Level = 1;
    UPROPERTY(SaveGame, BlueprintReadWrite) int32 Experience = 0;
    UPROPERTY(SaveGame, BlueprintReadWrite) TArray<FName> Inventory;
    UPROPERTY(SaveGame, BlueprintReadWrite) TArray<FName> Equipment;
};

USTRUCT(BlueprintType)
struct FCrewRelationshipState
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadWrite) FName CharacterId;
    UPROPERTY(SaveGame, BlueprintReadWrite) FName Role;
    UPROPERTY(SaveGame, BlueprintReadWrite) int32 Trust = 0;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bMet = false;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bAvailable = false;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bRecruited = false;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bAboard = false;
};

/** Map-independent state for the authored Chapter 1/2 physical route. */
USTRUCT(BlueprintType)
struct FOpeningProgressState
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) uint8 Location = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) uint8 RaidState = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) uint8 RecoveryState = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) TSet<FName> FamilyInteractions;
    UPROPERTY(SaveGame, BlueprintReadOnly) FName BoardingEncounterId;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 BoardersRequired = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 BoardersDefeated = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bDravenBoarded = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bLaLiberacionBoarded = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bLaLiberacionHelmSecured = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bLaLiberacionClearedHarbor = false;
};
