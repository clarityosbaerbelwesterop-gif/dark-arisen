#include "TestHarness.h"

#include "DarkArisen/Core/EnemyBrain.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace DarkArisen::Core;

namespace
{
    constexpr float Frame = 1.0f / 60.0f;

    EnemyPerception Jake(const float Distance)
    {
        EnemyPerception Perception;
        Perception.TargetId = "character.jake";
        Perception.TargetVisible = true;
        Perception.DistanceMetres = Distance;
        return Perception;
    }

    /** Brain and combat model driven together, like EnemyBrainComponent; records what the player would read. */
    struct Duel
    {
        EnemyBrain Brain{EnemyProfile::DravenVoss()};
        Combatant Draven;
        std::vector<std::string> Moves;
        std::vector<BossStance> Stances;
        std::vector<MoveDelivery> Commits;
        std::vector<HitKind> CommitWeights;
        int Telegraphs = 0;
        int Feints = 0;
        float LastTelegraphAt = -1.0f;
        float ShortestTelegraph = 100.0f;
        float Clock = 0.0f;

        Duel()
        {
            Draven.EntityId = "character.draven_voss";
            Brain.Configure(Draven);
        }

        EnemyIntent Step(const EnemyPerception& Perception)
        {
            Clock += Frame;
            const EnemyIntent Intent = Brain.Tick(Frame, Perception, Draven);
            if (!Intent.MoveId.empty()) Moves.push_back(Intent.MoveId);
            if (Intent.StanceChanged) Stances.push_back(*Intent.StanceChanged);
            if (Intent.FeintCancelled) ++Feints;
            if (Intent.BeginTelegraph)
            {
                ++Telegraphs;
                LastTelegraphAt = Clock;
            }
            if (Intent.CommitAttack || Intent.FeintCancelled)
            {
                ShortestTelegraph = std::min(ShortestTelegraph, Clock - LastTelegraphAt);
            }
            if (Intent.CommitAttack)
            {
                Commits.push_back(Intent.Delivery);
                CommitWeights.push_back(*Intent.CommitAttack);
                if (Intent.Delivery == MoveDelivery::Melee)
                {
                    if (*Intent.CommitAttack == HitKind::Heavy) Draven.Combat.PerformHeavyAttack(Draven.Stamina);
                    else Draven.Combat.PerformLightAttack(Draven.Stamina);
                }
            }
            Draven.Combat.Tick(Frame, Frame);
            Draven.Stamina.Tick(Frame);
            return Intent;
        }

        void Run(const EnemyPerception& Perception, const float Seconds)
        {
            for (float Time = 0.0f; Time < Seconds; Time += Frame) Step(Perception);
        }

        void DropTo(const float HealthFraction)
        {
            Draven.Health.ApplyDamage(Draven.Health.CurrentHealth - Draven.Health.MaxHealth * HealthFraction, RallyDamageClass::StandardEnemy);
        }

        int Count(const std::string& MoveId) const { return static_cast<int>(std::count(Moves.begin(), Moves.end(), MoveId)); }
    };

    Combatant Target()
    {
        Combatant Jake;
        Jake.EntityId = "character.jake";
        return Jake;
    }
}

TEST_CASE("Draven: an authored duel, not a boarder with more health")
{
    const EnemyProfile Draven = EnemyProfile::DravenVoss();
    std::string Error;
    CHECK(Draven.Validate(Error));
    CHECK(Error.empty());
    CHECK_EQ(Draven.Moves.size(), std::size_t{12});
    CHECK(EnemyProfile::Boarder().Moves.empty());
    CHECK_NEAR(Draven.Phases[1].HealthFraction, 0.65, 1e-6); // Unreal thresholds kept
    CHECK_NEAR(Draven.Phases[2].HealthFraction, 0.30, 1e-6);
    for (const BossMove& Move : Draven.Moves)
    {
        for (const MoveStep& Step : Move.Steps) CHECK(Step.TelegraphSeconds >= EnemyProfile::MinimumTelegraphSeconds);
    }
}

