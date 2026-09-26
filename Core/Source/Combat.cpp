#include "DarkArisen/Core/Combat.h"

#include "DarkArisen/Core/EntityPolicy.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    namespace
    {
        /**
         * Frame-counted timers accumulate float error: six ticks of 1/60 s leave ~1e-9 s of a
         * 0.1 s window, which would hold the deflection window open for a seventh frame.
         * Anything below a tenth of a millisecond counts as expired.
         */
        constexpr float TimerEpsilon = 1.0e-4f;

        float CountDown(const float Remaining, const float Delta)
        {
            const float Next = Remaining - Delta;
            return Next <= TimerEpsilon ? 0.0f : Next;
        }
    }

    // ----------------------------------------------------------------------------- Stamina

    bool StaminaModel::Consume(const float Cost)
    {
        if (Cost <= 0.0f)
        {
            return true;
        }
        if (!CanAfford(Cost))
        {
            return false;
        }
        CurrentStamina = std::max(0.0f, CurrentStamina - Cost);
        RegenLockTimer = CurrentStamina <= 0.0f ? ExhaustionRegenDelay : RegenDelay;
        return true;
    }

    void StaminaModel::RestoreFromPerfectParry(const float Amount)
    {
        if (Amount <= 0.0f)
        {
            return;
        }
        CurrentStamina = std::min(MaxStamina, CurrentStamina + Amount);
        RegenLockTimer = 0.0f;
    }

    void StaminaModel::ResetForRespawn(const float Fraction)
    {
        Sprinting = false;
        RegenLockTimer = 0.0f;
        CurrentStamina = MaxStamina * std::clamp(Fraction, 0.0f, 1.0f);
    }

    void StaminaModel::Tick(const float DeltaSeconds)
    {
        if (Sprinting && CurrentStamina > 0.0f)
        {
            Consume(SprintDrainPerSecond * DeltaSeconds);
            return;
        }
        RegenLockTimer = std::max(0.0f, RegenLockTimer - DeltaSeconds);
        if (RegenLockTimer > 0.0f || CurrentStamina >= MaxStamina)
        {
            return;
        }
        CurrentStamina = std::min(MaxStamina, CurrentStamina + BaseRegenRate * std::max(0.0f, RegenMultiplier) * DeltaSeconds);
    }

    // ----------------------------------------------------------------------------- Health

    float HealthModel::ApplyDamage(const float Amount, const RallyDamageClass DamageClass)
    {
        if (DamageImmune || Dead || Amount <= 0.0f)
        {
            return 0.0f;
        }
        const float Previous = CurrentHealth;
        CurrentHealth = std::clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
        const float Applied = Previous - CurrentHealth;
        if (CurrentHealth <= 0.0f)
        {
            Dead = true;
            ExpireRally();
            return Applied;
        }
        const float Fraction = RallyEnabled ? RallyFraction(DamageClass) : 0.0f;
        if (Fraction > 0.0f && Applied > 0.0f)
        {
            RallyAvailableHealth = std::min(MaxHealth - CurrentHealth, RallyAvailableHealth + Applied * Fraction);
            RallyWindowRemaining = RallyWindowSeconds;
        }
        return Applied;
    }

    float HealthModel::RecoverRally(const HitKind RecoveryAction)
    {
        if (Dead || RallyAvailableHealth <= 0.0f)
        {
            return 0.0f;
        }
        const float Requested = RallyAvailableHealth * RecoveryFraction(RecoveryAction);
        const float Applied = std::min(Requested, MaxHealth - CurrentHealth);
        CurrentHealth += Applied;
        RallyAvailableHealth = std::min(std::max(0.0f, RallyAvailableHealth - Applied), MaxHealth - CurrentHealth);
        return Applied;
    }

    void HealthModel::Heal(const float Amount)
    {
        if (Dead || Amount <= 0.0f)
        {
            return;
        }
        CurrentHealth = std::min(MaxHealth, CurrentHealth + Amount);
        RallyAvailableHealth = std::min(RallyAvailableHealth, MaxHealth - CurrentHealth);
    }

    void HealthModel::ResetForRespawn(const float Fraction)
    {
        Dead = false;
        ExpireRally();
        CurrentHealth = MaxHealth * std::clamp(Fraction, 0.01f, 1.0f);
    }

    void HealthModel::Tick(const float DeltaSeconds)
    {
        if (RallyAvailableHealth > 0.0f)
        {
            RallyWindowRemaining = std::max(0.0f, RallyWindowRemaining - DeltaSeconds);
            if (RallyWindowRemaining <= 0.0f)
            {
                ExpireRally();
            }
        }
    }

    void HealthModel::ExpireRally()
    {
        RallyAvailableHealth = 0.0f;
        RallyWindowRemaining = 0.0f;
    }

    float HealthModel::RallyFraction(const RallyDamageClass DamageClass)
    {
        switch (DamageClass)
        {
        case RallyDamageClass::StandardEnemy: return 0.70f;
        case RallyDamageClass::EliteEnemy: return 0.60f;
        case RallyDamageClass::Boss: return 0.50f;
        case RallyDamageClass::Grab: return 0.30f;
        case RallyDamageClass::Environmental: return 0.40f;
        case RallyDamageClass::Fire: return 0.25f;
        case RallyDamageClass::Poison: return 0.0f;
        case RallyDamageClass::Fall: return 0.50f;
        case RallyDamageClass::Bleed: return 0.30f;
        }
        return 0.0f;
    }

    float HealthModel::RecoveryFraction(const HitKind RecoveryAction)
    {
        switch (RecoveryAction)
        {
        case HitKind::Light: return 0.15f;
        case HitKind::Heavy: return 0.25f;
        case HitKind::ParryStrike: return 0.40f;
        case HitKind::Critical: return 1.0f;
        }
        return 0.0f;
    }

    // ----------------------------------------------------------------------------- Combat

    bool CombatModel::BeginCommittedAction(
        StaminaModel& Stamina, const CombatState NewState, const float StaminaCost, const float Duration)
    {
        if (State != CombatState::Idle || Duration <= 0.0f || !Stamina.Consume(StaminaCost))
        {
            return false;
        }
        ActionCommitmentRemaining = Duration;
        State = NewState;
        return true;
    }

    void CombatModel::QueueMeleeHit(const HitKind Kind)
    {
        PendingHit = Kind;
        PendingHitDelayRemaining = CoreLoopTuning::FramesToSeconds(GetStartupFrames());
        PendingHitUsesFrameTimer = true;
    }

    bool CombatModel::PerformLightAttack(StaminaModel& Stamina)
    {
        if (!BeginCommittedAction(Stamina, CombatState::LightAttacking, LightAttackStaminaCost, GetMinimumCommitmentSeconds()))
        {
            return false;
        }
        QueueMeleeHit(HitKind::Light);
        return true;
    }

    bool CombatModel::PerformHeavyAttack(StaminaModel& Stamina)
    {
        if (!BeginCommittedAction(Stamina, CombatState::HeavyAttacking, HeavyAttackStaminaCost, GetMinimumCommitmentSeconds()))
        {
            return false;
        }
        QueueMeleeHit(HitKind::Heavy);
        return true;
    }

    bool CombatModel::PerformParry(StaminaModel& Stamina)
    {
        if (!BeginCommittedAction(Stamina, CombatState::Parrying, ParryStaminaCost, DesignLaws::DeflectionWindowSeconds))
        {
            return false;
        }
        DeflectionWindowRemaining = DesignLaws::DeflectionWindowSeconds;
        return true;
    }

    bool CombatModel::PerformDodge(StaminaModel& Stamina, const bool Backstep)
    {
        if (!BeginCommittedAction(Stamina, Backstep ? CombatState::Backstepping : CombatState::Dodging,
                Backstep ? BackstepStaminaCost : DodgeStaminaCost, CoreLoopTuning::FramesToSeconds(GetRecoveryFrames())))
        {
            return false;
        }
        InvulnerabilityRemaining = CoreLoopTuning::FramesToSeconds(CoreLoopTuning::DodgeInvulnerabilityFrames);
        return true;
    }

    void CombatModel::FinishAction()
    {
        if (!IsTargetable() || ActionCommitmentRemaining > 0.0f || DeflectionWindowRemaining > 0.0f)
        {
            return;
        }
        PendingHit.reset();
        State = CombatState::Idle;
    }

    std::optional<HitKind> CombatModel::ConsumeQueuedHit(const HitKind Expected)
    {
        if (!PendingHit || *PendingHit != Expected)
        {
            return std::nullopt;
        }
        PendingHit.reset();
        return Expected;
    }

    void CombatModel::AddPostureDamage(const float Amount)
    {
        if (Amount <= 0.0f || !IsTargetable() || State == CombatState::Staggered)
        {
            return;
        }
        CurrentPosture = std::clamp(CurrentPosture + Amount * std::max(0.0f, PoiseMultiplier), 0.0f, MaxPosture);
        PostureRegenDelayRemaining = CoreLoopTuning::PostureRegenDelaySeconds;
        if (GetPostureVisualState() == PostureVisualState::Broken)
        {
            DeflectionWindowRemaining = 0.0f;
            PendingHit.reset();
            ActionCommitmentRemaining = CoreLoopTuning::PostureBreakVulnerabilitySeconds;
            State = CombatState::Staggered;
        }
    }

    void CombatModel::CompleteStagger()
    {
        CurrentPosture = MaxPosture * 0.5f; // CoreLoopTuning::PostureResetFraction
        PostureRegenDelayRemaining = CoreLoopTuning::PostureRegenDelaySeconds;
        State = CombatState::Idle;
    }

    void CombatModel::SetDead()
    {
        StopRache();
        DeflectionWindowRemaining = 0.0f;
        ActionCommitmentRemaining = 0.0f;
        PendingHit.reset();
        State = CombatState::Dead;
    }

    void CombatModel::SetNonHostile()
    {
        if (State == CombatState::Dead)
        {
            return;
        }
        StopRache();
        DeflectionWindowRemaining = 0.0f;
        ActionCommitmentRemaining = 0.0f;
        PendingHit.reset();
        State = CombatState::NonHostile;
    }

    void CombatModel::ResetAfterRespawn()
    {
        StopRache();
        DeflectionWindowRemaining = 0.0f;
        InvulnerabilityRemaining = 0.0f;
        ActionCommitmentRemaining = 0.0f;
        PostureRegenDelayRemaining = 0.0f;
        PendingHit.reset();
        CurrentPosture = 0.0f;
        State = CombatState::Idle;
    }

    bool CombatModel::StartRache()
    {
        if (!RacheUnlocked || RacheActive || CurrentRache <= 0.0f)
        {
            return false;
        }
        RacheActive = true;
        RacheRealSecondsRemaining = std::min(DesignLaws::RacheMaximumRealSeconds, CurrentRache / RacheDrainPerRealSecond);
        return true;
    }

    void CombatModel::StopRache()
    {
        RacheActive = false;
        RacheRealSecondsRemaining = 0.0f;
    }

    void CombatModel::AddRacheFuel(const float Percent)
    {
        if (Percent <= 0.0f)
        {
            return;
        }
        CurrentRache = std::clamp(CurrentRache + MaxRache * Percent / 100.0f, 0.0f, MaxRache);
    }

    CombatTickEvents CombatModel::Tick(const float GameDeltaSeconds, const float RealDeltaSeconds)
    {
        CombatTickEvents Events;
        const float Delta = std::max(0.0f, GameDeltaSeconds);
        InvulnerabilityRemaining = CountDown(InvulnerabilityRemaining, Delta);
        DeflectionWindowRemaining = CountDown(DeflectionWindowRemaining, Delta);
        if (ActionCommitmentRemaining > 0.0f)
        {
            ActionCommitmentRemaining = CountDown(ActionCommitmentRemaining, Delta);
            if (ActionCommitmentRemaining <= 0.0f)
            {
                if (State == CombatState::Staggered)
                {
                    CompleteStagger();
                }
                else
                {
                    FinishAction();
                }
                Events.ReturnedToIdle = State == CombatState::Idle;
            }
        }
        if (PendingHit && PendingHitUsesFrameTimer)
        {
            PendingHitDelayRemaining = CountDown(PendingHitDelayRemaining, Delta);
            if (PendingHitDelayRemaining <= 0.0f)
            {
                Events.MeleeHitDue = PendingHit;
                PendingHit.reset();
            }
        }
        if (State == CombatState::Idle && CurrentPosture > 0.0f)
        {
            PostureRegenDelayRemaining = std::max(0.0f, PostureRegenDelayRemaining - Delta);
            if (PostureRegenDelayRemaining <= 0.0f)
            {
                CurrentPosture = std::max(0.0f, CurrentPosture - CoreLoopTuning::PostureRegenPerSecond * Delta);
            }
        }
        if (RacheActive)
        {
            const float Real = std::max(0.0f, RealDeltaSeconds);
            RacheRealSecondsRemaining = std::max(0.0f, RacheRealSecondsRemaining - Real);
            CurrentRache = std::max(0.0f, CurrentRache - RacheDrainPerRealSecond * Real);
            if (RacheRealSecondsRemaining <= 0.0f || CurrentRache <= 0.0f)
            {
                StopRache();
                Events.RacheEnded = true;
            }
        }
        return Events;
    }

    void CombatModel::SetWeightClass(const WeaponWeightClass NewWeightClass)
    {
        if (State == CombatState::Idle)
        {
            WeightClass = NewWeightClass;
        }
    }

    int CombatModel::GetStartupFrames() const
    {
        using namespace CoreLoopTuning;
        switch (WeightClass)
        {
        case WeaponWeightClass::Light: return LightStartupFrames;
        case WeaponWeightClass::Medium: return MediumStartupFrames;
        case WeaponWeightClass::Heavy: return HeavyStartupFrames;
        case WeaponWeightClass::Great: return GreatStartupFrames;
        case WeaponWeightClass::Polearm: return PolearmStartupFrames;
        }
        return MediumStartupFrames;
    }

    int CombatModel::GetRecoveryFrames() const
    {
        using namespace CoreLoopTuning;
        switch (WeightClass)
        {
        case WeaponWeightClass::Light: return LightRecoveryFrames;
        case WeaponWeightClass::Medium: return MediumRecoveryFrames;
        case WeaponWeightClass::Heavy: return HeavyRecoveryFrames;
        case WeaponWeightClass::Great: return GreatRecoveryFrames;
        case WeaponWeightClass::Polearm: return PolearmRecoveryFrames;
        }
        return MediumRecoveryFrames;
    }

    float CombatModel::GetMinimumCommitmentSeconds() const
    {
        return CoreLoopTuning::FramesToSeconds(GetStartupFrames() + GetRecoveryFrames());
    }

    float CombatModel::GetPostureRemainingFraction() const
    {
        return MaxPosture > 0.0f ? std::clamp(1.0f - CurrentPosture / MaxPosture, 0.0f, 1.0f) : 0.0f;
    }

    PostureVisualState CombatModel::GetPostureVisualState() const
    {
        return EvaluatePostureVisualState(GetPostureRemainingFraction());
    }

    PostureVisualState CombatModel::EvaluatePostureVisualState(const float RemainingFraction)
    {
        using namespace CoreLoopTuning;
        const float Remaining = std::clamp(RemainingFraction, 0.0f, 1.0f);
        if (Remaining >= PostureSetThreshold) return PostureVisualState::Set;
        if (Remaining >= PosturePressedThreshold) return PostureVisualState::Pressed;
        if (Remaining >= PostureFailingThreshold) return PostureVisualState::Failing;
        if (Remaining >= PostureBrokenImminentThreshold) return PostureVisualState::BrokenImminent;
        return PostureVisualState::Broken;
    }

    HitProfile CombatModel::GetHitProfile(const HitKind Kind)
    {
        switch (Kind)
        {
        case HitKind::Light: return {24.0f, 18.0f, 32.0f};
        case HitKind::Heavy: return {42.0f, 34.0f, 48.0f};
        case HitKind::ParryStrike: return {36.0f, 45.0f, 0.0f};
        case HitKind::Critical: return {72.0f, 100.0f, 0.0f};
        }
        return {};
    }

    // ----------------------------------------------------------------------------- Damage

    DamageResult DamagePipeline::Resolve(Combatant& Source, Combatant& Target, const DamageContext& Context)
    {
        DamageResult Result;
        if (&Source == &Target || Context.BaseDamage < 0.0f || Context.PostureDamage < 0.0f ||
            !std::isfinite(Context.BaseDamage) || !std::isfinite(Context.PostureDamage) ||
            !EntityPolicy::CanBeHostileTarget(Target.EntityId) || Target.Health.IsDead() || !Target.Combat.IsTargetable())
        {
            return Result;
        }
        Result.Resolved = true;
        if (Target.Combat.IsInvulnerable())
        {
            Result.Invulnerable = true;
            return Result;
        }
        // A blade answers a blade or a bullet; nobody deflects an explosion or a falling mast.
        if (Target.Combat.IsDeflectionWindowOpen() && Context.Type != AttackType::Critical && Context.Type != AttackType::Environmental)
        {
            Result.Deflected = true;
            Source.Combat.AddPostureDamage(Context.PostureDamage * 1.5f);
            return Result;
        }
        const float Armor = std::clamp(Target.Combat.ArmorFraction, 0.0f, 0.9f);
        const float Resistance = std::clamp(Target.Combat.GetResistance(Context.Type), 0.0f, 0.9f);
        Result.HealthDamage = Context.BaseDamage * (1.0f - Armor) * (1.0f - Resistance);
        Result.PostureDamage = Context.PostureDamage;
        Target.Health.ApplyDamage(Result.HealthDamage, Context.DamageClass);
        Target.Combat.AddPostureDamage(Result.PostureDamage);
        Result.Staggered = Target.Combat.GetState() == CombatState::Staggered;
        Result.Killed = Target.Health.IsDead();
        if (Result.Killed)
        {
            Target.Combat.SetDead();
        }
        return Result;
    }

    DamageResult ResolveMeleeHit(Combatant& Attacker, Combatant& Target, const HitKind Kind)
    {
        if (!Attacker.Combat.IsTargetable())
        {
            return {};
        }
        const HitProfile Profile = CombatModel::GetHitProfile(Kind);
        DamageContext Context;
        Context.Type = static_cast<AttackType>(static_cast<std::uint8_t>(Kind));
        Context.BaseDamage = Profile.HealthDamage;
        Context.PostureDamage = Profile.PostureDamage;
        const DamageResult Result = DamagePipeline::Resolve(Attacker, Target, Context);
        if (!Result.Resolved)
        {
            return Result;
        }
        if (Result.Deflected)
        {
            Attacker.Combat.AddPostureDamage(Profile.DeflectedPostureDamage);
            return Result;
        }
        if (!Result.Invulnerable)
        {
            Attacker.Health.RecoverRally(Kind);
        }
        return Result;
    }
}
