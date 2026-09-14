#include "Story/MainStoryContactActor.h"
#include "Components/StaticMeshComponent.h"
#include "Story/MainStorySubsystem.h"
#include "UObject/ConstructorHelpers.h"
AMainStoryContactActor::AMainStoryContactActor()
{
    PrimaryActorTick.bCanEverTick=false;Visual=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));SetRootComponent(Visual);
    Visual->SetRelativeScale3D(FVector(.55f,.55f,1.75f));static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));if(Mesh.Succeeded())Visual->SetStaticMesh(Mesh.Object);
}
bool AMainStoryContactActor::CanInteract_Implementation(AActor* Interactor) const{return IsValid(Interactor)&&!ContactId.IsNone()&&!DialogueId.IsNone();}
EInteractionClass AMainStoryContactActor::GetInteractionClass_Implementation() const{return EInteractionClass::Examine;}
FText AMainStoryContactActor::GetInteractionLabel_Implementation() const{return DisplayName.IsEmpty()?NSLOCTEXT("DarkArisenStory","Speak","Speak"):DisplayName;}
FText AMainStoryContactActor::GetExamineTitle_Implementation() const{return DisplayName;}
FText AMainStoryContactActor::GetExamineBody_Implementation() const{return DialogueText;}
void AMainStoryContactActor::CompleteInteraction_Implementation(AActor*)
{
    if(!bActivateMissionOnInteraction||MissionId.IsNone()||!GetGameInstance())return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();if(Story&&Story->GetMissionState(MissionId)==EMainMissionState::Available)Story->ActivateMission(MissionId);
}
