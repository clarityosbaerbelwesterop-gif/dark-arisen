#include "Opening/OpeningCrewRecruitmentActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Story/DarkArisenWorldDirector.h"
#include "Story/MainStorySubsystem.h"
AOpeningCrewRecruitmentActor::AOpeningCrewRecruitmentActor()
{
    PrimaryActorTick.bCanEverTick=false; PresentationMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PresentationMesh")); RootComponent=PresentationMesh;
}
bool AOpeningCrewRecruitmentActor::CanInteract_Implementation(AActor* Interactor) const
{
    if(!Interactor||CrewId.IsNone())return false; const UWorld* W=GetWorld(); const UGameInstance* GI=W?W->GetGameInstance():nullptr; const UMainStorySubsystem* S=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
    return S&&!S->IsCrewRecruited(CrewId);
}
FText AOpeningCrewRecruitmentActor::GetInteractionLabel_Implementation() const
{
    const UWorld* W=GetWorld(); const UGameInstance* GI=W?W->GetGameInstance():nullptr; const UMainStorySubsystem* S=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
    const FText Name=DisplayName.IsEmpty()?FText::FromName(CrewId):DisplayName;
    if(S&&S->IsCrewAvailable(CrewId))return FText::Format(NSLOCTEXT("DarkArisenOpening","RecruitCrew","Ask {0} to join"),Name);
    if(S&&S->IsCrewMet(CrewId))return FText::Format(NSLOCTEXT("DarkArisenOpening","ContinueCrew","Speak with {0}"),Name);
    return FText::Format(NSLOCTEXT("DarkArisenOpening","MeetCrew","Speak with {0}"),Name);
}
void AOpeningCrewRecruitmentActor::CompleteInteraction_Implementation(AActor*)
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); UOpeningRuntimeComponent* O=D?D->OpeningRuntime:nullptr; const UWorld* W=GetWorld(); const UGameInstance* GI=W?W->GetGameInstance():nullptr; const UMainStorySubsystem* S=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
    if(!O||!S)return;
    if(!S->IsCrewMet(CrewId)){O->SignalCrewMet(CrewId);return;}
    if(!S->IsCrewAvailable(CrewId)){O->SignalCrewRecruitmentAvailable(CrewId);return;}
    if(!S->IsCrewRecruited(CrewId))O->SignalCrewRecruited(CrewId);
}
