#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpeningRuntimeComponent.generated.h"

UENUM(BlueprintType)
enum class EOpeningLocation : uint8 { FamilyMerchantShip, OpenWater, OuterReef, DriftwoodBeach, DriftwoodCamp, MirasCove, MangroveShallows, KoasTradingPost, GalleonCove, LaLiberacion, OpenSea, RexaHarbor };
UENUM(BlueprintType)
enum class EOpeningRaidState : uint8 { Peaceful, FleetDetected, Alert, Boarding, FirstEncounter, Taking, Overboard, Complete };
UENUM(BlueprintType)
enum class EWaterRecoveryState : uint8 { Dry, Overboard, Swimming, OuterReef, BeachCrawl, Recovered };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOpeningLocationChanged,EOpeningLocation,Previous,EOpeningLocation,Current);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpeningPresentationEvent,FName,EventId);

/** Native Chapter 1/2 orchestration. Level actors only report authored contacts into this authority. */
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
    UFUNCTION(BlueprintCallable) bool SignalFirstBoarderDefeated();
    UFUNCTION(BlueprintCallable) bool SignalTakingCinematicComplete();
    UFUNCTION(BlueprintCallable) bool SignalEnteredWater();
    UFUNCTION(BlueprintCallable) bool SignalReachedOuterReef();
    UFUNCTION(BlueprintCallable) bool SignalReachedLocation(EOpeningLocation NewLocation);
    UFUNCTION(BlueprintCallable) bool SignalRecoveryComplete();
    UFUNCTION(BlueprintCallable) bool SignalGalleonCoveCleared();
    UFUNCTION(BlueprintCallable) bool ClaimLaLiberacion();
    UFUNCTION(BlueprintCallable) bool BeginFirstWake();
    UFUNCTION(BlueprintCallable) bool ReachRexaHarbor();
    UFUNCTION(BlueprintCallable) bool RecoverAtCheckpoint(AActor* Jake) const;
    UFUNCTION(BlueprintPure) bool CanUseHelm() const;
    UFUNCTION(BlueprintPure) EOpeningLocation GetLocation() const{return Location;}
    UPROPERTY(BlueprintAssignable) FOpeningLocationChanged OnLocationChanged;
    UPROPERTY(BlueprintAssignable) FOpeningPresentationEvent OnCinematicRequested;
    UPROPERTY(BlueprintAssignable) FOpeningPresentationEvent OnJournalObservation;
private:
    UPROPERTY(SaveGame) EOpeningLocation Location=EOpeningLocation::FamilyMerchantShip;
    UPROPERTY(SaveGame) EOpeningRaidState RaidState=EOpeningRaidState::Peaceful;
    UPROPERTY(SaveGame) EWaterRecoveryState RecoveryState=EWaterRecoveryState::Dry;
    UPROPERTY(SaveGame) TSet<FName> FamilyInteractions;
    bool AdvanceLocation(EOpeningLocation Expected,EOpeningLocation Next,FName Fact=NAME_None);
    class UMainStorySubsystem* Story() const;
};
