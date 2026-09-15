#include "Ship/HostileNavalShip.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ship/NavalCombatComponent.h"
AHostileNavalShip::AHostileNavalShip()
{
    PrimaryActorTick.bCanEverTick=true;Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));SetRootComponent(Root);
    HullPresentation=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HullPresentation"));HullPresentation->SetupAttachment(Root);
    NavalCombat=CreateDefaultSubobject<UNavalCombatComponent>(TEXT("NavalCombat"));
    NavalCombat->MaxHull=760.f;NavalCombat->CannonsPerSide=6;NavalCombat->DamagePerCannon=18.f;NavalCombat->ReloadSeconds=9.f;
}
void AHostileNavalShip::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);if(!NavalCombat||NavalCombat->IsSunk()){SetActorTickEnabled(false);return;}
    FireDecisionRemaining-=DeltaSeconds;if(FireDecisionRemaining>0.f)return;FireDecisionRemaining=FMath::Max(.25f,FireDecisionSeconds);
    if(!NavalCombat->FireBroadside(ENavalBroadside::Port))NavalCombat->FireBroadside(ENavalBroadside::Starboard);
}
