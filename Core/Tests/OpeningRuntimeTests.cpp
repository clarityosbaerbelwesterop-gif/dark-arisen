#include "TestHarness.h"

#include "DarkArisen/Core/Facts.h"
#include "DarkArisen/Core/OpeningRuntime.h"
#include "DarkArisen/Core/SaveCodec.h"

using namespace DarkArisen::Core;

namespace
{
    void PlayRaid(OpeningRuntime& Opening)
    {
        for (const char* Family : {"character.marc", "character.denise", "character.ethan"})
        {
            CHECK(Opening.SignalFamilyInteraction(Family));
        }
        CHECK(Opening.SignalFleetDetected());
        CHECK(Opening.SignalBoardingStarted());
        CHECK(Opening.BeginBoardingEncounter("Encounter.HarlowDeck.FirstBoarders", 3));
        for (int Index = 0; Index < 3; ++Index)
        {
            CHECK(Opening.SignalBoarderDefeated("Encounter.HarlowDeck.FirstBoarders"));
        }
        CHECK(Opening.SignalDravenBoarded("Encounter.HarlowDeck.FirstBoarders"));
        CHECK(Opening.SignalTakingStarted());
        CHECK(Opening.SignalTakingCinematicComplete());
    }
}

TEST_CASE("Opening: Harlow ship to Driftwood Camp vertical slice path")
{
    CampaignRuntime Campaign;
    std::vector<std::string> Cinematics;
    std::vector<OpeningLocation> Locations;
    OpeningRuntime::Events Events;
    Events.CinematicRequested = [&](const std::string_view Id) { Cinematics.emplace_back(Id); };
    Events.LocationChanged = [&](OpeningLocation, const OpeningLocation Now) { Locations.push_back(Now); };
    OpeningRuntime Opening(Campaign, Events);

    PlayRaid(Opening);
    CHECK(Campaign.HasFact(Facts::MarcDead));
    CHECK(Campaign.HasFact(Facts::DeniseDead));
    CHECK(Campaign.HasFact(Facts::EthanAbducted));
    CHECK(!Campaign.HasFact(Facts::EthanRecovered));
    CHECK(Campaign.GetMissionState("Main.C01.04.Undertow") == MissionState::Active);

    CHECK(Opening.SignalEnteredWater());
    CHECK(Opening.SignalReachedOuterReef());
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodBeach));
    CHECK(Opening.SignalRecoveryComplete());
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodCamp));

    CHECK(Campaign.GetMissionState("Main.C01.04.Undertow") == MissionState::Completed);
    CHECK(Campaign.GetMissionState("Main.C02.01.ShatteredCoast") == MissionState::Active);
    CHECK(Campaign.HasFact("World.DriftwoodCampReached"));
    CHECK_EQ(Campaign.State().CheckpointId, std::string("Checkpoint.Moran.DriftwoodBeach"));
    CHECK(Opening.Progress().Location == OpeningLocation::DriftwoodCamp);
    CHECK_EQ(Cinematics.size(), std::size_t{4});
    CHECK_EQ(Locations.size(), std::size_t{4});
}

TEST_CASE("Opening: raid beats cannot be skipped or reordered")
{
    CampaignRuntime Campaign;
    OpeningRuntime Opening(Campaign);
    CHECK(!Opening.SignalFleetDetected());
    CHECK(!Opening.SignalFamilyInteraction("character.draven"));
    for (const char* Family : {"character.marc", "character.denise", "character.ethan"})
    {
        CHECK(Opening.SignalFamilyInteraction(Family));
    }
    CHECK(!Opening.BeginBoardingEncounter("Encounter", 3));
    CHECK(Opening.SignalFleetDetected());
    CHECK(Opening.SignalBoardingStarted());
    CHECK(!Opening.BeginBoardingEncounter("Encounter", 1));
    CHECK(Opening.BeginBoardingEncounter("Encounter", 2));
    CHECK(!Opening.SignalDravenBoarded("Encounter"));
    CHECK(Opening.SignalBoarderDefeated("Encounter"));
    CHECK(!Opening.SignalBoarderDefeated("OtherEncounter"));
    CHECK(Opening.SignalBoarderDefeated("Encounter"));
    CHECK(!Opening.SignalBoarderDefeated("Encounter"));
    CHECK(!Opening.SignalTakingStarted());
    CHECK(!Opening.SignalEnteredWater());
}

