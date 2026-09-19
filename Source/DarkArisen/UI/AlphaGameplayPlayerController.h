#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlphaGameplayPlayerController.generated.h"
UENUM(BlueprintType) enum class EAlphaOverlay:uint8{None,Pause,Map,Settings};
UCLASS() class DARKARISEN_API AAlphaGameplayPlayerController:public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    UPROPERTY(BlueprintReadOnly,Category="Alpha|UI") EAlphaOverlay Overlay=EAlphaOverlay::None;
    UPROPERTY(BlueprintReadOnly,Category="Alpha|UI") bool bMiniMapEnabled=true;
    UPROPERTY(BlueprintReadOnly,Category="Alpha|UI") FString LastSystemMessage;
    UFUNCTION(BlueprintCallable) void TogglePauseMenu();
    UFUNCTION(BlueprintCallable) void ToggleWorldMap();
    UFUNCTION(BlueprintCallable) void ToggleMiniMap();
    UFUNCTION(BlueprintCallable) void QuickSave();
    UFUNCTION(BlueprintCallable) void QuickLoad();
    UFUNCTION(BlueprintCallable) void OpenSettings();
    UFUNCTION(BlueprintCallable) void CloseOverlay();
    UFUNCTION(BlueprintCallable) void ReturnToMainMenu();
    UFUNCTION(BlueprintCallable) void SetQualityPreset(int32 Level);
    UFUNCTION(BlueprintCallable) void ToggleVSync();
    UFUNCTION(BlueprintCallable) void SetResolutionPreset(int32 Width,int32 Height);
    UFUNCTION(BlueprintCallable) void TogglePathTracing();
    UPROPERTY(BlueprintReadOnly,Category="Alpha|Rendering") bool bPathTracingEnabled=false;
private:void ApplyOverlayInput();
};
