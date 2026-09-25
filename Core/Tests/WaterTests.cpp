#include "TestHarness.h"

#include "DarkArisen/Core/Combat.h"
#include "DarkArisen/Core/Water.h"

#include <cmath>

using namespace DarkArisen::Core;

TEST_CASE("Breath: tiers match water_physics.md 6.3")
{
    CHECK_NEAR(BreathModel::MaximumSecondsForTier(BreathTier::Bare), 30.0f, 1e-6f);
    CHECK_NEAR(BreathModel::MaximumSecondsForTier(BreathTier::Trained), 60.0f, 1e-6f);
    CHECK_NEAR(BreathModel::MaximumSecondsForTier(BreathTier::Master), 90.0f, 1e-6f);
    BreathModel Breath;
    Breath.SetTier(BreathTier::Equipment);
    CHECK(Breath.HasIndefiniteBreath());
    Breath.SetHeadSubmerged(true);
    CHECK_NEAR(Breath.Tick(600.0f, false), 0.0f, 1e-6f);
    CHECK_NEAR(Breath.GetRemainingFraction(), 1.0f, 1e-6f);
}

TEST_CASE("Breath: only drains under water and refills on surfacing")
{
    BreathModel Breath;
    CHECK_NEAR(Breath.Tick(10.0f, false), 0.0f, 1e-6f);
    CHECK_NEAR(Breath.GetRemainingSeconds(), 30.0f, 1e-6f);
    Breath.SetHeadSubmerged(true);
    for (int Frame = 0; Frame < 20 * 60; ++Frame)
    {
        CHECK_NEAR(Breath.Tick(1.0f / 60.0f, false), 0.0f, 1e-6f);
    }
    CHECK_NEAR(Breath.GetRemainingSeconds(), 10.0f, 1e-3f);
    Breath.SetHeadSubmerged(false);
    CHECK_NEAR(Breath.GetRemainingSeconds(), 30.0f, 1e-6f);
}

TEST_CASE("Breath: drowning deals 20 per whole second, frame-rate independent")
{
    for (const float Step : {1.0f / 30.0f, 1.0f / 60.0f, 1.0f / 144.0f})
    {
        BreathModel Breath;
        Breath.SetHeadSubmerged(true);
        float Damage = 0.0f;
        const int Frames = static_cast<int>(std::lround(35.5f / Step));
        for (int Frame = 0; Frame < Frames; ++Frame)
        {
            Damage += Breath.Tick(Step, false);
        }
        // 30 s of breath, then 5.5 s drowning: 5 whole seconds = 100 damage, 0.5 s carried.
        CHECK_NEAR(Damage, 100.0f, 1e-3f);
        CHECK(Breath.IsDrowning());
    }
}

TEST_CASE("Breath: breath running out mid-tick only counts the overflow as drowning")
{
    // Unreal counted the whole transition tick as drowning time; the port counts the overflow.
    BreathModel Breath;
    Breath.SetHeadSubmerged(true);
    CHECK_NEAR(Breath.Tick(29.5f, false), 0.0f, 1e-6f);
    CHECK_NEAR(Breath.Tick(1.0f, false), 0.0f, 1e-6f);   // 0.5 s drowning so far
    CHECK_NEAR(Breath.Tick(0.5f, false), 20.0f, 1e-6f);
}

TEST_CASE("Breath: drowning damage is environmental and stops once dead")
{
    HealthModel Health;
    Health.MaxHealth = Health.CurrentHealth = 50.0f;
    Health.RallyEnabled = true;
    BreathModel Breath;
    Breath.SetHeadSubmerged(true);
    CHECK_NEAR(Breath.TickAndApply(31.0f, Health), 20.0f, 1e-4f);
    CHECK_NEAR(Health.GetRallyAvailable(), 20.0f * 0.40f, 1e-4f);  // Environmental rally fraction
    Breath.TickAndApply(2.0f, Health);
    CHECK(Health.IsDead());
    CHECK_NEAR(Breath.TickAndApply(5.0f, Health), 0.0f, 1e-6f);
}

