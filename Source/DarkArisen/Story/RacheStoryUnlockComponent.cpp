#include "Story/RacheStoryUnlockComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/Actor.h"
#include "Story/MainStorySubsystem.h"
URacheStoryUnlockComponent::URacheStoryUnlockComponent(){PrimaryComponentTick.bCanEverTick=false;}
void URacheStoryUnlockComponent::BeginPlay(){Super::BeginPlay();if(UGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance():nullptr)if(UMainStorySubsystem* Story=GI->GetSubsystem<UMainStorySubsystem>()){Story->OnMissionChanged.AddDynamic(this,&URacheStoryUnlockComponent::HandleMissionChanged);Story->OnWorldFactChanged.AddDynamic(this,&URacheStoryUnlockComponent::HandleWorldFactChanged);}RefreshUnlock();}
void URacheStoryUnlockComponent::HandleMissionChanged(FName,EMainMissionState){RefreshUnlock();}
void URacheStoryUnlockComponent::HandleWorldFactChanged(FName Fact,bool){if(Fact==TEXT("Combat.RacheUnlocked"))RefreshUnlock();}
void URacheStoryUnlockComponent::RefreshUnlock(){AActor* Owner=GetOwner();if(!Owner)return;UCombatComponent* Combat=Owner->FindComponentByClass<UCombatComponent>();UGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance():nullptr;UMainStorySubsystem* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;if(Combat&&Story)Combat->bRacheUnlocked=Story->HasWorldFact(TEXT("Combat.RacheUnlocked"));}
