#include "TestHarness.h"

#include "DarkArisen/Core/Combat.h"

using namespace DarkArisen::Core;

namespace
{
    constexpr float Frame = 1.0f / 60.0f;

    Combatant Make(const char* Id)
    {
        Combatant Result;
        Result.EntityId = Id;
        return Result;
    }

    void TickFrames(Combatant& Actor, const int Frames)
    {
        for (int Index = 0; Index < Frames; ++Index)
        {
            Actor.Combat.Tick(Frame, Frame);
            Actor.Stamina.Tick(Frame);
            Actor.Health.Tick(Frame);
        }
    }
}

TEST_CASE("Combat: deflection window is exactly six frames at 60 fps")
{
    CHECK_NEAR(DesignLaws::DeflectionWindowSeconds, 0.1, 1e-6);
    Combatant Jake = Make("character.jake");
    CHECK(Jake.Combat.PerformParry(Jake.Stamina));
    CHECK(Jake.Combat.IsDeflectionWindowOpen());
    TickFrames(Jake, 5);
    CHECK(Jake.Combat.IsDeflectionWindowOpen());
    TickFrames(Jake, 1);
    CHECK(!Jake.Combat.IsDeflectionWindowOpen());
    CHECK(Jake.Combat.GetState() == CombatState::Idle);
}

TEST_CASE("Combat: a deflected blow deals no health damage and punishes the attacker's posture")
{
    Combatant Jake = Make("character.jake");
    Combatant Boarder = Make("enemy.boarder.harlow_deck");
    CHECK(Jake.Combat.PerformParry(Jake.Stamina));
    const DamageResult Result = ResolveMeleeHit(Boarder, Jake, HitKind::Light);
    CHECK(Result.Resolved);
    CHECK(Result.Deflected);
    CHECK_NEAR(Jake.Health.CurrentHealth, 200.0, 1e-6);
    // Unreal parity: 1.5 x 18 (pipeline) + 32 (deflected profile) = 59.
    CHECK_NEAR(Boarder.Combat.CurrentPosture, 59.0, 1e-4);
    CHECK(Boarder.Combat.GetPostureVisualState() == PostureVisualState::Failing);
}

TEST_CASE("Combat: criticals ignore deflection")
{
    Combatant Jake = Make("character.jake");
    Combatant Draven = Make("boss.draven_voss");
    CHECK(Jake.Combat.PerformParry(Jake.Stamina));
    const DamageResult Result = ResolveMeleeHit(Draven, Jake, HitKind::Critical);
    CHECK(!Result.Deflected);
    CHECK_NEAR(Jake.Health.CurrentHealth, 128.0, 1e-4);
    CHECK(Result.Staggered);
}

TEST_CASE("Combat: posture break staggers for 1.4 s, then resets to half")
{
    Combatant Boarder = Make("enemy.boarder");
    Boarder.Combat.AddPostureDamage(95.0f);
    CHECK(Boarder.Combat.GetState() == CombatState::Staggered);
    CHECK(Boarder.Combat.IsActionCommitted());
    Boarder.Combat.AddPostureDamage(50.0f); // ignored while staggered
    CHECK_NEAR(Boarder.Combat.CurrentPosture, 95.0, 1e-4);
    TickFrames(Boarder, 83);
    CHECK(Boarder.Combat.GetState() == CombatState::Staggered);
    TickFrames(Boarder, 2);
    CHECK(Boarder.Combat.GetState() == CombatState::Idle);
    CHECK_NEAR(Boarder.Combat.CurrentPosture, 50.0, 1e-4);
}

TEST_CASE("Combat: posture regenerates only after the three-second delay")
{
    Combatant Jake = Make("character.jake");
    Jake.Combat.AddPostureDamage(40.0f);
    TickFrames(Jake, 170);
    CHECK_NEAR(Jake.Combat.CurrentPosture, 40.0, 1e-4);
    TickFrames(Jake, 70);
    CHECK(Jake.Combat.CurrentPosture < 40.0f);
}

TEST_CASE("Combat: actions are committed, cost stamina and cannot be cancelled")
{
    Combatant Jake = Make("character.jake");
    CHECK(Jake.Combat.PerformLightAttack(Jake.Stamina));
    CHECK_NEAR(Jake.Stamina.CurrentStamina, 105.0, 1e-4);
    CHECK(!Jake.Combat.PerformParry(Jake.Stamina));
    CHECK(!Jake.Combat.PerformDodge(Jake.Stamina, false));
    Jake.Combat.FinishAction();
    CHECK(Jake.Combat.GetState() == CombatState::LightAttacking);
    // Medium weapon: 13 startup + 20 recovery frames.
    CHECK_NEAR(Jake.Combat.GetMinimumCommitmentSeconds(), 33.0 / 60.0, 1e-6);
    TickFrames(Jake, 34);
    CHECK(Jake.Combat.GetState() == CombatState::Idle);
}

