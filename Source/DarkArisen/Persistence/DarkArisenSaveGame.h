#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Story/MainStoryTypes.h"
#include "DarkArisenSaveGame.generated.h"

UCLASS()
class DARKARISEN_API UDarkArisenSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    static constexpr int32 CurrentVersion = 2;
    UPROPERTY(SaveGame) int32 SaveVersion = CurrentVersion;
    UPROPERTY(SaveGame) FName CurrentMission;
    UPROPERTY(SaveGame) int32 CurrentChapter = 1;
    UPROPERTY(SaveGame) TArray<FMainStoryMissionRuntime> MissionStates;
    UPROPERTY(SaveGame) TSet<FName> StoryFacts;
    UPROPERTY(SaveGame) TSet<FName> WorldFacts;
    UPROPERTY(SaveGame) FCharacterProgressionState CharacterProgression;
    UPROPERTY(SaveGame) TArray<FCrewRelationshipState> Crew;
    UPROPERTY(SaveGame) FName CurrentShip;
    UPROPERTY(SaveGame) TSet<FName> DefeatedBosses;
    UPROPERTY(SaveGame) TSet<FName> ImportantShortcuts;
    UPROPERTY(SaveGame) TMap<FName,FName> QuestOutcomes;
    UPROPERTY(SaveGame) FName CheckpointId;
    UPROPERTY(SaveGame) FName SpawnId;
    UPROPERTY(SaveGame) FOpeningProgressState OpeningProgress;
};
