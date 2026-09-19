#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PostureOnlyHUD.generated.h"
/** Minimal gameplay HUD: posture + requested minimap, with non-combat map/pause/settings overlays. */
UCLASS() class DARKARISEN_API APostureOnlyHUD:public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    virtual void NotifyHitBoxClick(FName BoxName) override;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Posture") FLinearColor BackgroundColor=FLinearColor(.02f,.02f,.02f,.72f);
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Posture") FLinearColor SetColor=FLinearColor(.82f,.62f,.10f,.92f);
private:
    void DrawPosture();void DrawInteractionPrompt();void DrawExaminePresentation();void DrawMiniMap();void DrawOverlay();void DrawButton(const FString&,FName,float,float,float,float);
};
