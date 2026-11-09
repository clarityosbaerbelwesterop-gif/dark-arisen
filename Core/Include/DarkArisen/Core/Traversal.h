#pragma once

#include <cstdint>

/**
 * Traversal model after docs/design/mechanics/traversal.md: gaits and their acceleration (2.1, 3.1, 4.1), ground
 * surfaces (2.3, 4.5), sprint exhaustion (4.3), obstacles while running (3.4), jumps and gaps (5.1, 5.5), climbing and
 * grip (6.3, 6.5, 6.6), falls and their mitigation (6.5, 7.1, 7.2), swimming (8.3, 8.4), horses (10.5), the shared
 * stamina pool (11.1, 11.2) and weather (12).
 *
 * Reconciled with higher authorities (Docs/DesignAuthority.md canon order):
 * - The stamina pool is Combat's StaminaModel (120 points, sprint drain 10/s). Its regeneration delays (one second
 *   after a cost, three after exhaustion) are the locked M0 values, so 11.2's "5/s for 3 s while exhausted" becomes
 *   no regeneration for those three seconds. 11.2's per-state rates set the rate once the delay has run out.
 * - 7.4's "respawn at last checkpoint" is the chapter/rest autosave discipline: death returns to the last rest.
 * - 10.5 puts horses on the Quiet Coast (Fjordlund) only; the O3DE Alpha plan's "Highmoore" is corrected to it.
 */
namespace DarkArisen::Core
{
    class StaminaModel;

    enum class Gait : std::uint8_t { Idle, Walk, Run, Sprint };

    /** Ground under Jake's feet (2.3, 4.5, 12.4). DeepGrass is 4.5's "deep grass, brush"; DeepSnow is 12.4's deep snow. */
    enum class Surface : std::uint8_t { Stone, Wood, Sand, Mud, ShallowWater, Snow, DeepSnow, Ice, Grass, DeepGrass, Metal, Cloth, WetDeck };

    enum class Weather : std::uint8_t { Clear, Rain, Storm, Fog, Snow, Hurricane, Heat };

    struct GaitSpec
    {
        float Speed = 0.0f;               // m/s
        float AccelerationSeconds = 0.0f; // to full speed
        float DecelerationSeconds = 0.0f; // to a stop
        float StaminaPerSecond = 0.0f;    // drain
    };

    /** 2.1 walk 3.5 m/s, 3.1 run 6.5 m/s, 4.1 sprint 10 m/s (10 stamina/s). */
    GaitSpec SpecFor(Gait G);
    /** Fraction of the gait's speed on a surface: 2.3 for walking and running, 4.5 for sprinting. */
    float SurfaceSpeedFactor(Surface Ground, Gait G);
    /** 12.1 rain: -15% on stone, wood and metal; 12.2 storm (and worse): -25% on everything. */
    float WeatherSpeedFactor(Weather Sky, Surface Ground);

    /** 3.4: what a running Jake does at an obstacle of the given height. */
    enum class ObstacleResponse : std::uint8_t { StepOver, AutoVault, VaultOnJump, JumpAndGrab, Climb };
    ObstacleResponse RespondToObstacle(float HeightMetres, bool Running, bool JumpPressed);

    struct JumpSpec
    {
        float Height = 0.0f;
        float Distance = 0.0f;
        float StaminaCost = 12.0f;
        int WindupFrames = 6;
        int RecoveryFrames = 12;
    };
    /** 5.1: standing 1.2 m / 2.5 m, running 4.0 m, sprinting 2.0 m high and 5.5 m far with an 8-frame recovery. */
    JumpSpec JumpFor(Gait G);

    /** 5.5: under 2.5 m Jake leaps; up to 5.5 m he hesitates for a tell frame; beyond, the jump does not execute. */
    enum class GapVerdict : std::uint8_t { Safe, Dangerous, Impossible };
    GapVerdict ReadGap(float DistanceMetres);
    /** Whether a jump from this gait clears the gap (a dangerous gap can still be too far for a standing jump). */
    bool JumpClears(float DistanceMetres, Gait G);

    /** 6.3 / 6.5 / 6.6 climbing, scaled by the weather. Impossible in a hurricane or on ice. */
    struct ClimbSpec
    {
        bool Possible = true;
        float UpSpeed = 2.0f;          // m/s, hand over hand (0.5 s per hand move)
        float SidewaysSpeed = 1.5f;    // m/s
        float StaminaPerSecond = 5.0f;
        float MaxContinuousHeight = 50.0f;
        float TopRecoverySeconds = 2.0f;
    };
    ClimbSpec ClimbFor(Weather Sky, bool IceCovered = false);

