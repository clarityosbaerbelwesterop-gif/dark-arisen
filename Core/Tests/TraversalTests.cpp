#include "TestHarness.h"

#include "DarkArisen/Core/Combat.h"
#include "DarkArisen/Core/Traversal.h"

using namespace DarkArisen::Core;

namespace
{
    /** Runs the model for Seconds at 60 Hz, the pool ticked by its owner after it, as the combatant does. */
    void Run(TraversalModel& Model, StaminaModel& Stamina, const TraversalInput& In, const float Seconds)
    {
        for (int Frame = 0; Frame < static_cast<int>(Seconds * 60.0f + 0.5f); ++Frame)
        {
            Model.Tick(1.0f / 60.0f, In, Stamina);
            Stamina.Tick(1.0f / 60.0f);
        }
    }
}

TEST_CASE("Traversal: gait speeds and ramps match traversal.md 2.1, 3.1 and 4.1")
{
    CHECK_NEAR(SpecFor(Gait::Walk).Speed, 3.5f, 1e-6f);
    CHECK_NEAR(SpecFor(Gait::Run).Speed, 6.5f, 1e-6f);
    CHECK_NEAR(SpecFor(Gait::Sprint).Speed, 10.0f, 1e-6f);
    CHECK_NEAR(SpecFor(Gait::Sprint).StaminaPerSecond, 10.0f, 1e-6f);

    // Arrange: standing; Act: run for exactly the acceleration time; Assert: at full running speed, not before
    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Wanted = Gait::Run;
    Run(Model, Stamina, In, 0.25f);
    CHECK(Model.GetSpeed() > 2.0f && Model.GetSpeed() < 6.5f);
    Run(Model, Stamina, In, 0.25f);
    CHECK_NEAR(Model.GetSpeed(), 6.5f, 1e-3f);
    In.Wanted = Gait::Idle;
    Run(Model, Stamina, In, 0.4f);
    CHECK_NEAR(Model.GetSpeed(), 0.0f, 1e-3f);
}

TEST_CASE("Traversal: surfaces slow walking and sprinting by their own tables")
{
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Mud, Gait::Walk), 0.70f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Mud, Gait::Sprint), 0.50f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Sand, Gait::Run), 0.85f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Sand, Gait::Sprint), 0.75f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Ice, Gait::Walk), 0.60f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::WetDeck, Gait::Sprint), 0.85f, 1e-6f);
    CHECK_NEAR(SurfaceSpeedFactor(Surface::Stone, Gait::Sprint), 1.0f, 1e-6f);
    CHECK_NEAR(WeatherSpeedFactor(Weather::Rain, Surface::Stone), 0.85f, 1e-6f);
    CHECK_NEAR(WeatherSpeedFactor(Weather::Rain, Surface::Grass), 1.0f, 1e-6f);
    CHECK_NEAR(WeatherSpeedFactor(Weather::Storm, Surface::Grass), 0.75f, 1e-6f);

    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Wanted = Gait::Walk;
    In.Ground = Surface::Mud;
    Run(Model, Stamina, In, 1.0f);
    CHECK_NEAR(Model.GetSpeed(), 3.5f * 0.7f, 1e-3f);
}

TEST_CASE("Traversal: sprinting to empty forces a three-second walk and weaker attacks (4.3)")
{
    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Wanted = Gait::Sprint;
    // 120 stamina at 10/s: twelve seconds of sprint
    Run(Model, Stamina, In, 11.9f);
    CHECK(Model.GetGait() == Gait::Sprint);
    CHECK(!Model.IsExhausted());
    Run(Model, Stamina, In, 0.2f);
    CHECK(Model.IsExhausted());
    CHECK(Model.GetGait() == Gait::Walk);
    CHECK_NEAR(Model.AttackDamageMultiplier(), 0.75f, 1e-6f);
    Run(Model, Stamina, In, 1.0f);
    CHECK_NEAR(Model.GetSpeed(), 3.5f, 1e-3f);
    // the pool's exhaustion delay holds regeneration for the same three seconds, so sprint cannot resume at once
    Run(Model, Stamina, In, 2.0f);
    CHECK(Model.GetGait() == Gait::Walk);
}

TEST_CASE("Traversal: a sprint lasts at least half a second (4.1)")
{
    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Wanted = Gait::Sprint;
    Run(Model, Stamina, In, 0.1f);
    In.Wanted = Gait::Walk;
    Run(Model, Stamina, In, 0.2f);
    CHECK(Model.GetGait() == Gait::Sprint);
    Run(Model, Stamina, In, 0.3f);
    CHECK(Model.GetGait() == Gait::Walk);
}