TEST_CASE("Breath: respawn and tier change refill")
{
    BreathModel Breath;
    Breath.SetHeadSubmerged(true);
    Breath.Tick(40.0f, false);
    Breath.ResetForRespawn();
    CHECK(!Breath.IsHeadSubmerged());
    CHECK_NEAR(Breath.GetRemainingSeconds(), 30.0f, 1e-6f);
    Breath.SetHeadSubmerged(true);
    Breath.Tick(20.0f, false);
    Breath.SetTier(BreathTier::Master);
    CHECK_NEAR(Breath.GetRemainingSeconds(), 90.0f, 1e-6f);
}

namespace
{
    SwimStep SwimFor(SwimModel& Swim, StaminaModel& Stamina, const double IntentX, const double IntentY,
                     const SwimPace Pace, const float Seconds, const float Step = 1.0f / 60.0f)
    {
        SwimStep Last;
        const int Frames = static_cast<int>(std::lround(Seconds / Step));
        for (int Frame = 0; Frame < Frames; ++Frame)
        {
            Last = Swim.Tick(IntentX, IntentY, Pace, Stamina, Step);
            Stamina.Tick(Step);
        }
        return Last;
    }

    StaminaModel WaterStamina(const SwimModel& Swim)
    {
        StaminaModel Stamina;
        Stamina.RegenMultiplier = Swim.Tuning.InWaterRegenMultiplier;
        return Stamina;
    }
}

TEST_CASE("Swim: still water speeds are the design's 1 / 1.5 / 2 m/s")
{
    for (const SwimPace Pace : {SwimPace::Casual, SwimPace::Active, SwimPace::Sprint})
    {
        SwimModel Swim;
        StaminaModel Stamina = WaterStamina(Swim);
        Swim.Enter(0.0, 0.0);
        const SwimStep Step = SwimFor(Swim, Stamina, 0.0, 1.0, Pace, 6.0f);
        CHECK_NEAR(Step.VelocityY, static_cast<double>(Swim.SpeedForPace(Pace)), 0.01);
        CHECK_NEAR(Step.VelocityX, 0.0, 1e-9);
    }
}

TEST_CASE("Swim: integration is independent of tick length")
{
    SwimModel A;
    SwimModel B;
    StaminaModel StaminaA = WaterStamina(A);
    StaminaModel StaminaB = WaterStamina(B);
    A.Enter(3.0, 0.0);
    B.Enter(3.0, 0.0);
    A.SetCurrentAcceleration(1.3, 0.15);
    B.SetCurrentAcceleration(1.3, 0.15);
    const SwimStep Fine = SwimFor(A, StaminaA, 0.0, 1.0, SwimPace::Casual, 2.0f, 1.0f / 240.0f);
    const SwimStep Coarse = SwimFor(B, StaminaB, 0.0, 1.0, SwimPace::Casual, 2.0f, 1.0f / 20.0f);
    CHECK_NEAR(Fine.VelocityX, Coarse.VelocityX, 1e-6);
    CHECK_NEAR(Fine.VelocityY, Coarse.VelocityY, 1e-6);
}

TEST_CASE("Swim: Outer Reef current A pulls a casual swimmer backwards; a sprint beats it")
{
    // ContentSource Current.A: [130, 15, 0] cm/s^2.
    const double Ax = CurrentFromCentimetres(130.0);
    const double Ay = CurrentFromCentimetres(15.0);
    SwimModel Casual;
    StaminaModel CasualStamina = WaterStamina(Casual);
    Casual.Enter(0.0, 0.0);
    Casual.SetCurrentAcceleration(Ax, Ay);
    const SwimStep Against = SwimFor(Casual, CasualStamina, -1.0, 0.0, SwimPace::Casual, 8.0f);
    CHECK(Against.VelocityX > 0.0);  // net drift with the current: "sometimes impossible"

    SwimModel Sprint;
    StaminaModel SprintStamina = WaterStamina(Sprint);
    Sprint.Enter(0.0, 0.0);
    Sprint.SetCurrentAcceleration(Ax, Ay);
    const SwimStep Beating = SwimFor(Sprint, SprintStamina, -1.0, 0.0, SwimPace::Sprint, 3.0f);
    CHECK(Beating.VelocityX < 0.0);
    CHECK(SprintStamina.CurrentStamina < CasualStamina.CurrentStamina);
}

