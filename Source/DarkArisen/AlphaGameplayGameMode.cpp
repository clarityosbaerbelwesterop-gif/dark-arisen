#include "AlphaGameplayGameMode.h"
#include "JakeCharacter.h"
#include "PostureOnlyHUD.h"
AAlphaGameplayGameMode::AAlphaGameplayGameMode()
{
    DefaultPawnClass=AJakeCharacter::StaticClass();
    HUDClass=APostureOnlyHUD::StaticClass();
}
