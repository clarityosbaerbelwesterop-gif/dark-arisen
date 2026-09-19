#include "Opening/OpeningWaterCurrentVolume.h"
#include "Components/BoxComponent.h"
#include "Components/WaterBreathComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "JakeCharacter.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Story/DarkArisenWorldDirector.h"
AOpeningWaterCurrentVolume::AOpeningWaterCurrentVolume()
{
    PrimaryActorTick.bCanEverTick=true;
    Volume=CreateDefaultSubobject<UBoxComponent>(TEXT("WaterVolume")); RootComponent=Volume;
    Volume->SetCollisionProfileName(TEXT("Trigger"));
    Volume->OnComponentBeginOverlap.AddDynamic(this,&AOpeningWaterCurrentVolume::HandleBegin);
    Volume->OnComponentEndOverlap.AddDynamic(this,&AOpeningWaterCurrentVolume::HandleEnd);
}
void AOpeningWaterCurrentVolume::HandleBegin(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    AJakeCharacter* Jake=Cast<AJakeCharacter>(Other); if(!Jake)return;
    if(UCharacterMovementComponent* M=Jake->GetCharacterMovement())M->SetMovementMode(MOVE_Swimming);
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); UOpeningRuntimeComponent* O=D?D->OpeningRuntime:nullptr;
    if(O&&bSignalsWaterEntry)O->SignalEnteredWater();
    if(O&&bSignalsOuterReef)O->SignalReachedOuterReef();
}
void AOpeningWaterCurrentVolume::HandleEnd(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32)
{
    AJakeCharacter* Jake=Cast<AJakeCharacter>(Other); if(!Jake)return;
    if(Jake->WaterBreathComponent)Jake->WaterBreathComponent->SetHeadSubmerged(false);
    if(bShallowExit)if(UCharacterMovementComponent* M=Jake->GetCharacterMovement())M->SetMovementMode(MOVE_Walking);
}
void AOpeningWaterCurrentVolume::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds); if(DeltaSeconds<=0.f)return;
    TArray<AActor*> Actors; Volume->GetOverlappingActors(Actors,AJakeCharacter::StaticClass());
    for(AActor* A:Actors)
    {
        AJakeCharacter* Jake=Cast<AJakeCharacter>(A); if(!Jake)continue;
        if(UCharacterMovementComponent* M=Jake->GetCharacterMovement())M->AddForce(CurrentAcceleration*M->Mass);
        if(Jake->WaterBreathComponent)
        {
            const float SurfaceZ=Volume->Bounds.Origin.Z+Volume->Bounds.BoxExtent.Z-SurfaceOffsetCentimetres;
            Jake->WaterBreathComponent->SetHeadSubmerged(Jake->GetPawnViewLocation().Z<SurfaceZ);
        }
    }
}
