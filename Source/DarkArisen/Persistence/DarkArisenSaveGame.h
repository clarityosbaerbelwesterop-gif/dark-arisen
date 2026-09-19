#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Story/MainStoryTypes.h"
#include "Ship/ShipVoyageComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"
#include "Components/QuestJournalComponent.h"
#include "Systems/ProgressionEconomyComponent.h"
#include "World/NPCLivingWorldSubsystem.h"
#include "DarkArisenSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerRuntimeSnapshot
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) bool bValid = false;
    UPROPERTY(SaveGame) FName MissionId;
    /** Legacy snapshots without a source level use the authored arrival spawn. */
    UPROPERTY(SaveGame) FName SourceLevel;
    UPROPERTY(SaveGame) FTransform Transform = FTransform::Identity;
    UPROPERTY(SaveGame) float HealthFraction = 1.0f;
    UPROPERTY(SaveGame) float StaminaFraction = 1.0f;
};

UCLASS()
class DARKARISEN_API UDarkArisenSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    static constexpr int32 CurrentVersion = 9;
    UPROPERTY(SaveGame) int32 SaveVersion = CurrentVersion;
    UPROPERTY(SaveGame) FName CurrentMission;
    UPROPERTY(SaveGame) int32 CurrentChapter = 1;
    UPROPERTY(SaveGame) TArray<FMainStoryMissionRuntime> MissionStates;
    UPROPERTY(SaveGame) TSet<FName> StoryFacts;
    UPROPERTY(SaveGame) TSet<FName> WorldFacts;
    UPROPERTY(SaveGame) FCharacterProgressionState CharacterProgression;
    UPROPERTY(SaveGame) FPlayerRuntimeSnapshot PlayerRuntime;
    UPROPERTY(SaveGame) TArray<FCrewRelationshipState> Crew;
    UPROPERTY(SaveGame) FName CurrentShip;
    UPROPERTY(SaveGame) FShipVoyageSnapshot LaLiberacionVoyage;
    UPROPERTY(SaveGame) FDarkArisenWorldRulesSnapshot WorldRules;
    UPROPERTY(SaveGame) FQuestJournalSnapshot QuestJournal;
    UPROPERTY(SaveGame) FProgressionEconomySnapshot ProgressionEconomy;
    UPROPERTY(SaveGame) FNPCLivingWorldSnapshot LivingNPCWorld;
    UPROPERTY(SaveGame) TSet<FName> DefeatedBosses;
    UPROPERTY(SaveGame) TSet<FName> ImportantShortcuts;
    UPROPERTY(SaveGame) TSet<FName> DiscoveredDungeons;
    UPROPERTY(SaveGame) TSet<FName> CompletedDungeons;
    UPROPERTY(SaveGame) TSet<FName> RecoveredTreasures;
    UPROPERTY(SaveGame) TMap<FName,FName> QuestOutcomes;
    UPROPERTY(SaveGame) FName CheckpointId;
    UPROPERTY(SaveGame) FName SpawnId;
    UPROPERTY(SaveGame) FOpeningProgressState OpeningProgress;
};