TEST_CASE("Opening: Moran cannot be walked inland before the beach recovery")
{
    CampaignRuntime Campaign;
    OpeningRuntime Opening(Campaign);
    PlayRaid(Opening);
    CHECK(!Opening.SignalReachedLocation(OpeningLocation::DriftwoodBeach));
    CHECK(Opening.SignalEnteredWater());
    CHECK(!Opening.SignalReachedLocation(OpeningLocation::DriftwoodBeach));
    CHECK(Opening.SignalReachedOuterReef());
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodBeach));
    CHECK(!Opening.SignalReachedLocation(OpeningLocation::DriftwoodCamp));
    CHECK(!Opening.SignalReachedLocation(OpeningLocation::RexaHarbor));
    CHECK(Opening.SignalRecoveryComplete());
    CHECK(!Opening.SignalReachedLocation(OpeningLocation::MirasCove));
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodCamp));
}

TEST_CASE("Opening: full Moran route to Rexa Harbor, including save/load mid-route")
{
    CampaignRuntime Campaign;
    OpeningRuntime Opening(Campaign);
    PlayRaid(Opening);
    CHECK(Opening.SignalEnteredWater());
    CHECK(Opening.SignalReachedOuterReef());
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodBeach));
    CHECK(Opening.SignalRecoveryComplete());
    CHECK(Opening.SignalReachedLocation(OpeningLocation::DriftwoodCamp));
    CHECK(Opening.SignalReachedLocation(OpeningLocation::MirasCove));
    CHECK(!Opening.SignalCrewMet("crew.esteban"));
    CHECK(Opening.SignalCrewMet("crew.mira"));
    CHECK(Opening.SignalCrewRecruitmentAvailable("crew.mira"));
    CHECK(Opening.SignalCrewRecruited("crew.mira"));

    // Save mid-route and continue from the loaded state.
    CampaignState Decoded;
    std::vector<std::string> Errors;
    CHECK(SaveCodec::Decode(SaveCodec::Encode(Campaign.State()), Decoded, Errors));
    CampaignRuntime Loaded;
    CHECK(Loaded.LoadState(Decoded, Errors));
    OpeningRuntime Resumed(Loaded);
    CHECK(Resumed.Progress().Location == OpeningLocation::MirasCove);

    CHECK(Resumed.SignalReachedLocation(OpeningLocation::MangroveShallows));
    CHECK(Resumed.SignalCrewMet("crew.big_tom"));
    CHECK(Resumed.SignalCrewRecruitmentAvailable("crew.big_tom"));
    CHECK(Resumed.SignalCrewRecruited("crew.big_tom"));
    CHECK(Resumed.SignalReachedLocation(OpeningLocation::KoasTradingPost));
    CHECK(Resumed.SignalReachedLocation(OpeningLocation::GalleonCove));
    CHECK(Resumed.SignalCrewMet("crew.esteban"));
    CHECK(Resumed.SignalCrewRecruitmentAvailable("crew.esteban"));
    CHECK(Resumed.SignalGalleonCoveCleared());
    CHECK(Resumed.SignalLaLiberacionBoarded());
    CHECK(!Resumed.SignalLaLiberacionHelmSecured());
    CHECK(Resumed.SignalCrewRecruited("crew.esteban"));
    CHECK(Resumed.SignalLaLiberacionHelmSecured());
    CHECK(Resumed.SignalLaLiberacionHarborCleared());
    CHECK(Loaded.HasFact(Facts::LaLiberacionOwned));
    CHECK(Resumed.BeginFirstWake());
    CHECK(Resumed.ReachRexaHarbor());
    CHECK(Loaded.GetMissionState("Main.C02.03.FirstWake") == MissionState::Completed);
    CHECK(Loaded.GetMissionState("Main.C03.01.RexaHarbor") == MissionState::Available);
    CHECK(Loaded.HasFact("World.RexaEntered"));
    CHECK_EQ(Loaded.State().CurrentChapter, 3);
}
