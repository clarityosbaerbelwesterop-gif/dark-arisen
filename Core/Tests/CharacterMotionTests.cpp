#include "TestHarness.h"

#include "DarkArisen/Core/CharacterMotion.h"

using namespace DarkArisen::Core;

namespace
{
    /** Durations, loops, ground speeds and contacts as in the materialised MotionLibrary.json (Jake set). */
    MotionClipTable JakeClips()
    {
        MotionClipTable T;
        T.Add({"Idle", 3.0, true, 0.01, std::nullopt});
        T.Add({"Walk", 1.1, true, 1.15, std::nullopt});
        T.Add({"Run", 0.766, true, 3.413, std::nullopt});
        T.Add({"Sprint", 0.613, true, 4.267, std::nullopt});
        T.Add({"CombatIdle", 3.0, true, 0.01, std::nullopt});
        T.Add({"CombatForward", 1.1, true, 1.15, std::nullopt});
        T.Add({"CombatStrafeLeft", 1.154, true, 0.441, std::nullopt});
        T.Add({"LightAttack1", 0.72, false, 0.0, 0.42});
        T.Add({"LightAttack2", 0.9, false, 0.0, 0.5});
        T.Add({"HeavyAttack", 1.3, false, 0.0, 0.85});
        T.Add({"DodgeLeft", 0.636, false, 0.0, std::nullopt});
        T.Add({"DodgeForward", 0.96, false, 0.0, std::nullopt});
        T.Add({"HitReactionFront", 0.538, false, 0.0, std::nullopt});
        T.Add({"Death", 1.6, false, 0.0, std::nullopt});
        T.Add({"Swim", 1.6, true, 0.0, std::nullopt});
        return T;
    }
}

TEST_CASE("CharacterMotion: standing, walking, running and sprinting by ground speed")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    CHECK(M.Tick(In, 1.0 / 60.0).State == "Idle");
    In.PlanarSpeed = 1.15;
    const CharacterMotionOutput Walk = M.Tick(In, 1.0 / 60.0);
    CHECK(Walk.State == "Walk");
    CHECK(Walk.Started);
    CHECK_NEAR(Walk.BlendSeconds, 0.2, 1e-12);
    CHECK_NEAR(Walk.PlayRate, 1.0, 1e-12);  // capture speed = travel speed: no foot sliding
    In.PlanarSpeed = 3.3;                   // Jake's run
    const CharacterMotionOutput Run = M.Tick(In, 1.0 / 60.0);
    CHECK(Run.State == "Run");
    CHECK_NEAR(Run.PlayRate, 3.3 / 3.413, 1e-12);
    In.Sprinting = true;
    In.PlanarSpeed = 6.0;                   // Jake's sprint
    const CharacterMotionOutput Sprint = M.Tick(In, 1.0 / 60.0);
    CHECK(Sprint.State == "Sprint");
    CHECK_NEAR(Sprint.PlayRate, 6.0 / 4.267, 1e-12);
    CHECK(Sprint.Loop);
}

TEST_CASE("CharacterMotion: guarded movement strafes and the rate is held in range")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    In.Guarded = true;
    CHECK(M.Tick(In, 0.016).State == "CombatIdle");
    In.PlanarSpeed = 1.5;
    In.Direction = MoveDirection::Left;
    const CharacterMotionOutput Strafe = M.Tick(In, 0.016);
    CHECK(Strafe.State == "CombatStrafeLeft");
    CHECK_NEAR(Strafe.PlayRate, CharacterMotion::MaxRate, 1e-12);  // 1.5 / 0.441 would skate; capped
    In.Direction = MoveDirection::Back;                              // no CombatBack in this table: falls back
    CHECK(M.Tick(In, 0.016).State == "Idle");
}

