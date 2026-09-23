#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/EnemyBrain.h"
#include "DarkArisen/Core/Facts.h"

using namespace DarkArisen::Core;
using DarkArisen::Test::AdvanceUntil;

namespace
{
    constexpr float Frame = 1.0f / 60.0f;

    EnemyPerception SeeJake(const float Distance)
    {
        EnemyPerception Perception;
        Perception.TargetId = "character.jake";
        Perception.TargetVisible = true;
        Perception.DistanceMetres = Distance;
        return Perception;
    }

    /** Runs brain + combat model together like the engine adapter does. */
    struct Harness
    {
        EnemyBrain Brain;
        Combatant Self;
        int Telegraphs = 0;
        int Commits = 0;
        int Heavy = 0;

        explicit Harness(EnemyProfile Profile) : Brain(std::move(Profile))
        {
            Self.EntityId = "enemy.test";
            Brain.Configure(Self);
        }

        EnemyIntent Step(const EnemyPerception& Perception)
        {
            const EnemyIntent Intent = Brain.Tick(Frame, Perception, Self);
            Telegraphs += Intent.BeginTelegraph ? 1 : 0;
            if (Intent.CommitAttack)
            {
                ++Commits;
                Heavy += *Intent.CommitAttack == HitKind::Heavy ? 1 : 0;
                if (*Intent.CommitAttack == HitKind::Heavy) Self.Combat.PerformHeavyAttack(Self.Stamina);
                else Self.Combat.PerformLightAttack(Self.Stamina);
            }
            Self.Combat.Tick(Frame, Frame);
            Self.Stamina.Tick(Frame);
            return Intent;
        }
    };
}

TEST_CASE("Enemy: every authored profile validates")
{
    std::string Error;
    CHECK(EnemyProfile::Boarder().Validate(Error));
    CHECK(EnemyProfile::DreamEthan().Validate(Error));
    CHECK(EnemyProfile::DravenVoss().Validate(Error));
    CHECK(EnemyProfile::HolderBoss("boss.herrera", "Main.C04.03.HerrerasFall").Validate(Error));
    CHECK(!EnemyProfile::HolderBoss("boss.ethan", "Main.C09.03.DreamFight").Validate(Error));
}

TEST_CASE("Enemy: detect, approach, telegraph, commit, recover, reposition (UE state order)")
{
    Harness Boarder(EnemyProfile::Boarder());
    CHECK(Boarder.Step(SeeJake(8.0f)).State == EnemyState::Detect);
    const EnemyIntent Approach = Boarder.Step(SeeJake(8.0f));
    CHECK(Approach.State == EnemyState::Approach && Approach.MoveTowardTarget);
    CHECK(Boarder.Step(SeeJake(2.0f)).State == EnemyState::Combat);
    const EnemyIntent Telegraph = Boarder.Step(SeeJake(2.0f));
    CHECK(Telegraph.State == EnemyState::Attack && Telegraph.BeginTelegraph.has_value());
    CHECK(!Telegraph.CommitAttack.has_value());
    int Frames = 0;
    while (Boarder.Commits == 0 && Frames < 120)
    {
        Boarder.Step(SeeJake(2.0f));
        ++Frames;
    }
    CHECK_EQ(Boarder.Commits, 1);
    CHECK(Frames >= 20); // 0.35 s telegraph at 60 fps
    CHECK(Boarder.Self.Combat.GetState() == CombatState::LightAttacking);
    bool SawRecover = false;
    bool SawReposition = false;
    for (int Step = 0; Step < 180; ++Step)
    {
        const EnemyState State = Boarder.Step(SeeJake(2.0f)).State;
        SawRecover = SawRecover || State == EnemyState::Recover;
        SawReposition = SawReposition || State == EnemyState::Reposition;
    }
    CHECK(SawRecover && SawReposition);
    CHECK(Boarder.Commits >= 2); // cooldown 1.15 s elapsed in 3 s
    CHECK_EQ(Boarder.Telegraphs, Boarder.Commits + (Boarder.Brain.GetState() == EnemyState::Attack ? 1 : 0));
}

TEST_CASE("Enemy: a posture break during the wind-up cancels the attack")
{
    Harness Boarder(EnemyProfile::Boarder());
    for (int Step = 0; Step < 4; ++Step) Boarder.Step(SeeJake(2.0f));
    CHECK(Boarder.Brain.GetState() == EnemyState::Attack);
    Boarder.Self.Combat.AddPostureDamage(95.0f);
    CHECK(Boarder.Step(SeeJake(2.0f)).State == EnemyState::Recover);
    for (int Step = 0; Step < 60; ++Step) Boarder.Step(SeeJake(2.0f));
    CHECK_EQ(Boarder.Commits, 0);
}

