// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DesignLaws.h"

/** Exact M1 subsystem values. Project-wide locks remain in DesignLaws.h. */
namespace DarkArisen::CoreLoopTuning
{
    constexpr float FramesToSeconds(const int32 Frames)
    {
        return static_cast<float>(Frames) /
            static_cast<float>(DesignLaws::TargetFramesPerSecond);
    }

    inline constexpr float DeathHoldSeconds = 4.0f;
    inline constexpr float EmergenceAnchorSeconds = 6.0f;
    inline constexpr float InteractionPromptSeconds = 4.0f;
    inline constexpr float DefaultTakeSeconds = 0.6f;

    inline constexpr float WindedStaminaThreshold = 0.30f;
    inline constexpr float HurtHealthThreshold = 0.60f;
    inline constexpr float BadHealthThreshold = 0.30f;
    inline constexpr float FailingHealthThreshold = 0.12f;

    inline constexpr float PostureSetThreshold = 0.80f;
    inline constexpr float PosturePressedThreshold = 0.55f;
    inline constexpr float PostureFailingThreshold = 0.30f;
    inline constexpr float PostureBrokenImminentThreshold = 0.10f;
    inline constexpr float PostureBreakVulnerabilitySeconds = 1.4f;
    inline constexpr float PostureResetFraction = 0.50f;
    inline constexpr float PostureRegenDelaySeconds = 3.0f;
    inline constexpr float PostureRegenPerSecond = 30.0f;

    inline constexpr int32 LightStartupFrames = 9;
    inline constexpr int32 LightRecoveryFrames = 14;
    inline constexpr int32 MediumStartupFrames = 13;
    inline constexpr int32 MediumRecoveryFrames = 20;
    inline constexpr int32 HeavyStartupFrames = 19;
    inline constexpr int32 HeavyRecoveryFrames = 31;
    inline constexpr int32 GreatStartupFrames = 26;
    inline constexpr int32 GreatRecoveryFrames = 44;
    inline constexpr int32 PolearmStartupFrames = 15;
    inline constexpr int32 PolearmRecoveryFrames = 24;

    static_assert(FramesToSeconds(DesignLaws::DeflectionWindowFrames) ==
        DesignLaws::DeflectionWindowSeconds,
        "M1 frame conversion must preserve the locked deflection window.");
}