TEST_CASE("Combat: the melee hit frame arrives after the weapon's startup frames")
{
    Combatant Jake = Make("character.jake");
    Jake.Combat.SetWeightClass(WeaponWeightClass::Heavy);
    CHECK(Jake.Combat.PerformHeavyAttack(Jake.Stamina));
    std::optional<HitKind> Due;
    int Frames = 0;
    while (!Due && Frames < 100)
    {
        Due = Jake.Combat.Tick(Frame, Frame).MeleeHitDue;
        ++Frames;
    }
    CHECK(Due.has_value());
    CHECK(Due && *Due == HitKind::Heavy);
    CHECK_EQ(Frames, 19);
}

TEST_CASE("Combat: an animation notify consumes the queued hit exactly once")
{
    Combatant Jake = Make("character.jake");
    CHECK(Jake.Combat.PerformLightAttack(Jake.Stamina));
    Jake.Combat.RouteQueuedHitToAnimation();
    TickFrames(Jake, 20);
    CHECK(!Jake.Combat.ConsumeQueuedHit(HitKind::Heavy).has_value());
    CHECK(Jake.Combat.ConsumeQueuedHit(HitKind::Light).has_value());
    CHECK(!Jake.Combat.ConsumeQueuedHit(HitKind::Light).has_value());
}

TEST_CASE("Combat: exhaustion blocks actions and delays regeneration")
{
    Combatant Jake = Make("character.jake");
    Jake.Stamina.CurrentStamina = 20.0f;
    CHECK(!Jake.Combat.PerformHeavyAttack(Jake.Stamina));
    CHECK(Jake.Stamina.Consume(20.0f));
    TickFrames(Jake, 170);
    CHECK_NEAR(Jake.Stamina.CurrentStamina, 0.0, 1e-4);
    TickFrames(Jake, 60);
    CHECK(Jake.Stamina.CurrentStamina > 0.0f);
}

TEST_CASE("Combat: dodge grants eight frames of invulnerability")
{
    Combatant Jake = Make("character.jake");
    Combatant Boarder = Make("enemy.boarder");
    CHECK(Jake.Combat.PerformDodge(Jake.Stamina, false));
    CHECK_NEAR(Jake.Stamina.CurrentStamina, 95.0, 1e-4);
    TickFrames(Jake, 7);
    CHECK(ResolveMeleeHit(Boarder, Jake, HitKind::Heavy).Invulnerable);
    TickFrames(Jake, 1);
    CHECK(!Jake.Combat.IsInvulnerable());
}

TEST_CASE("Combat: armor and resistance scale health damage; kills are final")
{
    Combatant Jake = Make("character.jake");
    Combatant Guard = Make("enemy.armada.guard");
    Guard.Combat.ArmorFraction = 0.5f;
    Guard.Combat.PhysicalResistance = 0.2f;
    const DamageResult Result = ResolveMeleeHit(Jake, Guard, HitKind::Heavy);
    CHECK_NEAR(Result.HealthDamage, 42.0 * 0.5 * 0.8, 1e-4);
    Guard.Health.CurrentHealth = 1.0f;
    CHECK(ResolveMeleeHit(Jake, Guard, HitKind::Light).Killed);
    CHECK(Guard.Combat.GetState() == CombatState::Dead);
    CHECK(!ResolveMeleeHit(Jake, Guard, HitKind::Light).Resolved);
}

TEST_CASE("Combat: rally returns a share of recent damage to aggressive play")
{
    Combatant Jake = Make("character.jake");
    Combatant Boarder = Make("enemy.boarder");
    Jake.Health.RallyEnabled = true;
    Jake.Health.ApplyDamage(100.0f, RallyDamageClass::StandardEnemy);
    CHECK_NEAR(Jake.Health.GetRallyAvailable(), 70.0, 1e-4);
    ResolveMeleeHit(Jake, Boarder, HitKind::Light);
    CHECK_NEAR(Jake.Health.CurrentHealth, 100.0 + 70.0 * 0.15, 1e-3);
    TickFrames(Jake, 185);
    CHECK_NEAR(Jake.Health.GetRallyAvailable(), 0.0, 1e-6);
}

TEST_CASE("Combat: Rache is 30 percent time scale for at most five real seconds")
{
    Combatant Jake = Make("character.jake");
    Jake.Combat.AddRacheFuel(100.0f);
    CHECK(!Jake.Combat.StartRache());
    Jake.Combat.RacheUnlocked = true;
    CHECK(Jake.Combat.StartRache());
    CHECK_NEAR(Jake.Combat.GetTimeScale(), 0.30, 1e-6);
    bool Ended = false;
    float RealSeconds = 0.0f;
    while (!Ended && RealSeconds < 10.0f)
    {
        Ended = Jake.Combat.Tick(Frame * Jake.Combat.GetTimeScale(), Frame).RacheEnded;
        RealSeconds += Frame;
    }
    CHECK(Ended);
    CHECK(RealSeconds <= DesignLaws::RacheMaximumRealSeconds + 2 * Frame);
    CHECK_NEAR(Jake.Combat.GetTimeScale(), 1.0, 1e-6);
}
