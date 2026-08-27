// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "PostureOnlyHUD.h"

#include "Components/CombatComponent.h"
#include "Components/InteractionComponent.h"
#include "DesignLaws.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

static_assert(
    DarkArisen::DesignLaws::PermittedCombatHudElements == 1,
    "The combat HUD may contain posture only.");

void APostureOnlyHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !PlayerOwner) return;
    DrawPosture();
    DrawInteractionPrompt();
    DrawExaminePresentation();
}

void APostureOnlyHUD::DrawPosture()
{
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

void APostureOnlyHUD::DrawInteractionPrompt()
{
    const APawn* Pawn = PlayerOwner->GetPawn();
    const UInteractionComponent* Interaction = Pawn
        ? Pawn->FindComponentByClass<UInteractionComponent>() : nullptr;
    if (!Interaction || !Interaction->IsPromptVisible() || Interaction->IsExamineVisible()) return;
    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    if (!Font) return;
    const FString Prompt = Interaction->GetPromptLabel().ToString();
    float Width = 0.0f;
    float Height = 0.0f;
    Canvas->StrLen(Font, Prompt, Width, Height);
    constexpr float Margin = 28.0f;
    DrawText(
        Prompt,
        FLinearColor(0.88f, 0.86f, 0.78f, 0.92f),
        Canvas->SizeX - Width - Margin,
        Canvas->SizeY - Height - Margin,
        Font,
        1.0f,
        false);
}

void APostureOnlyHUD::DrawExaminePresentation()
{
    const APawn* Pawn = PlayerOwner->GetPawn();
    const UInteractionComponent* Interaction = Pawn
        ? Pawn->FindComponentByClass<UInteractionComponent>() : nullptr;
    if (!Interaction || !Interaction->IsExamineVisible()) return;
    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    if (!Font) return;

    const float Width = FMath::Min(Canvas->SizeX * 0.42f, 620.0f);
    const float X = Canvas->SizeX - Width - 48.0f;
    const float Y = Canvas->SizeY * 0.18f;
    DrawRect(FLinearColor(0.03f, 0.025f, 0.02f, 0.74f), X, Y, Width, Canvas->SizeY * 0.60f);
    DrawText(
        Interaction->GetExamineTitle().ToString(),
        FLinearColor(0.92f, 0.86f, 0.70f, 1.0f),
        X + 24.0f,
        Y + 22.0f,
        Font,
        1.25f,
        false);
    DrawText(
        Interaction->GetExamineBody().ToString(),
        FLinearColor(0.88f, 0.86f, 0.80f, 1.0f),
        X + 24.0f,
        Y + 64.0f,
        Font,
        1.0f,
        false);
}
