#include <DarkArisen/Core/Water.h>

#include <DarkArisen/Core/Combat.h>

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    // ----------------------------------------------------------------------------- Breath

    float BreathModel::MaximumSecondsForTier(const BreathTier Tier)
    {
        switch (Tier)
        {
        case BreathTier::Bare: return 30.0f;
        case BreathTier::Trained: return 60.0f;
        case BreathTier::Master: return 90.0f;
        case BreathTier::Equipment: return 0.0f;
        }
        return 30.0f;
    }

    void BreathModel::SetTier(const BreathTier NewTier)
    {
        Tier = NewTier;
        Refill();
    }

    void BreathModel::SetHeadSubmerged(const bool Submerged)
    {
        if (HeadSubmerged == Submerged)
        {
            return;
        }
        HeadSubmerged = Submerged;
        DrowningSeconds = 0.0f;
        if (!HeadSubmerged)
        {
            Refill();
        }
    }

    float BreathModel::GetRemainingFraction() const
    {
        if (HasIndefiniteBreath())
        {
            return 1.0f;
        }
        const float Maximum = MaximumSecondsForTier(Tier);
        return std::clamp(RemainingSeconds / Maximum, 0.0f, 1.0f);
    }

    float BreathModel::Tick(const float DeltaSeconds, const bool OwnerDead)
    {
        if (!HeadSubmerged || HasIndefiniteBreath() || DeltaSeconds <= 0.0f || OwnerDead)
        {
            return 0.0f;
        }
        // Breath that runs out mid-tick starts drowning with the remainder of the tick.
        const float Overflow = std::max(0.0f, DeltaSeconds - RemainingSeconds);
        RemainingSeconds = std::max(0.0f, RemainingSeconds - DeltaSeconds);
        if (RemainingSeconds > 0.0f)
        {
            DrowningSeconds = 0.0f;
            return 0.0f;
        }
        DrowningSeconds += Overflow;
        const float WholeSeconds = std::floor(DrowningSeconds);
        if (WholeSeconds <= 0.0f)
        {
            return 0.0f;
        }
        DrowningSeconds -= WholeSeconds;
        return DrowningDamagePerSecond * WholeSeconds;
    }

    float BreathModel::TickAndApply(const float DeltaSeconds, HealthModel& Health)
    {
        const float Damage = Tick(DeltaSeconds, Health.IsDead());
        return Damage > 0.0f ? Health.ApplyDamage(Damage, RallyDamageClass::Environmental) : 0.0f;
    }

    void BreathModel::ResetForRespawn()
    {
        HeadSubmerged = false;
        Refill();
    }

    void BreathModel::Refill()
    {
        RemainingSeconds = MaximumSecondsForTier(Tier);
        DrowningSeconds = 0.0f;
    }

    // ----------------------------------------------------------------------------- Swimming

    void SwimModel::Enter(const double InVelocityX, const double InVelocityY)
    {
        Swimming = true;
        VelocityX = std::isfinite(InVelocityX) ? InVelocityX : 0.0;
        VelocityY = std::isfinite(InVelocityY) ? InVelocityY : 0.0;
    }

    void SwimModel::Exit()
    {
        Swimming = false;
        Exhausted = false;
        VelocityX = VelocityY = 0.0;
        CurrentX = CurrentY = 0.0;
    }

    void SwimModel::SetCurrentAcceleration(const double AccelerationX, const double AccelerationY)
    {
        CurrentX = std::isfinite(AccelerationX) ? AccelerationX : 0.0;
        CurrentY = std::isfinite(AccelerationY) ? AccelerationY : 0.0;
    }

    double SwimModel::GetCurrentDriftX() const
    {
        return Tuning.WaterDragPerSecond > 0.0f ? CurrentX / Tuning.WaterDragPerSecond : 0.0;
    }

    double SwimModel::GetCurrentDriftY() const
    {
        return Tuning.WaterDragPerSecond > 0.0f ? CurrentY / Tuning.WaterDragPerSecond : 0.0;
    }

    float SwimModel::SpeedForPace(const SwimPace Pace) const
    {
        switch (Pace)
        {
        case SwimPace::Casual: return Tuning.CasualSpeed;
        case SwimPace::Active: return Tuning.ActiveSpeed;
        case SwimPace::Sprint: return Tuning.SprintSpeed;
        }
        return Tuning.CasualSpeed;
    }

    SwimStep SwimModel::Tick(double IntentX, double IntentY, const SwimPace RequestedPace, StaminaModel& Stamina, const float DeltaSeconds)
    {
        SwimStep Step;
        if (!Swimming || DeltaSeconds <= 0.0f)
        {
            Step.VelocityX = VelocityX;
            Step.VelocityY = VelocityY;
            Step.Exhausted = Exhausted;
            return Step;
        }
        if (!std::isfinite(IntentX) || !std::isfinite(IntentY))
        {
            IntentX = IntentY = 0.0;
        }
        const double IntentLength = std::hypot(IntentX, IntentY);
        if (IntentLength > 1.0)
        {
            IntentX /= IntentLength;
            IntentY /= IntentLength;
        }
        const bool Stroking = IntentLength > 1e-3;

        // Exhaustion hysteresis: strokes stay casual until stamina has partly recovered.
        if (Exhausted && Stamina.CurrentStamina >= Stamina.MaxStamina * Tuning.ExhaustionRecoveryFraction)
        {
            Exhausted = false;
        }
        const SwimPace Pace = Exhausted || !Stroking ? SwimPace::Casual : RequestedPace;
        Step.EffectivePace = Pace;

        // Stamina: pace cost plus the current fighting (or helping) the stroke.
        if (Stroking)
        {
            float Drain = 0.0f;
            if (Pace == SwimPace::Active) Drain = Tuning.ActiveDrainPerSecond;
            else if (Pace == SwimPace::Sprint) Drain = Tuning.SprintDrainPerSecond;
            const double DirectionX = IntentX / std::max(IntentLength, 1e-9);
            const double DirectionY = IntentY / std::max(IntentLength, 1e-9);
            const double Opposing = -(DirectionX * GetCurrentDriftX() + DirectionY * GetCurrentDriftY());
            Drain += Tuning.CurrentDrainPerMetrePerSecond * static_cast<float>(Opposing);
            const float Cost = std::max(0.0f, Drain) * DeltaSeconds;
            if (Cost > 0.0f)
            {
                // Taking the last of it empties the pool and arms the exhaustion regen delay.
                const float Taken = std::min(Cost, Stamina.CurrentStamina);
                Stamina.Consume(Taken);
                Step.StaminaDrained = Taken;
                if (Stamina.CurrentStamina <= 0.0f)
                {
                    Exhausted = true;
                }
            }
        }

        // Exact integration of dv/dt = a + drag * (v_stroke - v).
        const double Speed = SpeedForPace(Pace);
        const double StrokeX = IntentX * Speed;
        const double StrokeY = IntentY * Speed;
        const double Drag = Tuning.WaterDragPerSecond;
        if (Drag > 0.0)
        {
            const double TargetX = StrokeX + CurrentX / Drag;
            const double TargetY = StrokeY + CurrentY / Drag;
            const double Decay = std::exp(-Drag * static_cast<double>(DeltaSeconds));
            VelocityX = TargetX + (VelocityX - TargetX) * Decay;
            VelocityY = TargetY + (VelocityY - TargetY) * Decay;
        }
        else
        {
            VelocityX = StrokeX + CurrentX * DeltaSeconds;
            VelocityY = StrokeY + CurrentY * DeltaSeconds;
        }
        Step.VelocityX = VelocityX;
        Step.VelocityY = VelocityY;
        Step.Exhausted = Exhausted;
        return Step;
    }

    double FloatingBaseHeight(const double SurfaceHeight, const double EyeHeightAboveBase, const double HeadClearance)
    {
        return SurfaceHeight + HeadClearance - EyeHeightAboveBase;
    }
}
