#include "World/DungeonStateAnchorActor.h"
#include "Components/BoxComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "Story/MainStorySubsystem.h"

ADungeonStateAnchorActor::ADungeonStateAnchorActor()
{
    PrimaryActorTick.bCanEverTick=false;
    Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(180.f,180.f,220.f));
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
    Trigger->OnComponentBeginOverlap.AddDynamic(this,&ADungeonStateAnchorActor::HandleBeginOverlap);
}

void ADungeonStateAnchorActor::HandleBeginOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
    if(DungeonId.IsNone() || !Cast<APawn>(Other) || !GetGameInstance()) return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    if(!Story) return;
    switch(Action)
    {
        case EDungeonStateAnchorAction::Discover: Story->DiscoverDungeon(DungeonId); break;
        case EDungeonStateAnchorAction::Complete: Story->CompleteDungeon(DungeonId); break;
        case EDungeonStateAnchorAction::OpenReturnShortcut:
            if(!ShortcutId.IsNone()) Story->SetWorldFact(ShortcutId,true);
            break;
        default: break;
    }
}
