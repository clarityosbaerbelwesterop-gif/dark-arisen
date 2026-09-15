#include "Story/MainStoryEvidenceActor.h"
#include "Story/MainStorySubsystem.h"

bool AMainStoryEvidenceActor::CanInteract_Implementation(AActor* Interactor) const
{
    if(!Super::CanInteract_Implementation(Interactor)||MissionId.IsNone()||!GetGameInstance())return false;
    const UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    return Story&&Story->GetMissionState(MissionId)==EMainMissionState::Active
        &&(RequiredBossId.IsNone()||Story->HasBossDefeated(RequiredBossId));
}

void AMainStoryEvidenceActor::CompleteInteraction_Implementation(AActor* Interactor)
{
    if(!CanInteract_Implementation(Interactor))return;
    Super::CompleteInteraction_Implementation(Interactor);
    if(!GetGameInstance())return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    if(Story&&Story->CompleteAuthoredMission(MissionId)&&!CheckpointId.IsNone()&&!SpawnId.IsNone())
        Story->SetCheckpoint(CheckpointId,SpawnId);
}
