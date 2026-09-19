#include "Story/MainStoryContactActor.h"
#include "Components/StaticMeshComponent.h"
#include "Story/MainStorySubsystem.h"
#include "Engine/GameInstance.h"

AMainStoryContactActor::AMainStoryContactActor()
{
    PrimaryActorTick.bCanEverTick=false;
    Visual=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    SetRootComponent(Visual);
    Visual->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

bool AMainStoryContactActor::CanInteract_Implementation(AActor* Interactor) const
{
    const UGameInstance* GI=GetGameInstance();
    const UMainStorySubsystem* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
    if(!Story) return false;
    if(MissionId.IsNone()) return true;
    const EMainMissionState State=Story->GetMissionState(MissionId);
    return State==EMainMissionState::Active || State==EMainMissionState::Completed;
}

EInteractionClass AMainStoryContactActor::GetInteractionClass_Implementation() const { return EInteractionClass::Talk; }
FText AMainStoryContactActor::GetInteractionLabel_Implementation() const { return FText::FromString(TEXT("Talk")); }
FText AMainStoryContactActor::GetExamineTitle_Implementation() const { return DisplayName.IsEmpty()?FText::FromName(ContactId):DisplayName; }
FText AMainStoryContactActor::GetExamineBody_Implementation() const { return DialogueText; }

void AMainStoryContactActor::CompleteInteraction_Implementation(AActor* Interactor)
{
    UGameInstance* GI=GetGameInstance();
    UMainStorySubsystem* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
    if(!Story || MissionId.IsNone()) return;
    const EMainMissionState State=Story->GetMissionState(MissionId);
    if(bActivateMissionOnInteraction && State==EMainMissionState::Available)
        Story->ActivateMission(MissionId);
    if(bCompleteMissionOnInteraction && Story->GetMissionState(MissionId)==EMainMissionState::Active)
    {
        if(Story->CompleteAuthoredMission(MissionId) && !CheckpointId.IsNone() && !SpawnId.IsNone())
            Story->SetCheckpoint(CheckpointId,SpawnId);
    }
}
