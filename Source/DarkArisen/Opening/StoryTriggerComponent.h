#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StoryTriggerComponent.generated.h"
UENUM(BlueprintType) enum class EStoryTriggerAction:uint8{SetWorldFact,CompleteMission,SetCheckpoint,RequestCinematic};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStoryTriggerPresentation,FName,EventId);
/** Idempotent native trigger for overlap/interaction adapters. Fails closed unless its required mission is active. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent)) class DARKARISEN_API UStoryTriggerComponent:public UActorComponent
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) bool Execute(AActor* InstigatorActor);
 UPROPERTY(EditAnywhere) EStoryTriggerAction Action=EStoryTriggerAction::SetWorldFact;
 UPROPERTY(EditAnywhere) FName RequiredMission;
 UPROPERTY(EditAnywhere) FName PayloadId;
 UPROPERTY(EditAnywhere,meta=(EditCondition="Action==EStoryTriggerAction::SetCheckpoint")) FName SpawnId;
 UPROPERTY(EditAnywhere) bool bOneShot=true;
 UPROPERTY(BlueprintAssignable) FStoryTriggerPresentation OnPresentationRequested;
private: UPROPERTY(SaveGame) bool bConsumed=false;
};
