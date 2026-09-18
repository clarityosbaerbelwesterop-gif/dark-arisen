#include "AlphaGameplayGameMode.h"
#include "JakeCharacter.h"
#include "PostureOnlyHUD.h"
#include "UI/AlphaGameplayPlayerController.h"
AAlphaGameplayGameMode::AAlphaGameplayGameMode(){DefaultPawnClass=AJakeCharacter::StaticClass();PlayerControllerClass=AAlphaGameplayPlayerController::StaticClass();HUDClass=APostureOnlyHUD::StaticClass();}
