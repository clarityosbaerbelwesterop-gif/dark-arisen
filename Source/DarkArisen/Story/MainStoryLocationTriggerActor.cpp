#include "Story/MainStoryLocationTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Components/HealthComponent.h"
#include "DuelingEnemyCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Story/MainStorySubsystem.h"

AMainStoryLocationTriggerActor::AMainStoryLocationTriggerActor()
{
    PrimaryActorTick.bCanEverTick=false;
    Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger")); SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(260.f,260.f,180.f)); Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

void AMainStoryLocationTriggerActor::BeginPlay()
{
    Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this,&AMainStoryLocationTriggerActor::HandleOverlap);
}

void AMainStoryLocationTriggerActor::HandleOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(!Cast<APawn>(Other)||MissionId.IsNone()||!GetGameInstance())return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>(); if(!Story)return;

    // Authored combat spaces must not be bypassable by sprinting through the completion volume.
    // Living dueling enemies in the current physical mission map gate completion; bosses keep
    // their own defeat-driven story authority through AMainStoryHolderBossCharacter.
    if(Action==EMainStoryLocationAction::CompleteMission)
    {
        if(UWorld* World=GetWorld())
        {
            for(TActorIterator<ADuelingEnemyCharacter> It(World);It;++It)
            {
                const ADuelingEnemyCharacter* Enemy=*It;
                if(IsValid(Enemy)&&IsValid(Enemy->HealthComponent)&&!Enemy->HealthComponent->IsDead())
                    return;
            }
        }
    }

    bool bChanged=false;
    if(Action==EMainStoryLocationAction::ActivateMission && Story->GetMissionState(MissionId)==EMainMissionState::Available) bChanged=Story->ActivateMission(MissionId);
    else if(Action==EMainStoryLocationAction::CompleteMission && Story->GetMissionState(MissionId)==EMainMissionState::Active) bChanged=Story->CompleteAuthoredMission(MissionId);
    if(bChanged&&!CheckpointId.IsNone()&&!SpawnId.IsNone()) Story->SetCheckpoint(CheckpointId,SpawnId);
}