TEST_CASE("CharacterMotion: a swing reports its contact exactly once, at the clip's contact time")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    In.Combat = CombatState::LightAttacking;
    int Contacts = 0;
    double ContactTime = -1.0;
    for (int Frame = 0; Frame < 90; ++Frame)
    {
        const CharacterMotionOutput Out = M.Tick(In, 1.0 / 60.0);
        CHECK(Out.State == "LightAttack1");
        if (Out.Contact)
        {
            ++Contacts;
            ContactTime = Out.Time;
        }
    }
    CHECK(Contacts == 1);
    CHECK(ContactTime >= 0.42 && ContactTime < 0.42 + 1.0 / 60.0 + 1e-9);
    // the finished swing holds its last frame until the combat state lets go
    CHECK_NEAR(M.Tick(In, 1.0 / 60.0).Time, 0.72, 1e-12);

    // the second swing of the chain is a new clip with its own contact
    In.LightCombo = 1;
    const CharacterMotionOutput Second = M.Tick(In, 1.0 / 60.0);
    CHECK(Second.State == "LightAttack2");
    CHECK(Second.Started);
    CHECK_NEAR(Second.BlendSeconds, 0.08, 1e-12);
}

TEST_CASE("CharacterMotion: a new attack after idle restarts the clip and its contact")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    In.Combat = CombatState::HeavyAttacking;
    int Contacts = 0;
    for (int Frame = 0; Frame < 120; ++Frame) Contacts += M.Tick(In, 1.0 / 60.0).Contact ? 1 : 0;
    In.Combat = CombatState::Idle;
    M.Tick(In, 1.0 / 60.0);
    In.Combat = CombatState::HeavyAttacking;
    const CharacterMotionOutput Again = M.Tick(In, 1.0 / 60.0);
    CHECK(Again.Started);
    CHECK_NEAR(Again.Time, 0.0, 1e-12);
    for (int Frame = 0; Frame < 120; ++Frame) Contacts += M.Tick(In, 1.0 / 60.0).Contact ? 1 : 0;
    CHECK(Contacts == 2);
}

TEST_CASE("CharacterMotion: dodges by side, hit reactions play out, death holds")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    In.Combat = CombatState::Dodging;
    In.DodgeSide = -1;
    CHECK(M.Tick(In, 0.016).State == "DodgeLeft");
    In.DodgeSide = 0;
    CHECK(M.Tick(In, 0.016).State == "DodgeForward");

    In = CharacterMotionInput{};
    In.HitTaken = true;
    CHECK(M.Tick(In, 0.016).State == "HitReactionFront");
    In.HitTaken = false;
    CHECK(M.Tick(In, 0.3).State == "HitReactionFront");  // still playing out
    M.Tick(In, 0.3);                                      // past its 0.538 s
    CHECK(M.Tick(In, 0.016).State == "Idle");

    In.HitTaken = true;
    M.Tick(In, 0.016);
    In.HitTaken = false;
    In.Combat = CombatState::LightAttacking;              // an attack cuts the reaction short
    CHECK(M.Tick(In, 0.016).State == "LightAttack1");

    In = CharacterMotionInput{};
    In.Dead = true;
    const CharacterMotionOutput Fall = M.Tick(In, 0.016);
    CHECK(Fall.State == "Death");
    CHECK_NEAR(Fall.BlendSeconds, 0.15, 1e-12);
    for (int Frame = 0; Frame < 300; ++Frame) M.Tick(In, 0.016);
    const CharacterMotionOutput Lying = M.Tick(In, 0.016);
    CHECK(!Lying.Started);
    CHECK_NEAR(Lying.Time, 1.6, 1e-12);
}

TEST_CASE("CharacterMotion: swimming, missing clips fall back, an empty table plays nothing")
{
    const MotionClipTable Clips = JakeClips();
    CharacterMotion M(Clips);
    CharacterMotionInput In;
    In.Swimming = true;
    In.PlanarSpeed = 1.0;
    const CharacterMotionOutput Swim = M.Tick(In, 0.016);
    CHECK(Swim.State == "Swim");
    CHECK_NEAR(Swim.PlayRate, 1.0, 1e-12);                // no ground speed in a bench capture: unscaled
    CHECK_NEAR(Swim.BlendSeconds, 0.35, 1e-12);
    In.Struggling = true;                                 // no StrugglingSwim here: stays in Swim
    CHECK(M.Tick(In, 0.016).State == "Swim");

    const MotionClipTable Empty;
    CharacterMotion Nothing(Empty);
    CHECK(Nothing.Tick(CharacterMotionInput{}, 0.016).State.empty());
}
