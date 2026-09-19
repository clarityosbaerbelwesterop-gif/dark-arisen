#include "Opening/OpeningFamilyInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Story/DarkArisenWorldDirector.h"
AOpeningFamilyInteractableActor::AOpeningFamilyInteractableActor()
{
    PrimaryActorTick.bCanEverTick=false;
    PresentationMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PresentationMesh"));
    RootComponent=PresentationMesh;
}
bool AOpeningFamilyInteractableActor::CanInteract_Implementation(AActor* Interactor) const
{
    if(!Interactor||CharacterId.IsNone())return false;
    const ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this);
    return D&&D->OpeningRuntime&&D->OpeningRuntime->GetLocation()==EOpeningLocation::FamilyMerchantShip&&D->OpeningRuntime->GetRaidState()==EOpeningRaidState::Peaceful;
}
FText AOpeningFamilyInteractableActor::GetInteractionLabel_Implementation() const
{
    return CharacterName.IsEmpty()?NSLOCTEXT("DarkArisenOpening","SpeakFamily","Speak"):FText::Format(NSLOCTEXT("DarkArisenOpening","SpeakNamed","Speak with {0}"),CharacterName);
}
void AOpeningFamilyInteractableActor::CompleteInteraction_Implementation(AActor*)
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this);
    if(D&&D->OpeningRuntime)D->OpeningRuntime->SignalFamilyInteraction(CharacterId);
}
