#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AlphaMenuHUD.generated.h"
UCLASS() class DARKARISEN_API AAlphaMenuHUD:public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    virtual void NotifyHitBoxClick(FName BoxName) override;
private:
    void DrawButton(const FString& Label,FName Id,float X,float Y,float W,float H,bool bEnabled=true);
};
