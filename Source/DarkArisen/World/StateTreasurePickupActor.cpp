#include "World/StateTreasurePickupActor.h"
#include "Engine/GameInstance.h"
#include "Story/MainStorySubsystem.h"

AStateTreasurePickupActor::AStateTreasurePickupActor()
{
    ItemName=NSLOCTEXT("DarkArisenTreasure","StateTreasure","State record");
}

bool AStateTreasurePickupActor::CanInteract_Implementation(AActor* Interactor) const
{
    if(!Super::CanInteract_Implementation(Interactor) || TreasureId.IsNone() || !GetGameInstance()) return false;
    const UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    return Story && !Story->HasRecoveredTreasure(TreasureId);
}

void AStateTreasurePickupActor::CompleteInteraction_Implementation(AActor* Interactor)
{
    if(!GetGameInstance()) return;
    UMainStorySubsystem* Story=GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    if(!Story || TreasureId.IsNone() || Story->HasRecoveredTreasure(TreasureId)) return;
    Super::CompleteInteraction_Implementation(Interactor);
    Story->RecoverTreasure(TreasureId);
}
