#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlphaMenuPlayerController.generated.h"
UCLASS() class DARKARISEN_API AAlphaMenuPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") bool StartNewGame();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") bool ContinueGame();
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void SetVSyncEnabled(bool bEnabled);
    UFUNCTION(BlueprintPure,Category="Alpha|Menu") bool HasContinueSave() const;
    UFUNCTION(BlueprintCallable,Category="Alpha|Menu") void QuitGame();
    static const FString& GetAlphaSaveSlot();
};
