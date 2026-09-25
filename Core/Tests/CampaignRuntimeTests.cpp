#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/Facts.h"
#include "DarkArisen/Core/SaveRules.h"

using namespace DarkArisen::Core;
using DarkArisen::Test::AdvanceUntil;
using DarkArisen::Test::CompleteCampaign;
using DarkArisen::Test::RecruitOpeningCrew;

TEST_CASE("Campaign: new game is valid and starts at Home Water")
{
    CampaignRuntime Campaign;
    std::vector<std::string> Errors;
    CHECK(Campaign.Validate(Errors));
    CHECK_EQ(Campaign.State().CurrentMission, std::string("Main.C01.01.HomeWater"));
    CHECK_EQ(Campaign.State().CurrentChapter, 1);
    CHECK(Campaign.GetMissionState("Main.C01.01.HomeWater") == MissionState::Available);
    CHECK(Campaign.GetMissionState("Main.C01.02.BlackSails") == MissionState::Locked);
}

TEST_CASE("Campaign: missions cannot be skipped, doubled or completed without activation")
{
    CampaignRuntime Campaign;
    CHECK(!Campaign.ActivateMission("Main.C01.02.BlackSails"));
    CHECK(!Campaign.CompleteMission("Main.C01.01.HomeWater"));
    CHECK(!Campaign.ActivateMission("Main.Unknown"));
    CHECK(Campaign.ActivateMission("Main.C01.01.HomeWater"));
    CHECK(!Campaign.ActivateMission("Main.C01.01.HomeWater"));
    CHECK(Campaign.CompleteMission("Main.C01.01.HomeWater"));
    CHECK(!Campaign.CompleteMission("Main.C01.01.HomeWater"));
    CHECK(Campaign.GetMissionState("Main.C01.02.BlackSails") == MissionState::Available);
}

TEST_CASE("Campaign: ship mission cannot complete without the recruited core crew")
{
    CampaignRuntime Campaign;
    CHECK(AdvanceUntil(Campaign, "Main.C02.01.ShatteredCoast"));
    CHECK(Campaign.ActivateMission("Main.C02.01.ShatteredCoast"));
    CHECK(Campaign.CompleteMission("Main.C02.01.ShatteredCoast"));
    CHECK(Campaign.ActivateMission("Main.C02.02.AShipToTake"));
    CHECK(!Campaign.CompleteMission("Main.C02.02.AShipToTake"));
    CHECK(RecruitOpeningCrew(Campaign));
    CHECK(Campaign.CompleteMission("Main.C02.02.AShipToTake"));
    CHECK(Campaign.HasFact(Facts::LaLiberacionOwned));
    CHECK(Campaign.HasFact("Crew.CoreThreeJoined"));
    CHECK_EQ(Campaign.State().CurrentShip, std::string("ship.la_liberacion"));
    for (const CrewRelationship& Crew : Campaign.State().Crew)
    {
        CHECK(Crew.Aboard);
    }
}

TEST_CASE("Campaign: start to credits sets every authority fact and stays valid")
{
    CampaignRuntime Campaign;
    CHECK(CompleteCampaign(Campaign));
    std::vector<std::string> Errors;
    CHECK(Campaign.Validate(Errors));
    for (const std::string_view Fact : {Facts::FamilyIntact, Facts::EthanPresent, Facts::DravenIdentified,
             Facts::MarcDead, Facts::DeniseDead, Facts::EthanAbducted, Facts::JakeOverboard, Facts::JakeSurvived,
             Facts::EthanAliveLead, Facts::EthanRecovered, Facts::EthanNoLongerCaptive, Facts::DreamResolved,
             Facts::DravenDefeated, Facts::DravenKilled, Facts::MainComplete, Facts::CreditsReachable})
    {
        CHECK(Campaign.HasFact(Fact));
    }
    for (const char* Chapter : {"Chapter.01.Complete", "Chapter.02.Complete", "Chapter.03.Complete",
             "Chapter.04.Complete", "Chapter.05.Complete", "Chapter.06.Complete", "Chapter.07.Complete",
             "Chapter.08.Complete", "Chapter.09.Complete", "Chapter.10.Complete"})
    {
        CHECK(Campaign.HasFact(Chapter));
    }
    CHECK(Campaign.HasFact(Facts::RacheUnlocked));
    CHECK(!Campaign.HasFact(Facts::DravenCaptured));
    CHECK_EQ(Campaign.State().CurrentMission, std::string("Main.C10.05.TheWakeAfter"));
    CHECK_EQ(Campaign.State().CurrentChapter, 10);
}

