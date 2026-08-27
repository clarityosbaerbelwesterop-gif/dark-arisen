// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PostureOnlyHUD.generated.h"

/** The sole combat HUD element: one posture indicator and nothing else. */
UCLASS()
class DARKARISEN_API APostureOnlyHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Posture")
    FLinearColor BackgroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.72f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Posture")
    FLinearColor SetColor = FLinearColor(0.82f, 0.62f, 0.10f, 0.92f);

private:
    void DrawPosture();
    void DrawInteractionPrompt();
    void DrawExaminePresentation();
};
