#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlphaMenuPlayerController.generated.h"
UENUM(BlueprintType) enum class EAlphaMenuPage:uint8{Main,Settings};
UCLASS() class DARKARISEN_API AAlphaMenuPlayerController:public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    UPROPERTY(BlueprintReadOnly,Category="Alpha|Menu") EAlphaMenuPage MenuPage=EAlphaMenuPage::Main;
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") bool StartNewGame();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") bool ContinueGame();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void OpenSettings();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void BackToMain();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void SetVSyncEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void ToggleVSync();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void SetQualityPreset(int32 Level);
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void ToggleFullscreen();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void SetResolutionPreset(int32 Width,int32 Height);
    UFUNCTION(BlueprintPure,Category="Alpha|Menu") bool HasContinueSave() const;
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void QuitGame();
    static const FString& GetAlphaSaveSlot();
};
