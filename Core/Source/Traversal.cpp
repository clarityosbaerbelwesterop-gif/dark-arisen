#include "DarkArisen/Core/Traversal.h"

#include "DarkArisen/Core/Combat.h"

#include <algorithm>

namespace DarkArisen::Core
{
    GaitSpec SpecFor(const Gait G)
    {
        switch (G)
        {
        case Gait::Walk: return {3.5f, 0.3f, 0.2f, 0.0f};
        case Gait::Run: return {6.5f, 0.5f, 0.4f, 0.0f};
        case Gait::Sprint: return {10.0f, 0.8f, 0.6f, 10.0f};
        case Gait::Idle: default: return {0.0f, 0.3f, 0.2f, 0.0f};
        }
    }

    float SurfaceSpeedFactor(const Surface Ground, const Gait G)
    {
        if (G == Gait::Sprint)
        {
            // 4.5; surfaces the sprint table leaves out keep their walking factor (2.3)
            switch (Ground)
            {
            case Surface::Grass: return 0.95f;
            case Surface::DeepGrass: return 0.80f;
            case Surface::Sand: return 0.75f;
            case Surface::Mud: return 0.50f;
            case Surface::ShallowWater: return 0.60f;
            case Surface::WetDeck: return 0.85f;
            default: break;
            }
        }
        switch (Ground)
        {
        case Surface::Sand: return 0.85f;
        case Surface::Mud: return 0.70f;
        case Surface::ShallowWater: return 0.75f;
        case Surface::Snow: return 0.75f;
        case Surface::DeepSnow: return 0.70f;
        case Surface::Ice: return 0.60f;
        default: return 1.0f;
        }
    }

    float WeatherSpeedFactor(const Weather Sky, const Surface Ground)
    {
        switch (Sky)
        {
        case Weather::Rain:
            return Ground == Surface::Stone || Ground == Surface::Wood || Ground == Surface::Metal || Ground == Surface::WetDeck ? 0.85f : 1.0f;
        case Weather::Storm:
        case Weather::Hurricane:
            return 0.75f;
        default:
            return 1.0f;
        }
    }

    ObstacleResponse RespondToObstacle(const float HeightMetres, const bool Running, const bool JumpPressed)
    {
        if (HeightMetres <= 0.35f) return ObstacleResponse::StepOver;
        if (Running && HeightMetres < 0.8f) return ObstacleResponse::AutoVault;
        if (HeightMetres <= 1.2f) return JumpPressed ? ObstacleResponse::VaultOnJump : ObstacleResponse::Climb;
        if (Running && HeightMetres <= 1.6f && JumpPressed) return ObstacleResponse::JumpAndGrab;
        return ObstacleResponse::Climb;
    }

    JumpSpec JumpFor(const Gait G)
    {
        switch (G)
        {
        case Gait::Run: return {1.2f, 4.0f, 12.0f, 6, 12};
        case Gait::Sprint: return {2.0f, 5.5f, 12.0f, 6, 8};
        default: return {1.2f, 2.5f, 12.0f, 6, 12};
        }
    }

    GapVerdict ReadGap(const float DistanceMetres)
    {
        if (DistanceMetres < 2.5f) return GapVerdict::Safe;
        if (DistanceMetres <= 5.5f) return GapVerdict::Dangerous;
        return GapVerdict::Impossible;
    }

    bool JumpClears(const float DistanceMetres, const Gait G)
    {
        return ReadGap(DistanceMetres) != GapVerdict::Impossible && DistanceMetres <= JumpFor(G).Distance;
    }

    ClimbSpec ClimbFor(const Weather Sky, const bool IceCovered)
    {
        ClimbSpec Spec;
        float Speed = 1.0f, Cost = 1.0f;
        switch (Sky)
        {
        case Weather::Rain: Speed = 0.8f; Cost = 1.5f; break;
        case Weather::Storm: Speed = 0.6f; Cost = 2.0f; break;
        case Weather::Snow: Speed = 0.9f; Cost = 1.2f; break;
        case Weather::Hurricane: Spec.Possible = false; break;
        default: break;
        }
        if (IceCovered) Spec.Possible = false;
        Spec.UpSpeed *= Speed;
        Spec.SidewaysSpeed *= Speed;
        Spec.StaminaPerSecond *= Cost;
        return Spec;
    }

    Grip GripFor(const float Stamina, const float MaxStamina)
    {
        if (Stamina <= 0.0f) return Grip::Lost;
        const float Fraction = MaxStamina > 0.0f ? Stamina / MaxStamina : 0.0f;
        if (Fraction <= 0.10f) return Grip::Slipping;
        if (Fraction <= 0.25f) return Grip::Straining;
        return Grip::Firm;
    }

