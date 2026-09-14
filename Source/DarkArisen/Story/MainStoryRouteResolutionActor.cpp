#include "Story/MainStoryRouteResolutionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Story/MainStorySubsystem.h"
#include "UObject/ConstructorHelpers.h"
AMainStoryRouteResolutionActor::AMainStoryRouteResolutionActor()
{
    PrimaryActorTick.bCanEverTick=false; Visual=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));SetRootComponent(Visual);
    Visual->SetRelativeScale3D(FVector(.45f,.45f,.9f)); static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));if(Mesh.Succeeded())Visual->SetStaticMesh(Mesh.Object);
    InteractionLabel=NSLOCTEXT("DarkArisenStory","ResolveRoute","Commit to route");
}
bool AMainStoryRouteResolutionActor::CanInteract_Implementation(AActor* Interactor) const
{
    if(!IsValid(Interactor)||MissionId.IsNone()||OutcomeKey.IsNone()||OutcomeValue.IsNone()||!GetGameInstance())return false;
    const UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();return Story&&Story->GetMissionState(MissionId)==EMainMissionState::Active;
}
FText AMainStoryRouteResolutionActor::GetInteractionLabel_Implementation() const{return InteractionLabel;}
void AMainStoryRouteResolutionActor::CompleteInteraction_Implementation(AActor*)
{
    if(!GetGameInstance())return;UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    if(!Story||Story->GetMissionState(MissionId)!=EMainMissionState::Active)return;
    if(Story->SetQuestOutcome(OutcomeKey,OutcomeValue)) Story->CompleteAuthoredMission(MissionId);
}
