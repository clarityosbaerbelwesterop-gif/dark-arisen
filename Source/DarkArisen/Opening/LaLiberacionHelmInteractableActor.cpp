#include "Opening/LaLiberacionHelmInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Ship/LaLiberacionShip.h"
#include "Ship/ShipVoyageComponent.h"
#include "Story/DarkArisenWorldDirector.h"
#include "Story/MainStorySubsystem.h"
#include "Engine/GameInstance.h"
ALaLiberacionHelmInteractableActor::ALaLiberacionHelmInteractableActor(){PrimaryActorTick.bCanEverTick=false;HelmMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmMesh"));RootComponent=HelmMesh;}
bool ALaLiberacionHelmInteractableActor::CanInteract_Implementation(AActor* Interactor) const{return Interactor&&Ship!=nullptr;}
void ALaLiberacionHelmInteractableActor::CompleteInteraction_Implementation(AActor*)
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); UOpeningRuntimeComponent* O=D?D->OpeningRuntime:nullptr; if(!O||!Ship||!Ship->VoyageComponent)return;
    if(O->GetLocation()==EOpeningLocation::GalleonCove){O->SignalLaLiberacionHelmSecured();return;}
    if(O->GetLocation()==EOpeningLocation::LaLiberacion&&O->CanUseHelm())
    {
        const UGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance():nullptr; const UMainStorySubsystem* S=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;
        if(S&&S->HasWorldFact(TEXT("Ship.LaLiberacionOwned")))
        {
            Ship->VoyageComponent->SetOwnedAndUnlocked(true);
            if(O->BeginFirstWake())Ship->VoyageComponent->SetJakeAtHelm(true);
        }
    }
}
