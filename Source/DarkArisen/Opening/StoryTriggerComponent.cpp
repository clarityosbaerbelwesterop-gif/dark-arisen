#include "Opening/StoryTriggerComponent.h"
#include "Story/MainStorySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
bool UStoryTriggerComponent::Execute(AActor* Actor)
{
 if(!IsValid(Actor)||(bOneShot&&bConsumed)||PayloadId.IsNone()||!GetWorld()||!GetWorld()->GetGameInstance())return false;auto*S=GetWorld()->GetGameInstance()->GetSubsystem<UMainStorySubsystem>();if(!S)return false;
 if(!RequiredMission.IsNone()&&S->GetMissionState(RequiredMission)!=EMainMissionState::Active)return false;bool Result=false;
 switch(Action){case EStoryTriggerAction::SetWorldFact:Result=S->SetWorldFact(PayloadId);break;case EStoryTriggerAction::CompleteMission:Result=S->CompleteMission(PayloadId);break;case EStoryTriggerAction::SetCheckpoint:Result=S->SetCheckpoint(PayloadId,SpawnId);break;case EStoryTriggerAction::RequestCinematic:OnPresentationRequested.Broadcast(PayloadId);Result=true;break;default:break;}if(Result&&bOneShot)bConsumed=true;return Result;
}
