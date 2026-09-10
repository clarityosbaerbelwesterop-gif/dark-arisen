#include "Story/MainStoryDirectorComponent.h"
#include "Story/MainStorySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
UMainStorySubsystem* UMainStoryDirectorComponent::Authority()const{return GetWorld()&&GetWorld()->GetGameInstance()?GetWorld()->GetGameInstance()->GetSubsystem<UMainStorySubsystem>():nullptr;}
bool UMainStoryDirectorComponent::ActivateCurrentMission(){auto*S=Authority();return S&&S->GetState()&&S->ActivateMission(S->GetState()->CurrentMission);}
bool UMainStoryDirectorComponent::CompleteCurrentMission(){auto*S=Authority();return S&&S->GetState()&&S->CompleteMission(S->GetState()->CurrentMission);}
bool UMainStoryDirectorComponent::CompleteMission(FName Id){auto*S=Authority();return S&&S->CompleteMission(Id);}
FName UMainStoryDirectorComponent::GetCurrentMission()const{const auto*S=Authority();return S&&S->GetState()?S->GetState()->CurrentMission:NAME_None;}
int32 UMainStoryDirectorComponent::GetCurrentChapter()const{const auto*S=Authority();return S&&S->GetState()?S->GetState()->CurrentChapter:0;}
EMainMissionState UMainStoryDirectorComponent::GetMissionState(FName Id)const{const auto*S=Authority();return S?S->GetMissionState(Id):EMainMissionState::Locked;}