TEST_CASE("Swim: sprinting drains faster than land sprint and exhausts to casual")
{
    SwimModel Swim;
    StaminaModel Stamina = WaterStamina(Swim);
    CHECK(Swim.Tuning.SprintDrainPerSecond > Stamina.SprintDrainPerSecond);
    Swim.Enter(0.0, 0.0);
    // 120 stamina at 15/s: dry after 8 s, then only casual strokes.
    const SwimStep Step = SwimFor(Swim, Stamina, 0.0, 1.0, SwimPace::Sprint, 9.0f);
    CHECK(Step.Exhausted);
    CHECK(Step.EffectivePace == SwimPace::Casual);
    const SwimStep Settled = SwimFor(Swim, Stamina, 0.0, 1.0, SwimPace::Sprint, 2.5f);
    CHECK_NEAR(Settled.VelocityY, 1.0, 0.1);
    // Casual strokes in still water are sustainable: after the 3 s exhaustion delay stamina
    // comes back at a quarter of the land rate.
    const float Before = Stamina.CurrentStamina;
    SwimFor(Swim, Stamina, 0.0, 1.0, SwimPace::Casual, 1.0f);
    CHECK_NEAR(Stamina.CurrentStamina - Before, Stamina.BaseRegenRate * Swim.Tuning.InWaterRegenMultiplier, 0.2f);
    // Sprint returns once a quarter of the pool is back.
    SwimFor(Swim, Stamina, 0.0, 1.0, SwimPace::Casual, 6.0f);
    CHECK(Stamina.CurrentStamina >= Stamina.MaxStamina * Swim.Tuning.ExhaustionRecoveryFraction);
    const SwimStep Recovered = Swim.Tick(0.0, 1.0, SwimPace::Sprint, Stamina, 1.0f / 60.0f);
    CHECK(Recovered.EffectivePace == SwimPace::Sprint);
}

TEST_CASE("Swim: a helping current reduces drain, an opposing current raises it")
{
    SwimModel With;
    SwimModel Against;
    StaminaModel WithStamina = WaterStamina(With);
    StaminaModel AgainstStamina = WaterStamina(Against);
    With.Enter(0.0, 0.0);
    Against.Enter(0.0, 0.0);
    With.SetCurrentAcceleration(0.5, 0.0);
    Against.SetCurrentAcceleration(0.5, 0.0);
    SwimFor(With, WithStamina, 1.0, 0.0, SwimPace::Active, 1.0f);
    SwimFor(Against, AgainstStamina, -1.0, 0.0, SwimPace::Active, 1.0f);
    CHECK_NEAR(WithStamina.MaxStamina - WithStamina.CurrentStamina, 3.0f - 4.0f * 0.5f, 0.05f);
    CHECK_NEAR(AgainstStamina.MaxStamina - AgainstStamina.CurrentStamina, 3.0f + 4.0f * 0.5f, 0.05f);
}

TEST_CASE("Swim: non-finite input is rejected, exit clears state")
{
    SwimModel Swim;
    StaminaModel Stamina = WaterStamina(Swim);
    Swim.Enter(std::nan(""), 1.0);
    CHECK_NEAR(Swim.GetVelocityX(), 0.0, 1e-12);
    Swim.SetCurrentAcceleration(std::nan(""), 0.0);
    const SwimStep Step = Swim.Tick(std::nan(""), 0.0, SwimPace::Sprint, Stamina, 0.1f);
    CHECK(std::isfinite(Step.VelocityX) && std::isfinite(Step.VelocityY));
    CHECK(Step.EffectivePace == SwimPace::Casual);
    Swim.Exit();
    CHECK(!Swim.IsSwimming());
    CHECK_NEAR(Swim.GetCurrentDriftX(), 0.0, 1e-12);
    const SwimStep Dry = Swim.Tick(1.0, 0.0, SwimPace::Sprint, Stamina, 0.1f);
    CHECK_NEAR(Dry.VelocityX, 0.0, 1e-12);
}

TEST_CASE("Swim: floating height keeps the eyes above the surface")
{
    CHECK_NEAR(FloatingBaseHeight(0.4, 1.62, 0.12), 0.4 + 0.12 - 1.62, 1e-12);
}
