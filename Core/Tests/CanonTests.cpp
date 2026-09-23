#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/Combat.h"
#include "DarkArisen/Core/EntityPolicy.h"
#include "DarkArisen/Core/Facts.h"

using namespace DarkArisen::Core;
using DarkArisen::Test::AdvanceUntil;
using DarkArisen::Test::CompleteCampaign;

TEST_CASE("Canon: real Ethan can never be hostile, damaged or a boss")
{
    CHECK(!EntityPolicy::CanBeHostileTarget("character.ethan"));
    CHECK(!EntityPolicy::CanBeBoss("character.ethan"));
    CHECK(!EntityPolicy::CanBeBoss("boss.ethan"));
    CHECK(!EntityPolicy::CanBeBoss("boss.ethan_betrayal"));
    CHECK(EntityPolicy::CanBeBoss("boss.dream_ethan"));
    CHECK(EntityPolicy::CanBeBoss("boss.draven_voss"));

    Combatant Jake;
    Jake.EntityId = "character.jake";
    Combatant Ethan;
    Ethan.EntityId = "character.ethan";
    const DamageResult Result = ResolveMeleeHit(Jake, Ethan, HitKind::Critical);
    CHECK(!Result.Resolved);
    CHECK_NEAR(Ethan.Health.CurrentHealth, 200.0, 1e-6);
    CHECK_NEAR(Ethan.Combat.CurrentPosture, 0.0, 1e-6);
}

TEST_CASE("Canon: the dream figure is a separate combatant that can be fought")
{
    Combatant Jake;
    Jake.EntityId = "character.jake";
    Combatant Dream;
    Dream.EntityId = "boss.dream_ethan";
    CHECK(ResolveMeleeHit(Jake, Dream, HitKind::Heavy).Resolved);
    CHECK(Dream.Health.CurrentHealth < 200.0f);
}

TEST_CASE("Canon: no hostile path connects with a child")
{
    CHECK(!EntityPolicy::CanBeHostileTarget("child.rexa.dock_runner"));
    Combatant Jake;
    Jake.EntityId = "character.jake";
    Combatant Child;
    Child.EntityId = "child.rexa.dock_runner";
    CHECK(!ResolveMeleeHit(Jake, Child, HitKind::Light).Resolved);
}

TEST_CASE("Canon: Ethan is abducted alive in Chapter 1 and recovered only in Chapter 8")
{
    CampaignRuntime Campaign;
    CHECK(AdvanceUntil(Campaign, "Main.C01.04.Undertow"));
    CHECK(Campaign.HasFact(Facts::EthanAbducted));
    CHECK(!Campaign.HasFact(Facts::EthanRecovered));
    CHECK(AdvanceUntil(Campaign, "Main.C08.03.HomewardBearing"));
    CHECK(Campaign.HasFact(Facts::EthanSignalUnderstood));
    CHECK(!Campaign.HasFact(Facts::EthanRecovered));
    CHECK(Campaign.ActivateMission("Main.C08.03.HomewardBearing"));
    CHECK(Campaign.CompleteMission("Main.C08.03.HomewardBearing"));
    CHECK(Campaign.HasFact(Facts::EthanRecovered));
    CHECK(!Campaign.HasFact(Facts::RacheUnlocked));
    CHECK(CompleteCampaign(Campaign));
    CHECK(Campaign.HasFact(Facts::EthanRecovered));
    CHECK(Campaign.HasFact(Facts::RacheUnlocked));
}

TEST_CASE("Canon: forged Ethan or Rache facts are rejected on load")
{
    CampaignState Forged = CampaignState::NewGame();
    Forged.StoryFacts.insert(std::string(Facts::EthanAbducted));
    Forged.StoryFacts.insert(std::string(Facts::EthanRecovered));
    CampaignRuntime Campaign;
    std::vector<std::string> Errors;
    CHECK(!Campaign.LoadState(Forged, Errors));

    CampaignState EarlyRache = CampaignState::NewGame();
    EarlyRache.WorldFacts.insert(std::string(Facts::RacheUnlocked));
    CHECK(!Campaign.LoadState(EarlyRache, Errors));
}

TEST_CASE("Canon: boss registry holds the Nine, the dream figure and Draven only")
{
    CampaignRuntime Campaign;
    CHECK_EQ(CampaignRuntime::CanonicalBossIds().size(), std::size_t{11});
    for (const std::string_view Id : CampaignRuntime::CanonicalBossIds())
    {
        CHECK(EntityPolicy::CanBeBoss(Id));
    }
    CHECK(!Campaign.MarkBossDefeated("boss.ethan"));
    CHECK(!Campaign.MarkBossDefeated("character.ethan"));
    CHECK(Campaign.MarkBossDefeated("boss.herrera"));
    // The dream fight and Draven belong to their own missions.
    CHECK(!Campaign.MarkBossDefeated("boss.dream_ethan"));
    CHECK(!Campaign.MarkBossDefeated("boss.draven_voss"));
    CHECK(AdvanceUntil(Campaign, "Main.C09.03.DreamFight"));
    CHECK(Campaign.ActivateMission("Main.C09.03.DreamFight"));
    CHECK(Campaign.MarkBossDefeated("boss.dream_ethan"));
}
