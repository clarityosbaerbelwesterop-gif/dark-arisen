#include "UI/AlphaMenuPlayerController.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Story/MainStoryMapCatalog.h"
#include "Story/MainStorySubsystem.h"
namespace{const FName OpeningMap(TEXT("L_HarlowOpening"));const FString SaveSlot(TEXT("DarkArisenAlpha"));}
const FString& AAlphaMenuPlayerController::GetAlphaSaveSlot(){return SaveSlot;}
void AAlphaMenuPlayerController::BeginPlay(){Super::BeginPlay();bShowMouseCursor=true;bEnableClickEvents=true;SetInputMode(FInputModeGameAndUI());}
bool AAlphaMenuPlayerController::StartNewGame(){UGameInstance* GI=GetGameInstance();auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;if(!Story)return false;Story->NewGame();UGameplayStatics::OpenLevel(this,OpeningMap);return true;}
bool AAlphaMenuPlayerController::ContinueGame(){UGameInstance* GI=GetGameInstance();auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;if(!Story||!UGameplayStatics::DoesSaveGameExist(SaveSlot,0)||!Story->Load(SaveSlot,0))return false;const auto* State=Story->GetState();const FName Map=State?FMainStoryMapCatalog::ResolveMap(State->CurrentMission):NAME_None;if(Map.IsNone())return false;UGameplayStatics::OpenLevel(this,Map);return true;}
void AAlphaMenuPlayerController::OpenSettings(){MenuPage=EAlphaMenuPage::Settings;}
void AAlphaMenuPlayerController::BackToMain(){MenuPage=EAlphaMenuPage::Main;}
void AAlphaMenuPlayerController::SetVSyncEnabled(bool b){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetVSyncEnabled(b);S->ApplySettings(false);S->SaveSettings();}}
void AAlphaMenuPlayerController::ToggleVSync(){if(auto* S=UGameUserSettings::GetGameUserSettings())SetVSyncEnabled(!S->IsVSyncEnabled());}
void AAlphaMenuPlayerController::SetQualityPreset(int32 L){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetOverallScalabilityLevel(FMath::Clamp(L,0,3));S->ApplySettings(false);S->SaveSettings();}}
void AAlphaMenuPlayerController::ToggleFullscreen(){if(auto* S=UGameUserSettings::GetGameUserSettings()){const EWindowMode::Type Next=S->GetFullscreenMode()==EWindowMode::WindowedFullscreen?EWindowMode::Windowed:EWindowMode::WindowedFullscreen;S->SetFullscreenMode(Next);S->ApplySettings(false);S->SaveSettings();}}
bool AAlphaMenuPlayerController::HasContinueSave()const{return UGameplayStatics::DoesSaveGameExist(SaveSlot,0);}
void AAlphaMenuPlayerController::QuitGame(){UKismetSystemLibrary::QuitGame(this,this,EQuitPreference::Quit,false);}

void AAlphaMenuPlayerController::SetResolutionPreset(int32 Width,int32 Height){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetScreenResolution(FIntPoint(FMath::Max(1280,Width),FMath::Max(720,Height)));S->SetFullscreenMode(EWindowMode::WindowedFullscreen);S->ApplySettings(false);S->SaveSettings();}}
