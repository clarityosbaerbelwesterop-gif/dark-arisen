// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GreyboxGameMode.generated.h"

/** Creates the M0 test room entirely in C++, so no placeholder binary map is required. */
UCLASS()
class DARKARISEN_API AGreyboxGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AGreyboxGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    virtual void StartPlay() override;

private:
    void BuildGreybox();
};