TEST_CASE("Traversal: obstacles while running follow 3.4")
{
    CHECK(RespondToObstacle(0.3f, true, false) == ObstacleResponse::StepOver);
    CHECK(RespondToObstacle(0.7f, true, false) == ObstacleResponse::AutoVault);
    CHECK(RespondToObstacle(1.0f, true, true) == ObstacleResponse::VaultOnJump);
    CHECK(RespondToObstacle(1.0f, true, false) == ObstacleResponse::Climb);
    CHECK(RespondToObstacle(1.5f, true, true) == ObstacleResponse::JumpAndGrab);
    CHECK(RespondToObstacle(1.8f, true, true) == ObstacleResponse::Climb);
}

TEST_CASE("Traversal: jumps and gaps follow 5.1 and 5.5")
{
    CHECK_NEAR(JumpFor(Gait::Idle).Distance, 2.5f, 1e-6f);
    CHECK_NEAR(JumpFor(Gait::Run).Distance, 4.0f, 1e-6f);
    CHECK_NEAR(JumpFor(Gait::Sprint).Distance, 5.5f, 1e-6f);
    CHECK_NEAR(JumpFor(Gait::Sprint).Height, 2.0f, 1e-6f);
    CHECK(JumpFor(Gait::Sprint).RecoveryFrames == 8);
    CHECK(JumpFor(Gait::Walk).RecoveryFrames == 12);
    CHECK(ReadGap(2.0f) == GapVerdict::Safe);
    CHECK(ReadGap(3.5f) == GapVerdict::Dangerous);
    CHECK(ReadGap(6.0f) == GapVerdict::Impossible);
    CHECK(JumpClears(3.5f, Gait::Run));
    CHECK(!JumpClears(3.5f, Gait::Idle));
    CHECK(JumpClears(5.4f, Gait::Sprint));
    CHECK(!JumpClears(5.6f, Gait::Sprint));
}

TEST_CASE("Traversal: climbing drains the pool for 24 seconds and the grip fails at zero (6.3, 6.5)")
{
    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Climbing = true;
    Run(Model, Stamina, In, 20.0f);
    CHECK(GripFor(Stamina.CurrentStamina, Stamina.MaxStamina) == Grip::Straining);
    Run(Model, Stamina, In, 2.5f);
    CHECK(GripFor(Stamina.CurrentStamina, Stamina.MaxStamina) == Grip::Slipping);
    Run(Model, Stamina, In, 1.6f);
    CHECK(GripFor(Stamina.CurrentStamina, Stamina.MaxStamina) == Grip::Lost);
    CHECK(GripFor(120.0f, 120.0f) == Grip::Firm);
}

TEST_CASE("Traversal: weather changes climbing (6.6)")
{
    CHECK(!ClimbFor(Weather::Hurricane).Possible);
    CHECK(!ClimbFor(Weather::Clear, true).Possible);
    CHECK_NEAR(ClimbFor(Weather::Rain).UpSpeed, 1.6f, 1e-5f);
    CHECK_NEAR(ClimbFor(Weather::Rain).StaminaPerSecond, 7.5f, 1e-5f);
    CHECK_NEAR(ClimbFor(Weather::Storm).StaminaPerSecond, 10.0f, 1e-5f);
    CHECK_NEAR(ClimbFor(Weather::Snow).UpSpeed, 1.8f, 1e-5f);
}