TEST_CASE("Draven phase 1: the officer's rotation, three-form chain and the answer to a deflect")
{
    Duel Fight;
    Fight.Run(Jake(1.6f), 6.0f);
    CHECK(!Fight.Stances.empty() && Fight.Stances.front() == BossStance::Officer);
    CHECK(Fight.Count("draven.lunge_thrust") >= 1);
    CHECK(Fight.Count("draven.three_form_slash") >= 1);
    // Deterministic rhythm: the two pattern moves alternate while both are in reach.
    CHECK(Fight.Moves.size() >= 2 && Fight.Moves[0] != Fight.Moves[1]);
    CHECK(Fight.ShortestTelegraph >= EnemyProfile::MinimumTelegraphSeconds - Frame);
    CHECK(std::all_of(Fight.Commits.begin(), Fight.Commits.end(), [](const MoveDelivery D) { return D == MoveDelivery::Melee; }));

    // Jake deflects: the next thing Draven does is the disengage, before his normal cooldown.
    Duel Punish;
    Punish.Run(Jake(1.0f), 0.2f);
    while (Punish.Brain.GetState() != EnemyState::Recover) Punish.Step(Jake(1.0f));
    DamageResult Deflected;
    Deflected.Resolved = true;
    Deflected.Deflected = true;
    Punish.Brain.NotifyHitResolved(Deflected);
    const std::size_t Before = Punish.Moves.size();
    Punish.Run(Jake(1.0f), 0.3f);
    CHECK(Punish.Moves.size() == Before + 1 && Punish.Moves.back() == "draven.saber_disengage");
}

TEST_CASE("Draven phase 2: the pirate feints, shoots a kiting Jake, throws powder and cycles his stances")
{
    Duel Fight;
    Fight.DropTo(0.6f);
    Fight.Run(Jake(1.5f), 12.0f);
    CHECK(std::find(Fight.Stances.begin(), Fight.Stances.end(), BossStance::Pirate) != Fight.Stances.end());
    CHECK(Fight.Count("draven.false_lunge") >= 1);
    CHECK(Fight.Feints >= 1);
    CHECK(Fight.Count("draven.saber_and_pistol") >= 1);
    CHECK(std::find(Fight.Commits.begin(), Fight.Commits.end(), MoveDelivery::PistolShot) != Fight.Commits.end());

    Duel Kite;
    Kite.DropTo(0.6f);
    Kite.Run(Jake(7.0f), 3.0f); // out of sabre reach: he does not just walk, he draws
    CHECK_EQ(Kite.Count("draven.pistol_shot"), 1);
    Kite.Run(Jake(7.0f), 18.0f);
    CHECK(Kite.Count("draven.powder_bomb") >= 1); // every 20 s of engagement
    CHECK(std::find(Kite.Commits.begin(), Kite.Commits.end(), MoveDelivery::PowderBomb) != Kite.Commits.end());

    Duel Cycle;
    Cycle.DropTo(0.6f);
    Cycle.Run(Jake(1.5f), 65.0f);
    // Met already wounded, he opens as the pirate; the stance shifts every 30 s, never mid-move.
    const std::vector<BossStance> Expected = {BossStance::Pirate, BossStance::Guarded, BossStance::Officer};
    CHECK(Cycle.Stances.size() >= Expected.size() && std::equal(Expected.begin(), Expected.end(), Cycle.Stances.begin()));
}

TEST_CASE("Draven's low guard answers a committed attack")
{
    Duel Fight;
    Fight.DropTo(0.6f);
    Fight.Run(Jake(1.5f), 31.0f);
    CHECK(Fight.Brain.GetStance() == BossStance::Guarded);
    while (Fight.Brain.GetState() == EnemyState::Attack) Fight.Step(Jake(1.5f));
    EnemyPerception Attacking = Jake(1.5f);
    Attacking.TargetAttacking = true;
    const std::size_t Before = Fight.Moves.size();
    for (int Frames = 0; Frames < 120 && Fight.Moves.size() == Before; ++Frames) Fight.Step(Attacking);
    CHECK(Fight.Moves.size() == Before + 1 && Fight.Moves.back() == "draven.guard_counter");
}

TEST_CASE("Draven phase 3: the man fights bare-handed, clinches, disarms once and waits")
{
    Duel Fight;
    Fight.DropTo(0.28f);
    Fight.Run(Jake(1.2f), 10.0f);
    CHECK(Fight.Brain.GetPhase() == 3);
    CHECK(!Fight.Stances.empty() && Fight.Stances.back() == BossStance::Unarmed);
    CHECK(Fight.Count("draven.close_quarters") >= 1);
    CHECK(Fight.Count("draven.embrace") >= 1); // Jake stayed within two metres for three seconds
    CHECK(std::find(Fight.Commits.begin(), Fight.Commits.end(), MoveDelivery::Grab) != Fight.Commits.end());
    CHECK_EQ(Fight.Count("draven.disarm"), 0);

    Fight.DropTo(0.24f);
    Fight.Run(Jake(1.2f), 3.0f);
    CHECK_EQ(Fight.Count("draven.disarm"), 1);
    // Jake's sword is on the deck: Draven attacks nothing until it is back in his hand.
    EnemyPerception Unarmed = Jake(1.2f);
    Unarmed.TargetArmed = false;
    while (Fight.Brain.GetState() == EnemyState::Attack) Fight.Step(Unarmed);
    const std::size_t Waiting = Fight.Moves.size();
    Fight.Run(Unarmed, 5.0f);
    CHECK_EQ(Fight.Moves.size(), Waiting);
    Fight.Run(Jake(1.2f), 5.0f);
    CHECK(Fight.Moves.size() > Waiting);
    CHECK_EQ(Fight.Count("draven.disarm"), 1);
}

