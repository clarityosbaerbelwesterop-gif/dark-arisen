#include "AlphaGameplayGameMode.h"
#include "JakeCharacter.h"
#include "PostureOnlyHUD.h"
#include "UI/AlphaGameplayPlayerController.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Story/MainStorySubsystem.h"
#include "UI/AlphaMenuPlayerController.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

AAlphaGameplayGameMode::AAlphaGameplayGameMode()
{
    DefaultPawnClass = AJakeCharacter::StaticClass();
    PlayerControllerClass = AAlphaGameplayPlayerController::StaticClass();
    HUDClass = APostureOnlyHUD::StaticClass();
    PrimaryActorTick.bCanEverTick = true;
}

void AAlphaGameplayGameMode::StartPlay()
{
    if (UMainStorySubsystem* Story = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UMainStorySubsystem>() : nullptr)
    {
        if (!Story->RestoreWorldState(GetWorld()))
            UE_LOG(LogTemp, Error, TEXT("Alpha world persistence could not be restored."));
    }
    // Restored clock/NPC records must exist when settlement actors run BeginPlay.
    Super::StartPlay();
}

void AAlphaGameplayGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    auto* Rules = GetWorld()->GetSubsystem<UDarkArisenWorldRulesSubsystem>();
    if (Rules && Rules->ConsumePendingAutosaveRequest())
    {
        auto* Story = GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
        if (!Story || !Story->Save(AAlphaMenuPlayerController::GetAlphaSaveSlot(), 0))
            UE_LOG(LogTemp, Error, TEXT("Requested rest/chapter autosave failed."));
    }
}
