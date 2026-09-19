#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Persistence/DarkArisenSaveGame.h"
#include "MainStorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMainMissionChanged,FName,MissionId,EMainMissionState,State);

/** Persistent native authority for story, world, crew, progression and save data. */
UCLASS()
class DARKARISEN_API UMainStorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    UFUNCTION(BlueprintCallable) void NewGame();
    UFUNCTION(BlueprintCallable) bool Save(const FString& SlotName,int32 UserIndex);
    UFUNCTION(BlueprintCallable) bool Load(const FString& SlotName,int32 UserIndex);
    UFUNCTION(BlueprintCallable) void ResetForNewGame();
    UFUNCTION(BlueprintCallable) bool ActivateMission(FName MissionId);
    UFUNCTION(BlueprintCallable) bool CompleteMission(FName MissionId);
    /** Physical/authored mission completion path. Adds the canonical persistent mission facts on the existing save authority. */
    UFUNCTION(BlueprintCallable) bool CompleteAuthoredMission(FName MissionId);
    UFUNCTION(BlueprintCallable) bool FailMission(FName MissionId);
    UFUNCTION(BlueprintCallable) bool SetWorldFact(FName Fact,bool bEnabled=true);
    UFUNCTION(BlueprintCallable) bool SetQuestOutcome(FName OutcomeKey,FName OutcomeValue);
    UFUNCTION(BlueprintCallable) bool MeetCrew(FName CharacterId);
    UFUNCTION(BlueprintCallable) bool SetCrewAvailable(FName CharacterId,FName Role);
    UFUNCTION(BlueprintCallable) bool RecruitCrew(FName CharacterId,FName Role);
    UFUNCTION(BlueprintCallable) bool SetCrewAboard(FName CharacterId,bool bAboard=true);
    UFUNCTION(BlueprintPure) bool IsCrewMet(FName CharacterId) const;
    UFUNCTION(BlueprintPure) bool IsCrewAvailable(FName CharacterId) const;
    UFUNCTION(BlueprintPure) bool IsCrewRecruited(FName CharacterId) const;
    UFUNCTION(BlueprintPure) bool AreOpeningCrewRecruited() const;
    UFUNCTION(BlueprintCallable) bool MarkBossDefeated(FName BossId);
    UFUNCTION(BlueprintPure) bool HasBossDefeated(FName BossId) const;
    UFUNCTION(BlueprintCallable) bool SetCheckpoint(FName CheckpointId,FName SpawnId);
    bool SetOpeningProgress(const FOpeningProgressState& Progress);
    UFUNCTION(BlueprintPure) bool HasStoryFact(FName Fact) const;
    UFUNCTION(BlueprintPure) bool HasWorldFact(FName Fact) const;
    UFUNCTION(BlueprintPure) EMainMissionState GetMissionState(FName MissionId) const;
    UFUNCTION(BlueprintPure) const UDarkArisenSaveGame* GetState() const { return State; }
    bool Validate(TArray<FString>& OutErrors) const;
    bool ValidateState(const UDarkArisenSaveGame* Candidate,TArray<FString>& OutErrors) const;
    bool MigrateVersion(UDarkArisenSaveGame* Candidate,TArray<FString>& OutErrors) const;
    UPROPERTY(BlueprintAssignable) FMainMissionChanged OnMissionChanged;
private:
    UPROPERTY() TObjectPtr<UDarkArisenSaveGame> State;
    void ApplyMissionFacts(FName MissionId);
    void RefreshAvailability();
};
