#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "OpeningEventTriggerComponent.generated.h"
UENUM(BlueprintType)
enum class EOpeningEventAction:uint8
{
 FamilyInteraction,FleetDetected,BoardingStarted,BeginBoardingEncounter,BoarderDefeated,DravenBoarded,
 TakingStarted,TakingPresentationComplete,EnteredWater,ReachedOuterReef,ReachedLocation,RecoveryComplete,
 CrewMet,CrewAvailable,CrewRecruited,GalleonCoveCleared,LaLiberacionBoarded,LaLiberacionHelmSecured,
 LaLiberacionHarborCleared,BeginFirstWake,ReachRexaHarbor,RecoverCheckpoint
};
/** Thin authored-world adapter into UOpeningRuntimeComponent. Owns no durable progression state. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UOpeningEventTriggerComponent:public UActorComponent
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,Category="Dark Arisen|Opening") bool Execute(AActor* InstigatorActor);
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dark Arisen|Opening") EOpeningEventAction Action=EOpeningEventAction::FamilyInteraction;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dark Arisen|Opening") FName EventId;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dark Arisen|Opening",meta=(ClampMin="2")) int32 RequiredBoarders=2;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dark Arisen|Opening") EOpeningLocation TargetLocation=EOpeningLocation::DriftwoodBeach;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dark Arisen|Opening") bool bOneShot=true;
private:
 bool bConsumed=false;
};
