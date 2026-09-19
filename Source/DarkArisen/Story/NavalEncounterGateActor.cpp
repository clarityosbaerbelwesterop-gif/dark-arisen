#include "Story/NavalEncounterGateActor.h"
#include "EngineUtils.h"
#include "Ship/NavalCombatComponent.h"
#include "Story/MainStorySubsystem.h"
ANavalEncounterGateActor::ANavalEncounterGateActor(){PrimaryActorTick.bCanEverTick=true;}
void ANavalEncounterGateActor::Tick(float DeltaSeconds){Super::Tick(DeltaSeconds);if(bCompleted||MissionId.IsNone())return;int32 Total=0,Defeated=0;for(TActorIterator<AActor> It(GetWorld());It;++It){AActor* A=*It;if(!A||!A->ActorHasTag(TEXT("NavalHostile")))continue;if(auto* C=A->FindComponentByClass<UNavalCombatComponent>()){++Total;if(C->IsSunk())++Defeated;}}if(Total<RequiredDefeats||Defeated<Total)return;UGameInstance* GI=GetGameInstance();auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;if(!Story)return;if(!RequiredWorldFact.IsNone()&&!Story->HasWorldFact(RequiredWorldFact))return;if(Story->CompleteAuthoredMission(MissionId))bCompleted=true;}