TEST_CASE("Traversal: falls follow 7.1, a lost grip 6.5, and the mitigations 7.2")
{
    CHECK_NEAR(ResolveFall(3.0f, FallCause::Fell, Landing::Ground, false).HealthFraction, 0.0f, 1e-6f);
    CHECK_NEAR(ResolveFall(5.0f, FallCause::Fell, Landing::Ground, false).HealthFraction, 0.15f, 1e-6f);
    CHECK_NEAR(ResolveFall(8.0f, FallCause::Fell, Landing::Ground, false).HealthFraction, 0.40f, 1e-6f);
    CHECK(ResolveFall(12.0f, FallCause::Fell, Landing::Ground, false).MayKnockOut);
    CHECK_NEAR(ResolveFall(20.0f, FallCause::Fell, Landing::Ground, false).HealthFraction, 0.95f, 1e-6f);
    CHECK(ResolveFall(25.0f, FallCause::Fell, Landing::Ground, false).Fatal);
    CHECK(ResolveFall(30.0f, FallCause::Fell, Landing::HayNetOrSail, true).Fatal);
    // a lost grip uses 6.5's own table
    CHECK_NEAR(ResolveFall(4.0f, FallCause::GripLost, Landing::Ground, false).HealthFraction, 0.0f, 1e-6f);
    CHECK_NEAR(ResolveFall(7.0f, FallCause::GripLost, Landing::Ground, false).HealthFraction, 0.30f, 1e-6f);
    CHECK(ResolveFall(22.0f, FallCause::GripLost, Landing::Ground, false).Fatal);
    // deep water replaces the table
    CHECK_NEAR(ResolveFall(14.0f, FallCause::Fell, Landing::DeepWater, false).HealthFraction, 0.0f, 1e-6f);
    CHECK_NEAR(ResolveFall(20.0f, FallCause::Fell, Landing::DeepWater, false).HealthFraction, 0.20f, 1e-6f);
    CHECK_NEAR(ResolveFall(40.0f, FallCause::Fell, Landing::DeepWater, false).HealthFraction, 0.60f, 1e-6f);
    CHECK(!ResolveFall(40.0f, FallCause::Fell, Landing::DeepWater, false).Fatal);
    // a timed roll halves and costs 25 stamina; soft landings stack with it
    const FallResult Rolled = ResolveFall(8.0f, FallCause::Fell, Landing::Ground, true);
    CHECK_NEAR(Rolled.HealthFraction, 0.20f, 1e-6f);
    CHECK_NEAR(Rolled.StaminaCost, 25.0f, 1e-6f);
    CHECK_NEAR(ResolveFall(8.0f, FallCause::Fell, Landing::HayNetOrSail, false).HealthFraction, 0.10f, 1e-6f);
    CHECK_NEAR(ResolveFall(8.0f, FallCause::Fell, Landing::Enemy, false).HealthFraction, 0.16f, 1e-6f);
    CHECK(RollTimed(0.15f));
    CHECK(!RollTimed(0.3f));
}

TEST_CASE("Traversal: swimming drains 8 per second, 15 sprinting (8.3)")
{
    TraversalModel Model;
    StaminaModel Stamina;
    TraversalInput In;
    In.Swimming = true;
    Run(Model, Stamina, In, 10.0f);
    CHECK_NEAR(Stamina.CurrentStamina, 40.0f, 0.5f);
    In.Wanted = Gait::Sprint;
    Run(Model, Stamina, In, 2.0f);
    CHECK_NEAR(Stamina.CurrentStamina, 10.0f, 0.5f);
    CHECK_NEAR(Swimming().MaximumDepth, 30.0f, 1e-6f);
}

TEST_CASE("Traversal: regeneration per state follows 11.2 and heat without water costs 15% (12.5)")
{
    TraversalInput In;
    CHECK_NEAR(StaminaRegenFor(In), 25.0f, 1e-6f);
    In.Wanted = Gait::Walk;
    CHECK_NEAR(StaminaRegenFor(In), 20.0f, 1e-6f);
    In.Wanted = Gait::Run;
    CHECK_NEAR(StaminaRegenFor(In), 15.0f, 1e-6f);
    In.InCombat = true;
    In.Attacking = true;
    CHECK_NEAR(StaminaRegenFor(In), 10.0f, 1e-6f);
    In.InCombat = false;
    In.Wanted = Gait::Idle;
    In.Sky = Weather::Heat;
    In.CarriesWater = false;
    CHECK_NEAR(StaminaRegenFor(In), 25.0f * 0.85f, 1e-5f);

    // Arrange: drained pool, standing holstered; Act: wait past the one-second delay; Assert: 25 per second
    TraversalModel Model;
    StaminaModel Stamina;
    Stamina.Consume(60.0f);
    TraversalInput Rest;
    Run(Model, Stamina, Rest, 2.0f);
    CHECK_NEAR(Stamina.CurrentStamina, 60.0f + 25.0f, 0.6f);
}

TEST_CASE("Traversal: horses only on the Quiet Coast (10.5)")
{
    CHECK(HorsesIn(Region::QuietCoast));
    CHECK(!HorsesIn(Region::Highmoore));
    CHECK(!HorsesIn(Region::Rexa));
    CHECK_NEAR(HorseGallopSpeed, 14.0f, 1e-6f);
}
