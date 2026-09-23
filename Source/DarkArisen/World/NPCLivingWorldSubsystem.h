#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NPCLivingWorldSubsystem.generated.h"

UENUM(BlueprintType)
enum class ENPCMemoryKind : uint8 { DirectEncounter, WitnessedEvent, ReportedEvent, CulturalReputation };
UENUM(BlueprintType)
enum class ENPCMemoryEmotion : uint8 { Negative, Neutral, Positive };
UENUM(BlueprintType)
enum class ENPCLivingMood : uint8 { Content, Stressed, Happy, Sad, Angry, Excited };
UENUM(BlueprintType)
enum class ENPCConnectionKind : uint8 { Family, Friend, Professional, Community };

USTRUCT(BlueprintType)
struct FNPCLivingMemory
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) FName EventId;
    UPROPERTY(SaveGame) ENPCMemoryKind Kind=ENPCMemoryKind::DirectEncounter;
    UPROPERTY(SaveGame) ENPCMemoryEmotion Emotion=ENPCMemoryEmotion::Neutral;
    UPROPERTY(SaveGame) int32 OriginalWeight=1;
    UPROPERTY(SaveGame) float EffectiveWeight=1.f;
    UPROPERTY(SaveGame) int64 OccurredAtGameMinute=0;
    UPROPERTY(SaveGame) float DecayPerGameDay=0.f;
    UPROPERTY(SaveGame) FName SourceNpcId;
    UPROPERTY(SaveGame) bool bPermanent=false;
};

USTRUCT(BlueprintType)
struct FNPCSocialConnection
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) FName OtherNpcId;
    UPROPERTY(SaveGame) ENPCConnectionKind Kind=ENPCConnectionKind::Community;
};

USTRUCT(BlueprintType)
struct FNPCLivingRecord
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) FName NpcId;
    UPROPERTY(SaveGame) FName CommunityId;
    UPROPERTY(SaveGame) FName CurrentScheduleAnchorId;
    UPROPERTY(SaveGame) int32 PersonalReputation=0;
    UPROPERTY(SaveGame) int32 SpecificTrust=0;
    UPROPERTY(SaveGame) ENPCLivingMood Mood=ENPCLivingMood::Content;
    UPROPERTY(SaveGame) TArray<FNPCLivingMemory> Memories;
    UPROPERTY(SaveGame) TArray<FNPCSocialConnection> Connections;
};

USTRUCT(BlueprintType)
struct FNPCLivingWorldSnapshot
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) bool bValid=false;
    UPROPERTY(SaveGame) int64 LastSimulatedGameMinute=0;
    UPROPERTY(SaveGame) TMap<FName,FNPCLivingRecord> Records;
};

/**
 * Persistent living-population authority.
 * It stores only authored IDs/events and deterministic consequences; it never generates quests,
 * dialogue, NPC identities or lore. Physical actors remain owned by their regional systems.
 */
UCLASS()
class DARKARISEN_API UNPCLivingWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool RegisterNPC(FName NpcId,FName CommunityId);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool UpdateScheduleAnchor(FName NpcId,FName AnchorId);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool SetMood(FName NpcId,ENPCLivingMood Mood);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool ConnectNPCs(FName A,FName B,ENPCConnectionKind Kind);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool RecordMemory(FName NpcId,const FNPCLivingMemory& Memory);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    int32 PropagateMemory(FName OriginNpcId,FName EventId);
    UFUNCTION(BlueprintCallable,Category="NPC|LivingWorld")
    bool SimulateToGameMinute(int64 GameMinute);
    UFUNCTION(BlueprintPure,Category="NPC|LivingWorld")
    bool TryGetRecord(FName NpcId,FNPCLivingRecord& OutRecord) const;
    UFUNCTION(BlueprintCallable,Category="NPC|Persistence")
    FNPCLivingWorldSnapshot CaptureSnapshot() const;
    UFUNCTION(BlueprintCallable,Category="NPC|Persistence")
    bool RestoreSnapshot(const FNPCLivingWorldSnapshot& Snapshot);
    static bool ValidateSnapshot(const FNPCLivingWorldSnapshot& Snapshot);

private:
    UPROPERTY() TMap<FName,FNPCLivingRecord> Records;
    int64 LastSimulatedGameMinute=0;
    static bool IsValidMemory(const FNPCLivingMemory& Memory);
    static int32 ReputationDelta(const FNPCLivingMemory& Memory);
    static float ConnectionTransfer(ENPCConnectionKind Kind);
    static void UpsertConnection(FNPCLivingRecord& Record,FName Other,ENPCConnectionKind Kind);
};
