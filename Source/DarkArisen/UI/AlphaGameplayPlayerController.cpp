#include "UI/AlphaGameplayPlayerController.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Story/MainStoryMapCatalog.h"
#include "Story/MainStorySubsystem.h"
#include "UI/AlphaMenuPlayerController.h"
void AAlphaGameplayPlayerController::BeginPlay(){Super::BeginPlay();ApplyOverlayInput();}
void AAlphaGameplayPlayerController::SetupInputComponent(){Super::SetupInputComponent();check(InputComponent);FInputActionBinding& PauseBinding=InputComponent->BindAction(TEXT("PauseMenu"),IE_Pressed,this,&AAlphaGameplayPlayerController::TogglePauseMenu);PauseBinding.bExecuteWhenPaused=true;InputComponent->BindAction(TEXT("WorldMap"),IE_Pressed,this,&AAlphaGameplayPlayerController::ToggleWorldMap);InputComponent->BindAction(TEXT("MiniMap"),IE_Pressed,this,&AAlphaGameplayPlayerController::ToggleMiniMap);InputComponent->BindAction(TEXT("QuickSave"),IE_Pressed,this,&AAlphaGameplayPlayerController::QuickSave);InputComponent->BindAction(TEXT("QuickLoad"),IE_Pressed,this,&AAlphaGameplayPlayerController::QuickLoad);}
void AAlphaGameplayPlayerController::ApplyOverlayInput(){const bool bOpen=Overlay!=EAlphaOverlay::None;bShowMouseCursor=bOpen;bEnableClickEvents=bOpen;SetPause(Overlay==EAlphaOverlay::Pause||Overlay==EAlphaOverlay::Settings);if(bOpen)SetInputMode(FInputModeGameAndUI());else SetInputMode(FInputModeGameOnly());}
void AAlphaGameplayPlayerController::TogglePauseMenu(){Overlay=Overlay==EAlphaOverlay::Pause?EAlphaOverlay::None:EAlphaOverlay::Pause;ApplyOverlayInput();}
void AAlphaGameplayPlayerController::ToggleWorldMap(){Overlay=Overlay==EAlphaOverlay::Map?EAlphaOverlay::None:EAlphaOverlay::Map;ApplyOverlayInput();}
void AAlphaGameplayPlayerController::ToggleMiniMap(){bMiniMapEnabled=!bMiniMapEnabled;LastSystemMessage=bMiniMapEnabled?TEXT("Minimap enabled"):TEXT("Minimap disabled");}
void AAlphaGameplayPlayerController::QuickSave(){UGameInstance* GI=GetGameInstance();auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;LastSystemMessage=Story&&Story->Save(AAlphaMenuPlayerController::GetAlphaSaveSlot(),0)?TEXT("Game saved"):TEXT("Save failed");}
void AAlphaGameplayPlayerController::QuickLoad(){UGameInstance* GI=GetGameInstance();auto* Story=GI?GI->GetSubsystem<UMainStorySubsystem>():nullptr;if(!Story||!Story->Load(AAlphaMenuPlayerController::GetAlphaSaveSlot(),0)){LastSystemMessage=TEXT("Load failed");return;}const auto* State=Story->GetState();const FName Map=State?FMainStoryMapCatalog::ResolveMap(State->CurrentMission):NAME_None;if(Map.IsNone()){LastSystemMessage=TEXT("Save has no resolvable mission map");return;}UGameplayStatics::OpenLevel(this,Map);}
void AAlphaGameplayPlayerController::OpenSettings(){Overlay=EAlphaOverlay::Settings;ApplyOverlayInput();}
void AAlphaGameplayPlayerController::CloseOverlay(){Overlay=EAlphaOverlay::None;ApplyOverlayInput();}
void AAlphaGameplayPlayerController::ReturnToMainMenu(){SetPause(false);UGameplayStatics::OpenLevel(this,TEXT("L_AlphaStartup"));}
void AAlphaGameplayPlayerController::SetQualityPreset(int32 Level){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetOverallScalabilityLevel(FMath::Clamp(Level,0,3));S->ApplySettings(false);S->SaveSettings();LastSystemMessage=TEXT("Graphics preset applied");}}
void AAlphaGameplayPlayerController::ToggleVSync(){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetVSyncEnabled(!S->IsVSyncEnabled());S->ApplySettings(false);S->SaveSettings();LastSystemMessage=S->IsVSyncEnabled()?TEXT("VSync enabled"):TEXT("VSync disabled");}}

void AAlphaGameplayPlayerController::SetResolutionPreset(int32 Width,int32 Height){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetScreenResolution(FIntPoint(FMath::Max(1280,Width),FMath::Max(720,Height)));S->SetFullscreenMode(EWindowMode::WindowedFullscreen);S->ApplySettings(false);S->SaveSettings();LastSystemMessage=FString::Printf(TEXT("Resolution %dx%d applied"),Width,Height);}}

void AAlphaGameplayPlayerController::TogglePathTracing(){bPathTracingEnabled=!bPathTracingEnabled;UKismetRenderingLibrary::EnablePathTracing(bPathTracingEnabled);LastSystemMessage=bPathTracingEnabled?TEXT("Path Tracing enabled - high-end RTX mode"):TEXT("Path Tracing disabled - realtime renderer restored");}
