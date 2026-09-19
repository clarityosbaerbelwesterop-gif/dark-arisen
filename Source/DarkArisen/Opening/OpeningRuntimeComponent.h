#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpeningRuntimeComponent.generated.h"

UENUM(BlueprintType)
enum class EOpeningLocation : uint8
{
    FamilyMerchantShip, OpenWater, OuterReef, DriftwoodBeach, DriftwoodCamp, MirasCove,
    MangroveShallows, KoasTradingPost, GalleonCove, LaLiberacion, OpenSea, RexaHarbor
};

/** Existing ordinal values remain stable for save compatibility; DravenAboard is appended. */
UENUM(BlueprintType)
enum class EOpeningRaidState : uint8
{
    Peaceful, FleetDetected, Alert, Boarding, FirstEncounter, Taking, Overboard, Complete,
    DravenAboard
};

UENUM(BlueprintType)
enum class EWaterRecoveryState : uint8
{
    Dry, Overboard, Swimming, OuterReef, BeachCrawl, Recovered
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOpeningLocationChanged, EOpeningLocation, Previous, EOpeningLocation, Current);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpeningPresentationEvent, FName, EventId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOpeningCheckpointRecoveryEvent, FName, CheckpointId, FName, SpawnId);

/** Native Chapter 1/2 orchestration. Level actors report authored gameplay contacts into this authority. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UOpeningRuntimeComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UOpeningRuntimeComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable) bool SignalFamilyInteraction(FName FamilyMemberId);
    UFUNCTION(BlueprintCallable) bool SignalFleetDetected();
    UFUNCTION(BlueprintCallable) bool SignalBoardingStarted();
    UFUNCTION(BlueprintCallable) bool BeginBoardingEncounter(FName EncounterId,int32 RequiredBoarders);
    UFUNCTION(BlueprintCallable) bool SignalBoarderDefeated(FName EncounterId);
    /** Compatibility entry point. It records one defeat only; it never advances directly to The Taking. */
    UFUNCTION(BlueprintCallable) bool SignalFirstBoarderDefeated();
    UFUNCTION(BlueprintCallable) bool SignalDravenBoarded(FName EncounterId);
    UFUNCTION(BlueprintCallable) bool SignalTakingStarted();
    UFUNCTION(BlueprintCallable) bool SignalTakingCinematicComplete();

    UFUNCTION(BlueprintCallable) bool SignalEnteredWater();
    UFUNCTION(BlueprintCallable) bool SignalReachedOuterReef();
    UFUNCTION(BlueprintCallable) bool SignalReachedLocation(EOpeningLocation NewLocation);
    UFUNCTION(BlueprintCallable) bool SignalRecoveryComplete();

    UFUNCTION(BlueprintCallable) bool SignalCrewMet(FName CrewId);
    UFUNCTION(BlueprintCallable) bool SignalCrewRecruitmentAvailable(FName CrewId);
    UFUNCTION(BlueprintCallable) bool SignalCrewRecruited(FName CrewId);

    UFUNCTION(BlueprintCallable) bool SignalGalleonCoveCleared();
    /** Compatibility alias for the first real ship-acquisition step: physically boarding the prize. */
    UFUNCTION(BlueprintCallable) bool ClaimLaLiberacion();
    UFUNCTION(BlueprintCallable) bool SignalLaLiberacionBoarded();
    UFUNCTION(BlueprintCallable) bool SignalLaLiberacionHelmSecured();
    UFUNCTION(BlueprintCallable) bool SignalLaLiberacionHarborCleared();
    UFUNCTION(BlueprintCallable) bool BeginFirstWake();
    UFUNCTION(BlueprintCallable) bool ReachRexaHarbor();

    UFUNCTION(BlueprintCallable) bool RecoverAtCheckpoint(AActor* Jake);
    UFUNCTION(BlueprintPure) bool CanUseHelm() const;
    UFUNCTION(BlueprintPure) EOpeningLocation GetLocation() const{return Location;}
    UFUNCTION(BlueprintPure) EOpeningRaidState GetRaidState() const{return RaidState;}
    UFUNCTION(BlueprintPure) int32 GetBoardersRequired() const{return BoardersRequired;}
    UFUNCTION(BlueprintPure) int32 GetBoardersDefeated() const{return BoardersDefeated;}

    UPROPERTY(BlueprintAssignable) FOpeningLocationChanged OnLocationChanged;
    UPROPERTY(BlueprintAssignable) FOpeningPresentationEvent OnCinematicRequested;
    UPROPERTY(BlueprintAssignable) FOpeningPresentationEvent OnJournalObservation;
    UPROPERTY(BlueprintAssignable) FOpeningCheckpointRecoveryEvent OnCheckpointRecovered;

private:
    UPROPERTY(SaveGame) EOpeningLocation Location=EOpeningLocation::FamilyMerchantShip;
    UPROPERTY(SaveGame) EOpeningRaidState RaidState=EOpeningRaidState::Peaceful;
    UPROPERTY(SaveGame) EWaterRecoveryState RecoveryState=EWaterRecoveryState::Dry;
    UPROPERTY(SaveGame) TSet<FName> FamilyInteractions;
    UPROPERTY(SaveGame) FName BoardingEncounterId;
    UPROPERTY(SaveGame) int32 BoardersRequired=0;
    UPROPERTY(SaveGame) int32 BoardersDefeated=0;
    UPROPERTY(SaveGame) bool bDravenBoarded=false;
    UPROPERTY(SaveGame) bool bLaLiberacionBoarded=false;
    UPROPERTY(SaveGame) bool bLaLiberacionHelmSecured=false;
    UPROPERTY(SaveGame) bool bLaLiberacionClearedHarbor=false;

    bool AdvanceLocation(EOpeningLocation Expected,EOpeningLocation Next,FName Fact=NAME_None);
    void RestoreProgress();
    bool CommitProgress();
    class UMainStorySubsystem* Story() const;
};
