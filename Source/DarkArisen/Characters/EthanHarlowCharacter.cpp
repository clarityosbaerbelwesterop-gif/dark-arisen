#include "Characters/EthanHarlowCharacter.h"
#include "Components/HealthComponent.h"
#include "Story/MainStorySubsystem.h"
AEthanHarlowCharacter::AEthanHarlowCharacter(){PrimaryActorTick.bCanEverTick=false;HealthComponent=CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));HealthComponent->MaxHealth=180.f;HealthComponent->bDamageImmune=true;}
void AEthanHarlowCharacter::BeginPlay(){Super::BeginPlay();if(HealthComponent)HealthComponent->ResetForRespawn(1.f);if(RuntimeState==EEthanRuntimeState::Captive)if(UGameInstance* GI=GetGameInstance())if(UMainStorySubsystem* Story=GI->GetSubsystem<UMainStorySubsystem>())if(Story->HasStoryFact(TEXT("Story.EthanRecovered"))||Story->HasStoryFact(TEXT("Story.EthanNoLongerCaptive")))RuntimeState=EEthanRuntimeState::Rescued;}
bool AEthanHarlowCharacter::MarkRescued(){if(RuntimeState==EEthanRuntimeState::Aboard)return true;RuntimeState=EEthanRuntimeState::Rescued;return true;}
bool AEthanHarlowCharacter::MarkAboard(){if(RuntimeState==EEthanRuntimeState::Captive)return false;RuntimeState=EEthanRuntimeState::Aboard;return true;}
