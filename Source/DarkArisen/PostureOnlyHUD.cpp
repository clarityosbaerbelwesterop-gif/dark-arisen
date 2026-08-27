// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "PostureOnlyHUD.h"

#include "Components/CombatComponent.h"
#include "DesignLaws.h"
#include "Engine/Canvas.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

static_assert(
    DarkArisen::DesignLaws::PermittedCombatHudElements == 1,
    "The combat HUD may contain posture only.");

void APostureOnlyHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !PlayerOwner) return;
    const APawn* Pawn = PlayerOwner->GetPawn();
    const UCombatComponent* Combat = Pawn
        ? Pawn->FindComponentByClass<UCombatComponent>() : nullptr;
    if (!Combat ||
        (Combat->CurrentState == ECombatState::Idle && Combat->CurrentPosture <= 0.0f)) return;

    const float Width = FMath::Clamp(Canvas->SizeX * 0.22f, 220.0f, 440.0f);
    constexpr float Height = 8.0f;
    const float X = (Canvas->SizeX - Width) * 0.5f;
    const float Y = Canvas->SizeY - 72.0f;
    DrawRect(BackgroundColor, X, Y, Width, Height);
    DrawRect(
        SetColor,
        X,
        Y,
        Width * Combat->GetPostureRemainingFraction(),
        Height);
}
