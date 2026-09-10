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
    UPROPERTY(SaveGame, BlueprintReadWrite) int32 Trust = 0;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bRecruited = false;
    UPROPERTY(SaveGame, BlueprintReadWrite) bool bAboard = false;
};
