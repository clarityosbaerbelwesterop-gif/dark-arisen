#pragma once

#include "DarkArisen/Core/DesignLaws.h"

#include <cstdint>
#include <optional>
#include <string>

/**
 * Engine-neutral port of the Unreal M1 combat loop: UStaminaComponent,
 * UHealthComponent (rally), UCombatComponent and UDamagePipeline.
 *
 * The engine adapter owns animation, collision sweeps and time dilation. It ticks
 * the model, performs the melee sweep when Tick reports MeleeHitDue, and passes the
 * hit target to ResolveMeleeHit. All numbers come from the Unreal reference.
 */
namespace DarkArisen::Core
{
    enum class CombatState : std::uint8_t
    {
        Idle,
        LightAttacking,
        HeavyAttacking,
        Parrying,
        Dodging,
        Backstepping,
        Staggered,
        NonHostile,
        Dead
    };

    enum class WeaponWeightClass : std::uint8_t { Light, Medium, Heavy, Great, Polearm };
    enum class PostureVisualState : std::uint8_t { Set, Pressed, Failing, BrokenImminent, Broken };
    enum class HitKind : std::uint8_t { Light, Heavy, ParryStrike, Critical };
    enum class AttackType : std::uint8_t { Light, Heavy, ParryStrike, Critical, Projectile, Environmental };
    enum class RallyDamageClass : std::uint8_t
    {
        StandardEnemy, EliteEnemy, Boss, Grab, Environmental, Fire, Poison, Fall, Bleed
    };

    struct HitProfile
    {
        float HealthDamage = 0.0f;
        float PostureDamage = 0.0f;
        /** Posture the attacker takes when this hit is deflected. */
        float DeflectedPostureDamage = 0.0f;
    };

    class StaminaModel
    {
    public:
        float MaxStamina = 120.0f;
        float CurrentStamina = 120.0f;
        float BaseRegenRate = 20.0f;
        float RegenDelay = 1.0f;
        float ExhaustionRegenDelay = 3.0f;
        float RegenMultiplier = 1.0f;
        float SprintDrainPerSecond = 10.0f;

        bool CanAfford(float Cost) const { return Cost >= 0.0f && CurrentStamina >= Cost; }
        bool Consume(float Cost);
        void RestoreFromPerfectParry(float Amount);
        void SetSprinting(bool Active) { Sprinting = Active && CurrentStamina > 0.0f; }
        bool IsSprinting() const { return Sprinting; }
        void ResetForRespawn(float Fraction = 1.0f);
        void Tick(float DeltaSeconds);

    private:
        bool Sprinting = false;
        float RegenLockTimer = 0.0f;
    };

    /** Health never passively regenerates. Rally is the explicit, time-limited recovery path. */
    class HealthModel
    {
    public:
        float MaxHealth = 200.0f;
        float CurrentHealth = 200.0f;
        bool RallyEnabled = false;
        float RallyWindowSeconds = 3.0f;
        bool DamageImmune = false;

        bool IsDead() const { return Dead; }
        float GetHealthFraction() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }
        float GetRallyAvailable() const { return RallyAvailableHealth; }
        /** Returns the damage actually applied. */
        float ApplyDamage(float Amount, RallyDamageClass DamageClass = RallyDamageClass::StandardEnemy);
        float RecoverRally(HitKind RecoveryAction);
        void Heal(float Amount);
        void ResetForRespawn(float Fraction);
        void Tick(float DeltaSeconds);

        static float RallyFraction(RallyDamageClass DamageClass);
        static float RecoveryFraction(HitKind RecoveryAction);

