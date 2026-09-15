#include "Ship/NavalMissionGateActor.h"
#include "EngineUtils.h"
#include "Ship/HostileNavalShip.h"
#include "Ship/LaLiberacionShip.h"
#include "Ship/NavalCombatComponent.h"
#include "Story/MainStorySubsystem.h"
ANavalMissionGateActor::ANavalMissionGateActor(){PrimaryActorTick.bCanEverTick=true;SetActorTickInterval(.25f);}
void ANavalMissionGateActor::Tick(float){if(bResolved||MissionId.IsNone()||!GetGameInstance())return;int32 Total=0,Sunk=0;for(TActorIterator<AHostileNavalShip> It(GetWorld());It;++It){++Total;if(It->NavalCombat&&It->NavalCombat->IsSunk())++Sunk;}if(Total<RequiredSunkShips||Sunk<RequiredSunkShips)return;ALaLiberacionShip* PlayerShip=nullptr;for(TActorIterator<ALaLiberacionShip> It(GetWorld());It;++It){PlayerShip=*It;break;}if(!PlayerShip||FVector::DistSquared2D(PlayerShip->GetActorLocation(),GetActorLocation())>FMath::Square(CompletionRadius))return;UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();if(!Story||Story->GetMissionState(MissionId)!=EMainMissionState::Active)return;if(Story->CompleteAuthoredMission(MissionId)){bResolved=true;if(!CheckpointId.IsNone()&&!SpawnId.IsNone())Story->SetCheckpoint(CheckpointId,SpawnId);SetActorTickEnabled(false);}}