TEST_CASE("Enemy: losing the target searches for four seconds, then idles")
{
    Harness Boarder(EnemyProfile::Boarder());
    Boarder.Step(SeeJake(5.0f));
    Boarder.Step(SeeJake(5.0f));
    EnemyPerception Lost;
    CHECK(Boarder.Step(Lost).State == EnemyState::Search);
    for (int Step = 0; Step < 230; ++Step) Boarder.Step(Lost);
    CHECK(Boarder.Brain.GetState() == EnemyState::Search);
    for (int Step = 0; Step < 20; ++Step) Boarder.Step(Lost);
    CHECK(Boarder.Brain.GetState() == EnemyState::Idle);
    CHECK(Boarder.Step(SeeJake(20.0f)).State == EnemyState::Idle); // beyond 9 m awareness
}

TEST_CASE("Enemy: real Ethan and children are never pursued")
{
    Harness Boarder(EnemyProfile::Boarder());
    EnemyPerception Ethan = SeeJake(1.5f);
    Ethan.TargetId = "character.ethan";
    for (int Step = 0; Step < 60; ++Step) Boarder.Step(Ethan);
    CHECK(Boarder.Brain.GetState() == EnemyState::Idle);
    EnemyPerception Child = SeeJake(1.5f);
    Child.TargetId = "child.rexa.dock_runner";
    for (int Step = 0; Step < 60; ++Step) Boarder.Step(Child);
    CHECK(Boarder.Brain.GetState() == EnemyState::Idle);
    CHECK_EQ(Boarder.Commits, 0);
}

TEST_CASE("Enemy: Draven escalates through three phases and never de-escalates")
{
    Harness Draven(EnemyProfile::DravenVoss());
    CHECK_NEAR(Draven.Self.Health.MaxHealth, 520.0, 1e-6);
    CHECK_NEAR(Draven.Self.Combat.MaxPosture, 190.0, 1e-6);
    CHECK_NEAR(Draven.Self.Combat.PoiseMultiplier, 1.35, 1e-6);
    Draven.Self.Health.ApplyDamage(520.0f * 0.40f, RallyDamageClass::StandardEnemy);
    const EnemyIntent Two = Draven.Step(SeeJake(2.0f));
    CHECK(Two.PhaseChanged && *Two.PhaseChanged == 2);
    CHECK_NEAR(Draven.Self.Combat.PoiseMultiplier, 1.55, 1e-6);
    Draven.Self.Health.ApplyDamage(520.0f * 0.35f, RallyDamageClass::StandardEnemy);
    CHECK(Draven.Step(SeeJake(2.0f)).PhaseChanged.value_or(0) == 3);
    Draven.Self.Health.Heal(400.0f);
    Draven.Step(SeeJake(2.0f));
    CHECK_EQ(Draven.Brain.GetPhase(), 3);
    for (int Step = 0; Step < 600; ++Step) Draven.Step(SeeJake(2.0f));
    CHECK(Draven.Heavy >= 1); // phase 3 mixes heavy blows in on a learnable rhythm
}

TEST_CASE("Enemy: boss defeat resolves the Dream Fight only while it is active")
{
    CampaignRuntime Campaign;
    const EnemyProfile Dream = EnemyProfile::DreamEthan();
    CHECK(!ResolveBossDefeat(Campaign, Dream));
    CHECK(AdvanceUntil(Campaign, "Main.C09.03.DreamFight"));
    CHECK(Campaign.ActivateMission("Main.C09.03.DreamFight"));
    CHECK(ResolveBossDefeat(Campaign, Dream));
    CHECK(Campaign.HasBossDefeated("boss.dream_ethan"));
    CHECK(Campaign.GetMissionState("Main.C09.03.DreamFight") == MissionState::Completed);
    CHECK(Campaign.HasFact(Facts::DreamResolved));
    CHECK(Campaign.HasFact(Facts::EthanRecovered)); // real Ethan untouched by the dream fight
    CHECK_EQ(Campaign.State().QuestOutcomes.at("DreamEthan.Outcome"), std::string("Accepted"));
}

TEST_CASE("Enemy: dead enemies stay dead")
{
    Harness Boarder(EnemyProfile::Boarder());
    Boarder.Self.Health.ApplyDamage(1000.0f);
    CHECK(Boarder.Step(SeeJake(1.0f)).State == EnemyState::Dead);
    Boarder.Self.Health.ResetForRespawn(1.0f);
    CHECK(Boarder.Step(SeeJake(1.0f)).State == EnemyState::Dead);
}
