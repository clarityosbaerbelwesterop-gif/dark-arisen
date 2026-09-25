#pragma once

// Authoring enums shared by the O3DE Gem and the native ContentSource materialiser.
// Plain C++ with no engine headers: prefabs store these as integers, so the order is a
// serialization contract. Append only; never reorder.

namespace DarkArisen
{
    /** Authored physical beats of the Chapter 1/2 opening (OpeningRuntime signals). */
    enum class StorySignal : int
    {
        FamilyInteraction,
        FleetDetected,
        BoardingStarted,
        BeginBoardingEncounter,
        DravenBoarded,
        TakingStarted,
        TakingCinematicComplete,
        EnteredWater,
        ReachedOuterReef,
        ReachedLocation,
        RecoveryComplete,
        CrewMet,
        CrewRecruitmentAvailable,
        CrewRecruited,
        GalleonCoveCleared,
        LaLiberacionBoarded,
        LaLiberacionHelmSecured,
        LaLiberacionHarborCleared,
        BeginFirstWake,
        ReachRexaHarbor,
        /** Safe rest (Driftwood Camp shelter, later camps): the second legal autosave source. */
        Rest
    };

    /** Enemy data profiles from the engine-neutral core. */
    enum class EnemyProfileKind : int
    {
        Boarder,
        Holder,
        DreamEthan,
        DravenVoss
    };
}