    private:
        bool Dead = false;
        float RallyAvailableHealth = 0.0f;
        float RallyWindowRemaining = 0.0f;
        void ExpireRally();
    };

    struct CombatTickEvents
    {
        /** The frame-timer fallback reached the active frame; the adapter should sweep now. */
        std::optional<HitKind> MeleeHitDue;
        bool ReturnedToIdle = false;
        bool RacheEnded = false;
    };

    class CombatModel
    {
    public:
        float MaxPosture = 100.0f;
        /** Posture damage accumulated: zero is fully set, MaxPosture is broken. */
        float CurrentPosture = 0.0f;
        float PoiseMultiplier = 1.0f;
        float ArmorFraction = 0.0f;
        float PhysicalResistance = 0.0f;
        float MaxRache = 100.0f;
        bool RacheUnlocked = false;

        static constexpr float LightAttackStaminaCost = 15.0f;
        static constexpr float HeavyAttackStaminaCost = 30.0f;
        static constexpr float ParryStaminaCost = 10.0f;
        static constexpr float DodgeStaminaCost = 25.0f;
        static constexpr float BackstepStaminaCost = 10.0f;
        static constexpr float RacheDrainPerRealSecond = 20.0f;

        bool PerformLightAttack(StaminaModel& Stamina);
        bool PerformHeavyAttack(StaminaModel& Stamina);
        bool PerformParry(StaminaModel& Stamina);
        /** A zero-length direction is a backstep. */
        bool PerformDodge(StaminaModel& Stamina, bool Backstep);
        /** Animation may request completion but cannot bypass the commitment timer. */
        void FinishAction();
        /** The authored animation notify owns the hit frame from now on. */
        void RouteQueuedHitToAnimation() { PendingHitUsesFrameTimer = false; }
        /** Consumes the queued hit for an animation notify. Returns the kind to resolve. */
        std::optional<HitKind> ConsumeQueuedHit(HitKind Expected);

        void AddPostureDamage(float Amount);
        void SetDead();
        void SetNonHostile();
        void ResetAfterRespawn();

        bool StartRache();
        void StopRache();
        void AddRacheFuel(float Percent);
        float GetRache() const { return CurrentRache; }
        bool IsRacheActive() const { return RacheActive; }
        /** World time scale the adapter must apply. */
        float GetTimeScale() const { return RacheActive ? DesignLaws::RacheTimeScale : 1.0f; }

        CombatTickEvents Tick(float GameDeltaSeconds, float RealDeltaSeconds);

        void SetWeightClass(WeaponWeightClass WeightClass);
        WeaponWeightClass GetWeightClass() const { return WeightClass; }
        int GetStartupFrames() const;
        int GetRecoveryFrames() const;
        float GetMinimumCommitmentSeconds() const;

        CombatState GetState() const { return State; }
        bool IsTargetable() const { return State != CombatState::Dead && State != CombatState::NonHostile; }
        bool IsDeflectionWindowOpen() const { return DeflectionWindowRemaining > 0.0f; }
        bool IsInvulnerable() const { return InvulnerabilityRemaining > 0.0f; }
        bool IsActionCommitted() const { return ActionCommitmentRemaining > 0.0f; }
        float GetPostureRemainingFraction() const;
        PostureVisualState GetPostureVisualState() const;
        float GetResistance(AttackType Type) const { return Type == AttackType::Environmental ? 0.0f : PhysicalResistance; }

        static PostureVisualState EvaluatePostureVisualState(float RemainingFraction);
        static HitProfile GetHitProfile(HitKind Kind);

    private:
        CombatState State = CombatState::Idle;
        WeaponWeightClass WeightClass = WeaponWeightClass::Medium;
        float ActionCommitmentRemaining = 0.0f;
        float DeflectionWindowRemaining = 0.0f;
        float InvulnerabilityRemaining = 0.0f;
        float PostureRegenDelayRemaining = 0.0f;
        float PendingHitDelayRemaining = 0.0f;
        std::optional<HitKind> PendingHit;
        bool PendingHitUsesFrameTimer = true;
        float CurrentRache = 0.0f;
        bool RacheActive = false;
        float RacheRealSecondsRemaining = 0.0f;

        bool BeginCommittedAction(StaminaModel& Stamina, CombatState NewState, float StaminaCost, float Duration);
        void QueueMeleeHit(HitKind Kind);
        void CompleteStagger();
    };

    /** One combat participant. EntityId drives canon policy (see EntityPolicy.h). */
    struct Combatant
    {
        std::string EntityId;
        HealthModel Health;
        StaminaModel Stamina;
        CombatModel Combat;
    };

    struct DamageContext
    {
        AttackType Type = AttackType::Light;
        float BaseDamage = 0.0f;
        float PostureDamage = 0.0f;
        RallyDamageClass DamageClass = RallyDamageClass::StandardEnemy;
    };

    struct DamageResult
    {
        bool Resolved = false;
        float HealthDamage = 0.0f;
        float PostureDamage = 0.0f;
        bool Deflected = false;
        bool Invulnerable = false;
        bool Staggered = false;
        bool Killed = false;
    };

    class DamagePipeline
    {
    public:
        /** Unresolved (Resolved=false) for invalid input or a canon-protected target. */
        static DamageResult Resolve(Combatant& Source, Combatant& Target, const DamageContext& Context);
    };

    /**
     * Resolves one authored melee contact from Attacker's queued/animation hit.
     * Deflected hits cost the attacker the profile's deflected posture damage in
     * addition to the pipeline's generic 1.5x posture reflection (Unreal parity).
     */
    DamageResult ResolveMeleeHit(Combatant& Attacker, Combatant& Target, HitKind Kind);
}