TEST_CASE("Campaign: autosave fires exactly at the ten chapter boundaries, before listeners")
{
    CampaignRuntime Campaign;
    std::vector<int> BoundaryChapters;
    int Autosaves = 0;
    bool ListenerSawSavedFacts = true;
    CampaignWorldServices Services;
    Services.NotifyChapterBoundary = [&](const int Chapter) { BoundaryChapters.push_back(Chapter); };
    Services.IsAutosaveSuppressed = [] { return false; };
    Services.WriteAutosave = [&](const CampaignState& State, const DesignLaws::AutosaveTrigger Trigger)
    {
        CHECK(Trigger == DesignLaws::AutosaveTrigger::ChapterBoundary);
        std::vector<std::string> Errors;
        CHECK(ValidateCampaignState(State, Errors));
        ++Autosaves;
        return true;
    };
    Campaign.SetWorldServices(Services);
    int AutosavesWhenUndertowCompleted = -1;
    Campaign.AddMissionListener([&](const std::string_view Id, const MissionState State)
    {
        if (State != MissionState::Completed) return;
        if (Id == "Main.C01.04.Undertow")
        {
            AutosavesWhenUndertowCompleted = Autosaves;
            ListenerSawSavedFacts = ListenerSawSavedFacts && Campaign.HasFact("Chapter.01.Complete");
        }
    });
    CHECK(CompleteCampaign(Campaign));
    CHECK_EQ(Autosaves, 10);
    CHECK_EQ(AutosavesWhenUndertowCompleted, 1);
    CHECK(ListenerSawSavedFacts);
    CHECK_EQ(BoundaryChapters.size(), std::size_t{10});
    CHECK_EQ(BoundaryChapters.front(), 2);
    CHECK(Campaign.LastAutosaveError().empty());
}

TEST_CASE("Campaign: suppressed autosave and failed writes never corrupt progression")
{
    CampaignRuntime Campaign;
    int Autosaves = 0;
    bool Suppressed = true;
    CampaignWorldServices Services;
    Services.IsAutosaveSuppressed = [&] { return Suppressed; };
    Services.WriteAutosave = [&](const CampaignState&, DesignLaws::AutosaveTrigger) { ++Autosaves; return false; };
    Campaign.SetWorldServices(Services);
    CHECK(AdvanceUntil(Campaign, "Main.C02.01.ShatteredCoast"));
    CHECK_EQ(Autosaves, 0);
    Suppressed = false;
    CHECK(!Campaign.CompleteRest());
    CHECK_EQ(Autosaves, 1);
    CHECK_EQ(Campaign.LastAutosaveError(), std::string("Autosave write failed."));
    CHECK(Campaign.GetMissionState("Main.C01.04.Undertow") == MissionState::Completed);
}

TEST_CASE("Campaign: rest is the second legal autosave source")
{
    CampaignRuntime Campaign;
    int RestSaves = 0;
    CampaignWorldServices Services;
    Services.WriteAutosave = [&](const CampaignState&, const DesignLaws::AutosaveTrigger Trigger)
    {
        RestSaves += Trigger == DesignLaws::AutosaveTrigger::Rest ? 1 : 0;
        return true;
    };
    Campaign.SetWorldServices(Services);
    CHECK(Campaign.CompleteRest());
    CHECK_EQ(RestSaves, 1);
}

TEST_CASE("Campaign: world facts are validated and story facts are mission-owned")
{
    CampaignRuntime Campaign;
    CHECK(!Campaign.SetWorldFact(Facts::LaLiberacionOwned));
    CHECK(!Campaign.SetWorldFact(Facts::EthanRecovered));
    CHECK(!Campaign.SetWorldFact(Facts::RacheUnlocked));
    CHECK(Campaign.SetWorldFact("World.OuterReefReached"));
    int Notifications = 0;
    Campaign.AddFactListener([&](std::string_view, bool) { ++Notifications; });
    CHECK(Campaign.SetWorldFact("World.OuterReefReached"));
    CHECK_EQ(Notifications, 0);
    CHECK(Campaign.SetWorldFact("World.OuterReefReached", false));
    CHECK_EQ(Notifications, 1);
}

TEST_CASE("Campaign: failed missions re-enter through authored state, not a rewind")
{
    CampaignRuntime Campaign;
    CHECK(Campaign.ActivateMission("Main.C01.01.HomeWater"));
    CHECK(Campaign.FailMission("Main.C01.01.HomeWater"));
    std::vector<std::string> Errors;
    CHECK(Campaign.Validate(Errors));
    CHECK(Campaign.ActivateMission("Main.C01.01.HomeWater"));
    CHECK(Campaign.CompleteMission("Main.C01.01.HomeWater"));
}

TEST_CASE("Campaign: Draven capture is an authored alternative to his death")
{
    CampaignRuntime Campaign;
    CHECK(!Campaign.MarkDravenCaptured());
    CHECK(AdvanceUntil(Campaign, "Main.C10.04.DravenVoss"));
    CHECK(Campaign.ActivateMission("Main.C10.04.DravenVoss"));
    CHECK(Campaign.MarkBossDefeated("boss.draven_voss"));
    CHECK(Campaign.MarkDravenCaptured());
    CHECK(Campaign.CompleteMission("Main.C10.04.DravenVoss"));
    CHECK(Campaign.HasFact(Facts::DravenCaptured));
    CHECK(!Campaign.HasFact(Facts::DravenKilled));
    CHECK(Campaign.HasFact(Facts::DravenDefeated));
}

TEST_CASE("Campaign: treasures cap at the canonical nine")
{
    CampaignRuntime Campaign;
    for (int Index = 0; Index < 9; ++Index)
    {
        CHECK(Campaign.RecoverTreasure("treasure.state." + std::to_string(Index)));
    }
    CHECK(Campaign.HasFact(Facts::StateTreasuresComplete));
    CHECK(!Campaign.RecoverTreasure("treasure.state.9"));
    CHECK(!Campaign.RecoverTreasure("treasure.state.0"));
}