TEST_CASE("Draven: a posture break ends his chain")
{
    Duel Fight;
    Fight.Run(Jake(1.0f), 0.1f);
    while (Fight.Brain.GetState() != EnemyState::Attack) Fight.Step(Jake(1.0f));
    Fight.Draven.Combat.AddPostureDamage(1000.0f);
    CHECK(Fight.Draven.Combat.GetState() == CombatState::Staggered);
    const std::size_t Commits = Fight.Commits.size();
    Fight.Step(Jake(1.0f));
    CHECK(Fight.Brain.GetState() == EnemyState::Recover);
    Fight.Run(Jake(1.0f), 0.5f);
    CHECK_EQ(Fight.Commits.size(), Commits);
}

TEST_CASE("Boss deliveries keep the combat laws: shots deflect, powder does not, a disarm only dodges")
{
    Combatant Draven;
    Draven.EntityId = "character.draven_voss";
    Combatant Jake = Target();
    const DamageResult Shot = ResolveBossDelivery(Draven, Jake, MoveDelivery::PistolShot, HitKind::Heavy);
    CHECK(Shot.Resolved && !Shot.Deflected);
    CHECK_NEAR(Shot.HealthDamage, 42.0, 1e-4);

    Combatant Parrying = Target();
    CHECK(Parrying.Combat.PerformParry(Parrying.Stamina));
    CHECK(ResolveBossDelivery(Draven, Parrying, MoveDelivery::PistolShot, HitKind::Heavy).Deflected);
    Combatant ParryingBomb = Target();
    CHECK(ParryingBomb.Combat.PerformParry(ParryingBomb.Stamina));
    const DamageResult Bomb = ResolveBossDelivery(Draven, ParryingBomb, MoveDelivery::PowderBomb, HitKind::Heavy);
    CHECK(Bomb.Resolved && !Bomb.Deflected && Bomb.HealthDamage > 0.0f);
    Combatant ParryingDisarm = Target();
    CHECK(ParryingDisarm.Combat.PerformParry(ParryingDisarm.Stamina));
    const DamageResult Disarm = ResolveBossDelivery(Draven, ParryingDisarm, MoveDelivery::Disarm, HitKind::Light);
    CHECK(Disarm.Resolved && !Disarm.Deflected && Disarm.PostureDamage == 0.0f);

    Combatant Dodging = Target();
    CHECK(Dodging.Combat.PerformDodge(Dodging.Stamina, false));
    CHECK(ResolveBossDelivery(Draven, Dodging, MoveDelivery::Disarm, HitKind::Light).Invulnerable);

    Combatant ParryingGrab = Target();
    CHECK(ParryingGrab.Combat.PerformParry(ParryingGrab.Stamina));
    const float PostureBefore = Draven.Combat.CurrentPosture;
    CHECK(ResolveBossDelivery(Draven, ParryingGrab, MoveDelivery::Grab, HitKind::Heavy).Deflected);
    CHECK(Draven.Combat.CurrentPosture > PostureBefore); // a deflected clinch costs him posture
}

TEST_CASE("Movesets fail closed: unreadable, stalling or impossible moves are rejected")
{
    std::string Error;
    EnemyProfile Fast = EnemyProfile::DravenVoss();
    Fast.Moves[0].Steps[0].TelegraphSeconds = 0.1f;
    CHECK(!Fast.Validate(Error));
    EnemyProfile FeintLast = EnemyProfile::DravenVoss();
    FeintLast.Moves[0].Steps[0].Feint = true;
    CHECK(!FeintLast.Validate(Error));
    EnemyProfile FreeDisarm = EnemyProfile::DravenVoss();
    FreeDisarm.Moves.back().Trigger = MoveTrigger::Pattern;
    FreeDisarm.Moves.back().TriggerValue = 0.0f;
    CHECK(!FreeDisarm.Validate(Error));
    EnemyProfile LongArm = EnemyProfile::DravenVoss();
    LongArm.Moves[1].MaxRangeMetres = 6.0f;
    CHECK(!LongArm.Validate(Error));
    EnemyProfile Stalls = EnemyProfile::DravenVoss();
    Stalls.Moves.erase(std::remove_if(Stalls.Moves.begin(), Stalls.Moves.end(), [](const BossMove& Move) { return Move.MoveId == "draven.low_cut"; }),
        Stalls.Moves.end());
    CHECK(!Stalls.Validate(Error));
    EnemyProfile NoRule = EnemyProfile::DravenVoss();
    NoRule.StanceRules.pop_back();
    CHECK(!NoRule.Validate(Error));
}