    FallResult ResolveFall(const float HeightMetres, const FallCause Cause, const Landing Onto, const bool TimedRoll)
    {
        FallResult Result;
        const float H = std::max(0.0f, HeightMetres);
        if (Onto == Landing::DeepWater)
        {
            Result.HealthFraction = H < 15.0f ? 0.0f : H < 30.0f ? 0.20f : 0.60f;
            return Result;
        }
        float Base = 0.0f;
        bool Lethal = false;
        if (Cause == FallCause::GripLost)
        {
            // 6.5: a lost grip; 20 m and more is fatal unless deep water is below
            Base = H < 5.0f ? 0.0f : H < 10.0f ? 0.30f : H < 20.0f ? 0.60f : 1.0f;
            Lethal = H >= 20.0f;
        }
        else
        {
            Base = H <= 3.0f ? 0.0f : H <= 6.0f ? 0.15f : H <= 10.0f ? 0.40f : H <= 15.0f ? 0.75f : H < 25.0f ? 0.95f : 1.0f;
            Lethal = H >= 25.0f;
            Result.MayKnockOut = H > 10.0f && H <= 15.0f;
        }
        if (Lethal)
        {
            Result.HealthFraction = 1.0f;
            Result.Fatal = true;
            return Result;
        }
        float Scale = 1.0f;
        switch (Onto)
        {
        case Landing::HayNetOrSail: Scale = 0.25f; break;
        case Landing::GrassOrLeaves: Scale = 0.80f; break;
        case Landing::MudOrSand: Scale = 0.85f; break;
        case Landing::Snow: Scale = 0.70f; break;
        case Landing::Enemy: Scale = 0.40f; break;
        default: break;
        }
        if (TimedRoll)
        {
            Scale *= 0.5f;
            Result.StaminaCost = 25.0f;
        }
        Result.HealthFraction = Base * Scale;
        Result.Fatal = Result.HealthFraction >= 1.0f;
        return Result;
    }

    bool RollTimed(const float SecondsBeforeContact)
    {
        return SecondsBeforeContact >= 0.0f && SecondsBeforeContact <= 12.0f / 60.0f;
    }

    SwimSpec Swimming() { return {}; }

    bool HorsesIn(const Region Where) { return Where == Region::QuietCoast; }

    float StaminaRegenFor(const TraversalInput& In)
    {
        if (In.Climbing || In.Swimming || In.Wanted == Gait::Sprint) return 0.0f;
        float Rate = 0.0f;
        if (In.InCombat) Rate = In.Attacking ? 10.0f : 15.0f;
        else
        {
            switch (In.Wanted)
            {
            case Gait::Idle: Rate = In.WeaponHolstered ? 25.0f : 20.0f; break;
            case Gait::Walk: Rate = 20.0f; break;
            default: Rate = 15.0f; break;
            }
        }
        if (In.Sky == Weather::Heat && !In.CarriesWater) Rate *= 0.85f;
        return Rate;
    }

    void TraversalModel::Tick(const float DeltaSeconds, const TraversalInput& In, StaminaModel& Stamina)
    {
        const float Dt = std::max(0.0f, DeltaSeconds);
        Stamina.SetSprinting(false);  // this model owns the drain
        ExhaustedFor = std::max(0.0f, ExhaustedFor - Dt);

        if (In.Climbing || In.Swimming)
        {
            Current = Gait::Idle;
            Speed = 0.0f;
            SprintedFor = 0.0f;
        }
        else
        {
            Gait Want = In.Wanted;
            if (IsExhausted() && Want != Gait::Idle) Want = Gait::Walk;  // 4.3: forced to walking speed
            if (Current == Gait::Sprint && SprintedFor < MinimumSprintSeconds && !IsExhausted()) Want = Gait::Sprint;  // 4.1
            if (Want == Gait::Sprint && Stamina.CurrentStamina <= 0.0f) Want = Gait::Walk;
            const float Target = SpecFor(Want).Speed * SurfaceSpeedFactor(In.Ground, Want) * WeatherSpeedFactor(In.Sky, In.Ground);
            if (Target > Speed)
            {
                const GaitSpec Up = SpecFor(Want);
                Speed = std::min(Target, Speed + Up.Speed / std::max(Up.AccelerationSeconds, 1e-3f) * Dt);
            }
            else
            {
                // slow down at the rate of the gait the current speed belongs to, not the one being slowed to
                const GaitSpec Down = SpecFor(Speed > SpecFor(Gait::Run).Speed ? Gait::Sprint : Speed > SpecFor(Gait::Walk).Speed ? Gait::Run : Gait::Walk);
                Speed = std::max(Target, Speed - Down.Speed / std::max(Down.DecelerationSeconds, 1e-3f) * Dt);
            }
            Current = Want;
            SprintedFor = Current == Gait::Sprint ? SprintedFor + Dt : 0.0f;
        }

        float Drain = 0.0f;
        if (In.Climbing) Drain = ClimbFor(In.Sky).StaminaPerSecond;
        else if (In.Swimming) Drain = In.Wanted == Gait::Sprint ? Swimming().SprintStaminaPerSecond : Swimming().StaminaPerSecond;
        else if (Current == Gait::Sprint) Drain = SpecFor(Gait::Sprint).StaminaPerSecond;

        if (Drain > 0.0f)
        {
            const float Amount = std::min(Drain * Dt, Stamina.CurrentStamina);
            if (Amount > 0.0f) Stamina.Consume(Amount);
            if (Stamina.CurrentStamina <= 0.0f && Current == Gait::Sprint)
            {
                ExhaustedFor = ExhaustionSeconds;
                Current = Gait::Walk;
                SprintedFor = 0.0f;
            }
            return;
        }
        TraversalInput Now = In;
        Now.Wanted = Current;
        Stamina.BaseRegenRate = StaminaRegenFor(Now);
        Stamina.Tick(Dt);
    }
}
