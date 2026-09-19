#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AlphaGameplayGameMode.generated.h"
UCLASS() class DARKARISEN_API AAlphaGameplayGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AAlphaGameplayGameMode();
    virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;
};
