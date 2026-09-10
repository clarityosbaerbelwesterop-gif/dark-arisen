#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Story/MainStoryTypes.h"
#include "MainStoryDirectorComponent.generated.h"
/** World-facing bridge to the GameInstance story authority; owns no duplicate progression state. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent)) class DARKARISEN_API UMainStoryDirectorComponent:public UActorComponent
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) bool ActivateCurrentMission();
 UFUNCTION(BlueprintCallable) bool CompleteCurrentMission();
 UFUNCTION(BlueprintCallable) bool CompleteMission(FName MissionId);
 UFUNCTION(BlueprintPure) FName GetCurrentMission()const;
 UFUNCTION(BlueprintPure) int32 GetCurrentChapter()const;
 UFUNCTION(BlueprintPure) EMainMissionState GetMissionState(FName MissionId)const;
private: class UMainStorySubsystem* Authority()const;
};
