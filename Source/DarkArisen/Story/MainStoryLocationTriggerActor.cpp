#include "Story/MainStoryLocationTriggerActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Story/MainStorySubsystem.h"
AMainStoryLocationTriggerActor::AMainStoryLocationTriggerActor()
{
    PrimaryActorTick.bCanEverTick=false;
    Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger")); SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(260.f,260.f,180.f)); Trigger->SetCollisionProfileName(TEXT("Trigger"));
}
void AMainStoryLocationTriggerActor::BeginPlay(){Super::BeginPlay();Trigger->OnComponentBeginOverlap.AddDynamic(this,&AMainStoryLocationTriggerActor::HandleOverlap);}
void AMainStoryLocationTriggerActor::HandleOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(!Cast<APawn>(Other)||MissionId.IsNone()||!GetGameInstance())return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>(); if(!Story)return;
    bool bChanged=false;
    if(Action==EMainStoryLocationAction::ActivateMission && Story->GetMissionState(MissionId)==EMainMissionState::Available) bChanged=Story->ActivateMission(MissionId);
    else if(Action==EMainStoryLocationAction::CompleteMission && Story->GetMissionState(MissionId)==EMainMissionState::Active) bChanged=Story->CompleteMission(MissionId);
    if(bChanged&&!CheckpointId.IsNone()&&!SpawnId.IsNone()) Story->SetCheckpoint(CheckpointId,SpawnId);
}
