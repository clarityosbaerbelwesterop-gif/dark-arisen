#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Persistence/DarkArisenSaveGame.h"
#include "MainStoryMapTransitionActor.generated.h"

/**
 * Presentation-only bridge from canonical mission completion to the next authored physical map.
 * Story state remains owned by UMainStorySubsystem; this actor only performs level travel after
 * the subsystem reports that its configured mission completed.
 */
UCLASS()
class DARKARISEN_API AMainStoryMapTransitionActor : public AActor
{
    GENERATED_BODY()

public:
    AMainStoryMapTransitionActor();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story|Travel")
    FName MissionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story|Travel")
    FName NextMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story|Travel", meta=(ClampMin="0.0"))
    float TravelDelaySeconds = 1.25f;

private:
    UFUNCTION()
    void HandleMissionChanged(FName ChangedMissionId, EMainMissionState State);

    void TravelToNextMap();
    bool bTravelQueued = false;
};
