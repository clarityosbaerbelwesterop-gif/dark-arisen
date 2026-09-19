#include "Story/MainStoryMapTransitionActor.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Story/MainStorySubsystem.h"
#include "TimerManager.h"

AMainStoryMapTransitionActor::AMainStoryMapTransitionActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMainStoryMapTransitionActor::BeginPlay()
{
    Super::BeginPlay();
    if (!GetGameInstance()) return;
    if (UMainStorySubsystem* Story = GetGameInstance()->GetSubsystem<UMainStorySubsystem>())
    {
        Story->OnMissionChanged.AddDynamic(this, &AMainStoryMapTransitionActor::HandleMissionChanged);
    }
}

void AMainStoryMapTransitionActor::HandleMissionChanged(FName ChangedMissionId, EMainMissionState State)
{
    if (bTravelQueued || MissionId.IsNone() || NextMap.IsNone()) return;
    if (ChangedMissionId != MissionId || State != EMainMissionState::Completed) return;

    bTravelQueued = true;
    if (TravelDelaySeconds <= KINDA_SMALL_NUMBER)
    {
        TravelToNextMap();
        return;
    }

    FTimerHandle TravelTimer;
    GetWorldTimerManager().SetTimer(TravelTimer, this, &AMainStoryMapTransitionActor::TravelToNextMap,
        TravelDelaySeconds, false);
}

void AMainStoryMapTransitionActor::TravelToNextMap()
{
    if (NextMap.IsNone()) return;
    UGameplayStatics::OpenLevel(this, NextMap);
}
