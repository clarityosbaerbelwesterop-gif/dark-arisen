// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Build-visible constants for the twelve load-bearing design laws.
 * The full reasoning remains in the read-only design bible.
 *
 *  1. The player reads the world, not a HUD. The sole combat-HUD concession is posture.
 *  2. Interactions have no highlight and no fade; a corner prompt is allowed only at 1.4 m.
 *  3. Sixty frames per second is the floor because deflection is exactly six frames.
 *  4. Time dilation exists only for player-triggered Rache: 30% scale for at most five seconds.
 *  5. Music never reacts to an event; a cue starts, runs, and stops on its own schedule.
 *  6. The twenty-two prohibited cinematic moments remain under player control.
 *  7. Water travel is never skipped; the three land entries are strictly authored exceptions.
 *  8. Autosave fires only at a chapter boundary or rest, and is suppressed around the lake.
 *  9. Body, Craft, and Standing never convert into one another; no respec exists.
 * 10. No hostile input, targeting, damage, force, or ragdoll path can connect with a child.
 * 11. There is no systemic romance; the one authored exception is not a dialogue-tree system.
 * 12. The Belos undercroft is never rendered or shown.
 */
namespace DarkArisen::DesignLaws
{
    inline constexpr int32 TargetFramesPerSecond = 60;
    inline constexpr int32 DeflectionWindowFrames = 6;
    inline constexpr float DeflectionWindowSeconds =
        static_cast<float>(DeflectionWindowFrames) / static_cast<float>(TargetFramesPerSecond);

    inline constexpr float InteractionPromptRangeMetres = 1.4f;
    inline constexpr int32 PermittedCombatHudElements = 1;

    inline constexpr float RacheTimeScale = 0.30f;
    inline constexpr float RacheMaximumRealSeconds = 5.0f;

    inline constexpr int32 AuthoredCutsceneCount = 19;
    inline constexpr int32 ProhibitedCutsceneMomentCount = 22;
    inline constexpr int32 LandTraversalExceptionEntries = 3;

    enum class EAutosaveTrigger : uint8
    {
        ChapterBoundary,
        Rest,
        Count
    };

    /** Law 9. The three axes are separate kinds of capability and never exchange. */
    enum class EProgressionAxis : uint8
    {
        Body,
        Craft,
        Standing,
        Count
    };

    /**
     * Law 9 mirrors of the M4 catalog shape. UProgressionEconomyComponent owns the runtime
     * enforcement; these exist so the same numbers also fail the compile.
     *
     * These asserts are not engine-independent — this header includes CoreMinimal.h and there is
     * no non-UE build path in the repository — but they fire during the module compile, which is
     * strictly earlier than the automation tests and the DarkArisenOps validator. A drift between
     * these numbers and the component's constants breaks the build rather than a later gate.
     */
    inline constexpr int32 CraftNodeCount = 68;
    inline constexpr int32 CraftNodeTotalMarkCost = 141;
    inline constexpr int32 MarksAvailableInOnePlaythrough = 94;
    inline constexpr int32 NamedTeacherCount = 23;

    /** Law 9. No reallocation, no consumable, no NPC service, no paid concession. */
    inline constexpr bool bRespecExists = false;

    /** Law 9. Money buys equipment, ships, settlements, armies and information — never capability. */
    inline constexpr bool bCapabilityIsPurchasable = false;

    static_assert(TargetFramesPerSecond == 60, "Design law: 60 fps is the hard floor.");
    static_assert(DeflectionWindowFrames == 6, "Design law: deflection is exactly six frames.");
    static_assert(DeflectionWindowSeconds > 0.099f && DeflectionWindowSeconds < 0.101f,
        "Six frames at 60 fps must remain one tenth of a second.");
    static_assert(PermittedCombatHudElements == 1, "Only posture may appear in the combat HUD.");
    static_assert(RacheTimeScale == 0.30f, "Rache is the sole 30% time-scale exception.");
    static_assert(RacheMaximumRealSeconds == 5.0f, "Rache may last no more than five real seconds.");
    static_assert(AuthoredCutsceneCount == 19, "The authored cutscene catalog is locked at nineteen.");
    static_assert(ProhibitedCutsceneMomentCount == 22,
        "The prohibited cinematic list is locked at twenty-two.");
    static_assert(static_cast<uint8>(EAutosaveTrigger::Count) == 2,
        "Autosave must have exactly two triggers.");

    static_assert(InteractionPromptRangeMetres > 1.39f && InteractionPromptRangeMetres < 1.41f,
        "Design law: the corner prompt exists only at 1.4 m.");
    static_assert(LandTraversalExceptionEntries == 3,
        "Design law: water travel is never skipped; exactly three authored land exceptions.");

    static_assert(static_cast<uint8>(EProgressionAxis::Count) == 3,
        "Design law: Body, Craft and Standing are the only three axes.");
    static_assert(!bRespecExists, "Design law: no respec exists.");
    static_assert(!bCapabilityIsPurchasable, "Design law: money never buys capability.");
    static_assert(CraftNodeCount == 68, "The Craft tree is locked at sixty-eight nodes.");
    static_assert(CraftNodeTotalMarkCost == 141,
        "Opening every Craft node costs exactly one hundred and forty-one Marks.");
    static_assert(MarksAvailableInOnePlaythrough == 94,
        "A perfect playthrough yields exactly ninety-four Marks.");
    static_assert(MarksAvailableInOnePlaythrough < CraftNodeTotalMarkCost,
        "Incompleteness is the design: available Marks must never open the whole tree.");
    static_assert(NamedTeacherCount == 23, "The teacher register is locked at twenty-three.");
}

