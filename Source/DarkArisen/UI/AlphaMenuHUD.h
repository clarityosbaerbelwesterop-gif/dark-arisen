#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AlphaMenuHUD.generated.h"
UCLASS() class DARKARISEN_API AAlphaMenuHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
