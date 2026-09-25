#pragma once

#include <cstdint>

namespace DarkArisen::Core
{
    class HealthModel;
    class StaminaModel;

    /** water_physics.md 6.3 Breath Holding: 30 / 60 / 90 seconds, equipment indefinite. */
    enum class BreathTier : std::uint8_t { Bare, Trained, Master, Equipment };

    /**
     * Port of UWaterBreathComponent. Breath only drains while the head is under water and refills
     * the moment it surfaces. At zero breath, drowning deals 20 damage for every whole second
     * underwater. That damage counts as Environmental for rally purposes.
     */
    class BreathModel
    {
    public:
        static constexpr float DrowningDamagePerSecond = 20.0f;
        /** 0 means indefinite (equipment). */
        static float MaximumSecondsForTier(BreathTier Tier);

        void SetTier(BreathTier NewTier);
        BreathTier GetTier() const { return Tier; }
        bool HasIndefiniteBreath() const { return MaximumSecondsForTier(Tier) <= 0.0f; }

        void SetHeadSubmerged(bool Submerged);
        bool IsHeadSubmerged() const { return HeadSubmerged; }

        float GetRemainingSeconds() const { return RemainingSeconds; }
        /** 1 when full or indefinite, 0 when drowning. */
        float GetRemainingFraction() const;
        bool IsDrowning() const { return HeadSubmerged && !HasIndefiniteBreath() && RemainingSeconds <= 0.0f; }

        /**
         * Advances breath. Returns the drowning damage that fell due this tick: always a whole
         * number of seconds times DrowningDamagePerSecond, carrying partial seconds forward.
         */
        float Tick(float DeltaSeconds, bool OwnerDead);
        /** Tick and apply the drowning damage to Health as Environmental damage. */
        float TickAndApply(float DeltaSeconds, HealthModel& Health);

        void ResetForRespawn();

    private:
        BreathTier Tier = BreathTier::Bare;
        bool HeadSubmerged = false;
        float RemainingSeconds = 30.0f;
        float DrowningSeconds = 0.0f;

        void Refill();
    };

    /** movement_physics.md 6.1: casual 1 m/s, active 1.5 m/s, sprint 2 m/s (limited). */
    enum class SwimPace : std::uint8_t { Casual, Active, Sprint };

    struct SwimTuning
    {
        // Design authority: movement_physics.md 6.1 Specific Speeds.
        float CasualSpeed = 1.0f;
        float ActiveSpeed = 1.5f;
        float SprintSpeed = 2.0f;

        // PROVISIONAL: the design fixes only the direction of these values ("higher than land",
        // "slow in water", "sometimes impossible"). They need a design ruling before release.
        /** Water drag rate (1/s). A current's steady drift equals its acceleration divided by this. */
        float WaterDragPerSecond = 1.0f;
        /** Casual strokes are sustainable: no drain. */
        float ActiveDrainPerSecond = 3.0f;
        /** Higher than the 10/s land sprint drain. */
        float SprintDrainPerSecond = 15.0f;
        /** Extra drain per m/s of current drift opposing the stroke; a helping current reduces drain. */
        float CurrentDrainPerMetrePerSecond = 4.0f;
        /** Land regen is multiplied by this while in water ("recovery slow in water"). */
        float InWaterRegenMultiplier = 0.25f;
        /** After exhaustion, only casual strokes until stamina is back to this fraction. */
        float ExhaustionRecoveryFraction = 0.25f;
    };

    struct SwimStep
    {
        double VelocityX = 0.0;   // m/s, world XY
        double VelocityY = 0.0;
        SwimPace EffectivePace = SwimPace::Casual;
        float StaminaDrained = 0.0f;
        bool Exhausted = false;
    };

    /**
     * Surface swimming. The swimmer's velocity relaxes towards the stroke velocity under water
     * drag, while authored currents add a constant acceleration (the ContentSource volumes carry
     * cm/s^2, as the Unreal volumes applied them). The integration is exact for any tick length:
     *   dv/dt = a_current + drag * (v_stroke - v)
     * so a current's steady drift is a_current / drag and a strong opposing current can exceed
     * a sprint (design: "against current ... sometimes impossible").
     */
    class SwimModel
    {
    public:
        SwimTuning Tuning;

        /** Enter the water, keeping the horizontal velocity the swimmer arrived with. */
        void Enter(double VelocityX, double VelocityY);
        void Exit();
        bool IsSwimming() const { return Swimming; }
        bool IsExhausted() const { return Exhausted; }

        /** m/s^2, world XY. Zero in open water without an authored current. */
        void SetCurrentAcceleration(double AccelerationX, double AccelerationY);
        double GetCurrentDriftX() const;
        double GetCurrentDriftY() const;

        float SpeedForPace(SwimPace Pace) const;

        /**
         * One tick. IntentX/Y is the stick direction (clamped to length 1). Stamina drain is taken
         * from Stamina; regeneration stays with StaminaModel::Tick (use InWaterRegenMultiplier).
         */
        SwimStep Tick(double IntentX, double IntentY, SwimPace RequestedPace, StaminaModel& Stamina, float DeltaSeconds);

        double GetVelocityX() const { return VelocityX; }
        double GetVelocityY() const { return VelocityY; }

    private:
        bool Swimming = false;
        bool Exhausted = false;
        double VelocityX = 0.0;
        double VelocityY = 0.0;
        double CurrentX = 0.0;
        double CurrentY = 0.0;
    };

    /**
     * Height the character's base (feet) should float at so the eyes sit HeadClearance above the
     * local water surface.
     */
    double FloatingBaseHeight(double SurfaceHeight, double EyeHeightAboveBase, double HeadClearance);

    /** Convert an authored ContentSource current (cm/s^2) to m/s^2. */
    constexpr double CurrentFromCentimetres(double CentimetresPerSecondSquared) { return CentimetresPerSecondSquared / 100.0; }
}
