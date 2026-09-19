#include "AlphaStartupGameMode.h"
#include "UI/AlphaMenuHUD.h"
#include "UI/AlphaMenuPlayerController.h"
AAlphaStartupGameMode::AAlphaStartupGameMode()
{
    PlayerControllerClass=AAlphaMenuPlayerController::StaticClass();
    HUDClass=AAlphaMenuHUD::StaticClass();
    DefaultPawnClass=nullptr;
}
