#pragma once

#include <cstdint>

/**
 * Engine-neutral copy of the load-bearing design laws.
 *
 * Source of truth for the numbers: Source/DarkArisen/DesignLaws.h and
 * Source/DarkArisen/CoreLoopTuning.h (Unreal reference). UnrealParityTests
 * reads those headers and fails if any value here drifts from them.
 */
namespace DarkArisen::Core::DesignLaws
{
    inline constexpr int TargetFramesPerSecond = 60;
    inline constexpr int DeflectionWindowFrames = 6;
    inline constexpr float DeflectionWindowSeconds =
        static_cast<float>(DeflectionWindowFrames) / static_cast<float>(TargetFramesPerSecond);

    inline constexpr float InteractionPromptRangeMetres = 1.4f;

    /**
     * Death returns Jake to the last checkpoint after the death beat (mechanics/traversal.md 7.4:
     * two-second death animation, then respawn at last checkpoint). DesignAuthority: no failure screens.
     */
    inline constexpr float DeathToRespawnSeconds = 2.0f;

    inline constexpr float RacheTimeScale = 0.30f;
    inline constexpr float RacheMaximumRealSeconds = 5.0f;

    inline constexpr int AuthoredCutsceneCount = 19;
    inline constexpr int ProhibitedCutsceneMomentCount = 22;
    inline constexpr int LandTraversalExceptionEntries = 3;

    /** Autosave has exactly two legal sources. Manual save is unrestricted. */
    enum class AutosaveTrigger : std::uint8_t
    {
        ChapterBoundary,
        Rest,
        Count
    };

    static_assert(DeflectionWindowSeconds > 0.099f && DeflectionWindowSeconds < 0.101f,
        "Six frames at 60 fps must remain one tenth of a second.");
    static_assert(static_cast<int>(AutosaveTrigger::Count) == 2, "Autosave must have exactly two triggers.");
}

namespace DarkArisen::Core::CoreLoopTuning
{
    constexpr float FramesToSeconds(const int Frames)
    {
        return static_cast<float>(Frames) / static_cast<float>(DesignLaws::TargetFramesPerSecond);
    }

    inline constexpr float PostureSetThreshold = 0.80f;
    inline constexpr float PosturePressedThreshold = 0.55f;
    inline constexpr float PostureFailingThreshold = 0.30f;
    inline constexpr float PostureBrokenImminentThreshold = 0.10f;
    inline constexpr float PostureBreakVulnerabilitySeconds = 1.4f;
    inline constexpr float PostureRegenDelaySeconds = 3.0f;
    inline constexpr float PostureRegenPerSecond = 30.0f;

    inline constexpr int LightStartupFrames = 9;
    inline constexpr int LightRecoveryFrames = 14;
    inline constexpr int MediumStartupFrames = 13;
    inline constexpr int MediumRecoveryFrames = 20;
    inline constexpr int HeavyStartupFrames = 19;
    inline constexpr int HeavyRecoveryFrames = 31;
    inline constexpr int GreatStartupFrames = 26;
    inline constexpr int GreatRecoveryFrames = 44;
    inline constexpr int PolearmStartupFrames = 15;
    inline constexpr int PolearmRecoveryFrames = 24;

    /** Invulnerability granted by a dodge or backstep. */
    inline constexpr int DodgeInvulnerabilityFrames = 8;

    static_assert(FramesToSeconds(DesignLaws::DeflectionWindowFrames) == DesignLaws::DeflectionWindowSeconds,
        "Frame conversion must preserve the locked deflection window.");
}
