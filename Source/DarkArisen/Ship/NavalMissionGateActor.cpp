#include "Ship/NavalMissionGateActor.h"
#include "Components/BoxComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Ship/HostileNavalShip.h"
#include "Ship/NavalCombatComponent.h"
#include "Story/MainStorySubsystem.h"
ANavalMissionGateActor::ANavalMissionGateActor(){PrimaryActorTick.bCanEverTick=false;Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));SetRootComponent(Trigger);Trigger->SetBoxExtent(FVector(500.f,500.f,300.f));Trigger->SetCollisionProfileName(TEXT("Trigger"));}
void ANavalMissionGateActor::BeginPlay(){Super::BeginPlay();Trigger->OnComponentBeginOverlap.AddDynamic(this,&ANavalMissionGateActor::HandleOverlap);}
void ANavalMissionGateActor::HandleOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(!Cast<APawn>(Other)||MissionId.IsNone()||!GetGameInstance())return;
    int32 Total=0,Sunk=0;for(TActorIterator<AHostileNavalShip> It(GetWorld());It;++It){++Total;if(It->NavalCombat&&It->NavalCombat->IsSunk())++Sunk;}
    if(Total<RequiredSunkShips||Sunk<RequiredSunkShips)return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();if(!Story||Story->GetMissionState(MissionId)!=EMainMissionState::Active)return;
    if(Story->CompleteAuthoredMission(MissionId)&&!CheckpointId.IsNone()&&!SpawnId.IsNone())Story->SetCheckpoint(CheckpointId,SpawnId);
}