    /** 6.5 grip: stamina is grip. Straining at 25%, slipping at 10%, falling at zero. */
    enum class Grip : std::uint8_t { Firm, Straining, Slipping, Lost };
    Grip GripFor(float Stamina, float MaxStamina);

    /** What breaks a fall (7.2). */
    enum class Landing : std::uint8_t { Ground, DeepWater, ShallowWater, HayNetOrSail, GrassOrLeaves, MudOrSand, Snow, Enemy };
    /** Why Jake fell: a jump or a step off an edge (7.1) or grip lost on a climb (6.5). */
    enum class FallCause : std::uint8_t { Fell, GripLost };

    struct FallResult
    {
        float HealthFraction = 0.0f;  // of maximum health; 1 or more is death
        bool Fatal = false;
        bool MayKnockOut = false;     // 7.1: 10-15 m
        float StaminaCost = 0.0f;     // a landing roll costs 25
    };
    /**
     * 7.1's table (or 6.5's for a lost grip), then 7.2: deep water replaces the table (none under 15 m, 20% to 30 m, 60%
     * beyond); a timed roll halves the damage; soft landings and enemies reduce it. 25 m or more onto anything but
     * deep water is fatal whatever breaks the fall ("guaranteed fatal").
     */
    FallResult ResolveFall(float HeightMetres, FallCause Cause, Landing Onto, bool TimedRoll);
    /** 7.2: the roll's input window is 12 frames ending at ground contact. */
    bool RollTimed(float SecondsBeforeContact);

    /** 8.3 / 8.4 swimming. */
    struct SwimSpec
    {
        float SurfaceSpeed = 2.5f;
        float SprintSpeed = 4.0f;
        float StaminaPerSecond = 8.0f;
        float SprintStaminaPerSecond = 15.0f;
        float UnderwaterSpeed = 2.0f;
        float ComfortableDepth = 20.0f;
        float MaximumDepth = 30.0f;   // forced ascent
    };
    SwimSpec Swimming();

    /** 10.5: horses exist on the Quiet Coast only; 14 m/s at a gallop on their own stamina pool. */
    enum class Region : std::uint8_t { Moran, Rexa, Mire, Highmoore, OpenSea, FortCarrion, Ashenmoor, QuietCoast, TheMaw };
    bool HorsesIn(Region Where);
    constexpr float HorseGallopSpeed = 14.0f;

    /** What Jake is doing this frame, for the stamina rules of 11.2. */
    struct TraversalInput
    {
        Gait Wanted = Gait::Idle;
        Surface Ground = Surface::Stone;
        Weather Sky = Weather::Clear;
        bool Climbing = false;
        bool Swimming = false;
        bool InCombat = false;
        bool Attacking = false;
        bool WeaponHolstered = true;
        bool CarriesWater = true;      // 12.5: in the heat, regeneration drops 15% without water
    };

    /**
     * Ground movement and the shared stamina pool. Tick advances speed towards the wanted gait (with 2.1/3.1/4.1's
     * acceleration and deceleration), applies sprint exhaustion (4.3: forced walk for 3 s, no sprint, attacks at 75%)
     * and the minimum sprint of 0.5 s, drains the pool while sprinting, climbing or swimming (4.1, 6.6, 8.3) and sets
     * the rate it regenerates at otherwise (11.2, 12.5). It owns the drain, so the pool's own sprint flag stays off; the
     * pool's owner still ticks the pool (its regeneration delays) once per frame.
     */
    class TraversalModel
    {
    public:
        void Tick(float DeltaSeconds, const TraversalInput& In, StaminaModel& Stamina);

        float GetSpeed() const { return Speed; }
        Gait GetGait() const { return Current; }
        bool IsExhausted() const { return ExhaustedFor > 0.0f; }
        /** 4.3: attacks deal 25% less while exhausted. */
        float AttackDamageMultiplier() const { return IsExhausted() ? 0.75f : 1.0f; }

        static constexpr float ExhaustionSeconds = 3.0f;
        static constexpr float MinimumSprintSeconds = 0.5f;

    private:
        Gait Current = Gait::Idle;
        float Speed = 0.0f;
        float ExhaustedFor = 0.0f;
        float SprintedFor = 0.0f;
    };

    /** 11.2: regeneration per second once the pool's delay has run out. */
    float StaminaRegenFor(const TraversalInput& In);
}
