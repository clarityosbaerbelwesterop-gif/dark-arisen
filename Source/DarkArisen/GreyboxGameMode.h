// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GreyboxGameMode.generated.h"

class UMainStoryDirectorComponent;

/** Creates the native test/preview room and owns the finite critical-path director. */
UCLASS()
class DARKARISEN_API AGreyboxGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AGreyboxGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    virtual void StartPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
    TObjectPtr<UMainStoryDirectorComponent> MainStoryDirectorComponent;

private:
    void BuildGreybox();
};
