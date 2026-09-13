#include "Opening/OpeningBoarderCharacter.h"
#include "AI/BoardingEnemyComponent.h"
#include "Components/HealthComponent.h"
#include "Story/DarkArisenWorldDirector.h"
#include "Opening/OpeningRuntimeComponent.h"
AOpeningBoarderCharacter::AOpeningBoarderCharacter(){BoardingEnemy=CreateDefaultSubobject<UBoardingEnemyComponent>(TEXT("BoardingEnemy"));}
void AOpeningBoarderCharacter::BeginPlay(){Super::BeginPlay();if(HealthComponent)HealthComponent->OnDied.AddDynamic(this,&AOpeningBoarderCharacter::ReportOpeningDeath);}
void AOpeningBoarderCharacter::ConfigureEncounter(FName InEncounterId){EncounterId=InEncounterId;}
void AOpeningBoarderCharacter::ReportOpeningDeath(AActor*)
{
    ADarkArisenWorldDirector* D=ADarkArisenWorldDirector::Resolve(this); if(D&&D->OpeningRuntime&&!EncounterId.IsNone())D->OpeningRuntime->SignalBoarderDefeated(EncounterId);
}
