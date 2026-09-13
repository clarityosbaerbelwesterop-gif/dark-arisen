#include "Opening/OpeningBoardingEncounterActor.h"
#include "Opening/OpeningBoarderCharacter.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Story/DarkArisenWorldDirector.h"
#include "Engine/World.h"
AOpeningBoardingEncounterActor::AOpeningBoardingEncounterActor(){PrimaryActorTick.bCanEverTick=false;BoarderClass=AOpeningBoarderCharacter::StaticClass();}
void AOpeningBoardingEncounterActor::BeginPlay(){Super::BeginPlay();}
bool AOpeningBoardingEncounterActor::StartEncounter()
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); auto* O=D?D->OpeningRuntime:nullptr; if(!O)return false;
    if(O->GetRaidState()==EOpeningRaidState::Boarding){if(!O->BeginBoardingEncounter(EncounterId,RequiredBoarders))return false;}
    else if(O->GetRaidState()!=EOpeningRaidState::FirstEncounter)return false;
    return SpawnRemainingBoarders();
}
bool AOpeningBoardingEncounterActor::SpawnRemainingBoarders()
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); auto* O=D?D->OpeningRuntime:nullptr; if(!O||O->GetBoardersRequired()!=RequiredBoarders)return false;
    const int32 Remaining=FMath::Max(0,RequiredBoarders-O->GetBoardersDefeated()); if(Remaining==0)return true;
    if(SpawnTransforms.Num()<Remaining||!BoarderClass)return false;
    for(int32 I=0;I<Remaining;++I){auto* B=GetWorld()->SpawnActorDeferred<AOpeningBoarderCharacter>(BoarderClass,SpawnTransforms[I],this,nullptr,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);if(!B)return false;B->ConfigureEncounter(EncounterId);B->FinishSpawning(SpawnTransforms[I]);}
    return true;
}
bool AOpeningBoardingEncounterActor::SignalDravenArrived()
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); auto* O=D?D->OpeningRuntime:nullptr; return O&&O->SignalDravenBoarded(EncounterId);
}
