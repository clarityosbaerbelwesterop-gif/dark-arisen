#include "UI/AlphaMenuPlayerController.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Story/MainStorySubsystem.h"
namespace { const FName OpeningMap(TEXT("/Game/Alpha/Maps/L_HarlowOpening")); const FString SaveSlot(TEXT("DarkArisenAlpha")); }
const FString& AAlphaMenuPlayerController::GetAlphaSaveSlot(){return SaveSlot;}
void AAlphaMenuPlayerController::BeginPlay(){Super::BeginPlay();bShowMouseCursor=true;SetInputMode(FInputModeUIOnly());}
bool AAlphaMenuPlayerController::StartNewGame()
{
    UGameInstance* GI=GetGameInstance(); auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr; if(!Story)return false;
    Story->NewGame(); UGameplayStatics::OpenLevel(this,OpeningMap); return true;
}
bool AAlphaMenuPlayerController::ContinueGame()
{
    UGameInstance* GI=GetGameInstance(); auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr; if(!Story||!UGameplayStatics::DoesSaveGameExist(SaveSlot,0))return false;
    if(!Story->Load(SaveSlot,0))return false; UGameplayStatics::OpenLevel(this,OpeningMap); return true;
}
void AAlphaMenuPlayerController::SetVSyncEnabled(bool bEnabled){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetVSyncEnabled(bEnabled);S->ApplySettings(false);S->SaveSettings();}}
bool AAlphaMenuPlayerController::HasContinueSave()const{return UGameplayStatics::DoesSaveGameExist(SaveSlot,0);}
void AAlphaMenuPlayerController::QuitGame(){UKismetSystemLibrary::QuitGame(this,this,EQuitPreference::Quit,false);}
