#include "TestHarness.h"

#include "DarkArisen/Core/Facts.h"
#include "DarkArisen/Core/LevelSpawn.h"
#include "DarkArisen/Core/Presentation.h"

#include <optional>

using namespace DarkArisen::Core;

namespace
{
    PresentationBeat Beat(const double Time, const BeatKind Kind, std::string Text, std::string Speaker = {})
    {
        PresentationBeat Result;
        Result.Time = Time;
        Result.Kind = Kind;
        Result.Text = std::move(Text);
        Result.Speaker = std::move(Speaker);
        return Result;
    }

    /** The three sequences of ContentSource/Opening/HarlowOpeningPresentation.json. */
    std::vector<PresentationSequence> HarlowPresentation()
    {
        PresentationSequence Sails{"Cinematic.Opening.BlackSailsSighted", 6.5, "brief_hold", "Camera.BlackSails", {
            Beat(0.0, BeatKind::Shot, "Jake deck eyeline through rigging"),
            Beat(1.2, BeatKind::Shot, "distant black-sail silhouette breaks fog bank"),
            Beat(3.1, BeatKind::Subtitle, "Below deck. Now.", "Marc"),
            Beat(4.4, BeatKind::Subtitle, "That's not a customs flag.", "Ethan"),
            Beat(6.2, BeatKind::Callback, "FleetPresentationComplete")}};
        PresentationSequence Draven{"Presentation.Opening.DravenBoards", 3.8, "brief_hold", "Draven.Arrival", {
            Beat(0.0, BeatKind::Shot, "boots hit quarterdeck"),
            Beat(1.0, BeatKind::Shot, "Draven silhouette clears smoke"),
            Beat(2.2, BeatKind::Subtitle, "Harlow. We need your routes.", "Draven"),
            Beat(3.6, BeatKind::Callback, "ReadyForTaking")}};
        PresentationSequence Taking{"Cinematic.Opening.TheTaking", 18.0, "authored_takeover", "", {
            Beat(0.0, BeatKind::Actors, ""),
            Beat(3.0, BeatKind::Subtitle, "You leave my family out of this.", "Marc"),
            Beat(5.6, BeatKind::Event, "MarcKilled"),
            Beat(8.1, BeatKind::Event, "DeniseKilled"),
            Beat(10.0, BeatKind::Subtitle, "Jake—don't. Stay back.", "Ethan"),
            Beat(11.0, BeatKind::Event, "EthanSeizedAlive"),
            Beat(14.2, BeatKind::Event, "JakeForcedToOverboard"),
            Beat(17.8, BeatKind::Callback, "TakingPresentationComplete")}};
        return {Sails, Draven, Taking};
    }

    struct Harness
    {
        CampaignRuntime Campaign;
        std::optional<OpeningRuntime> Opening;
        std::optional<OpeningDirector> Director;
        int Released = 0;
        int Subtitles = 0;
        bool Overboard = false;
        std::vector<std::pair<std::string, OpeningDirector::Fate>> Fates;

        explicit Harness(std::vector<PresentationSequence> Sequences = HarlowPresentation())
        {
            OpeningRuntime::Events Events;
            Events.CinematicRequested = [this](const std::string_view Id) { if (Director) Director->OnCinematicRequested(Id); };
            Opening.emplace(Campaign, Events);
            OpeningDirector::Events Listeners;
            Listeners.ReleaseBoarders = [this](const int Remaining) { Released += Remaining; };
            Listeners.Subtitle = [this](const PresentationBeat&) { ++Subtitles; };
            Listeners.JakeForcedOverboard = [this]() { Overboard = true; };
            Listeners.FamilyFate = [this](const std::string_view Id, const OpeningDirector::Fate What) { Fates.emplace_back(std::string(Id), What); };
            Director.emplace(*Opening, std::move(Sequences), OpeningDirector::Encounter{}, Listeners);
        }

        void Run(const double Seconds, const double Step)
        {
            for (double Time = 0.0; Time < Seconds; Time += Step) Director->Tick(Step);
        }
    };
}

TEST_CASE("Presentation: Harlow timeline from ContentSource validates")
{
    std::vector<std::string> Errors;
    CHECK(ValidatePresentation(HarlowPresentation(), Errors));
    CHECK(Errors.empty());
}

TEST_CASE("Presentation: invalid timelines are rejected and never drive the story")
{
    std::vector<PresentationSequence> Broken = HarlowPresentation();
    Broken[0].Beats[3].Time = 9.0;  // beyond the 6.5 s duration
    Harness Test(Broken);
    CHECK(!Test.Director->Errors().empty());
    CHECK(!Test.Director->OnCinematicRequested("Cinematic.Opening.BlackSailsSighted"));
}

TEST_CASE("Opening director: the full Harlow raid runs from the authored presentation")
{
    for (const double Step : {1.0 / 60.0, 1.0 / 30.0, 0.5})
    {
        Harness Test;
        for (const char* Family : {"character.marc", "character.denise", "character.ethan"})
        {
            CHECK(Test.Opening->SignalFamilyInteraction(Family));
        }
        CHECK(Test.Opening->SignalFleetDetected());  // the cargo manifest beat
        CHECK(Test.Director->IsPresenting());
        Test.Run(7.0, Step);
        CHECK(Test.Opening->Progress().RaidState == OpeningRaidState::FirstEncounter);
        CHECK_EQ(Test.Released, 3);

        // The deck is cleared by combat; Draven boards on his own once it is.
        for (int Index = 0; Index < 3; ++Index)
        {
            CHECK(Test.Opening->SignalBoarderDefeated("Encounter.HarlowRaid.MainDeck"));
        }
        Test.Run(0.1, Step);
        CHECK(Test.Opening->Progress().RaidState == OpeningRaidState::DravenAboard);
        Test.Run(4.0, Step);
        CHECK(Test.Opening->Progress().RaidState == OpeningRaidState::Taking);
        Test.Run(18.5, Step);

        CHECK(Test.Opening->Progress().RaidState == OpeningRaidState::Overboard);
        CHECK(Test.Campaign.GetMissionState("Main.C01.03.TheTaking") == MissionState::Completed);
        CHECK(Test.Campaign.GetMissionState("Main.C01.04.Undertow") == MissionState::Active);
        CHECK(Test.Campaign.HasFact(Facts::EthanAbducted));
        CHECK(!Test.Campaign.HasFact(Facts::EthanRecovered));
        CHECK(Test.Overboard);
        CHECK_EQ(Test.Subtitles, 5);
        CHECK_EQ(Test.Fates.size(), std::size_t{3});
        for (const auto& [Id, What] : Test.Fates)
        {
            // Canon: real Ethan is taken alive; only Marc and Denise die in the Taking.
            CHECK((Id == "character.ethan") == (What == OpeningDirector::Fate::SeizedAlive));
        }
        // The overboard presentation is requested by the runtime but not authored in ContentSource.
        CHECK_EQ(Test.Director->Errors().size(), std::size_t{1});
        CHECK(Test.Director->Errors()[0].find("Cinematic.Opening.JakeOverboard") != std::string::npos);
    }
}

TEST_CASE("Opening director: unknown callbacks are rejected without advancing the raid")
{
    std::vector<PresentationSequence> Sequences = HarlowPresentation();
    Sequences[0].Beats[4].Text = "SkipToTheTaking";
    Harness Test(Sequences);
    for (const char* Family : {"character.marc", "character.denise", "character.ethan"})
    {
        Test.Opening->SignalFamilyInteraction(Family);
    }
    Test.Opening->SignalFleetDetected();
    Test.Run(7.0, 1.0 / 60.0);
    CHECK(Test.Opening->Progress().RaidState == OpeningRaidState::FleetDetected);
    CHECK_EQ(Test.Released, 0);
    CHECK_EQ(Test.Director->Errors().size(), std::size_t{1});
}

TEST_CASE("Opening director: a raid interrupted mid-fight resumes with the remaining boarders")
{
    Harness Before;
    for (const char* Family : {"character.marc", "character.denise", "character.ethan"})
    {
        Before.Opening->SignalFamilyInteraction(Family);
    }
    Before.Opening->SignalFleetDetected();
    Before.Run(7.0, 1.0 / 60.0);
    CHECK(Before.Opening->SignalBoarderDefeated("Encounter.HarlowRaid.MainDeck"));

    Harness After;
    std::vector<std::string> LoadErrors;
    CHECK(After.Campaign.LoadState(Before.Campaign.State(), LoadErrors));
    After.Opening->RestoreProgress();
    After.Director->Resume();
    CHECK_EQ(After.Released, 2);
    for (int Index = 0; Index < 2; ++Index)
    {
        CHECK(After.Opening->SignalBoarderDefeated("Encounter.HarlowRaid.MainDeck"));
    }
    After.Run(0.1, 1.0 / 60.0);
    CHECK(After.Opening->Progress().RaidState == OpeningRaidState::DravenAboard);
}

TEST_CASE("Level spawn: checkpoint, then overboard arrival, then the level arrival")
{
    const std::vector<SpawnPoint> Driftwood = {
        {"Spawn.Moran.DriftwoodBeach.Arrival", SpawnRule::Arrival},
        {"WaterEntry", SpawnRule::OpeningOverboard},
        {"Spawn.Moran.DriftwoodBeach.Recovery", SpawnRule::Checkpoint}};
    CampaignState State = CampaignState::NewGame();
    CHECK_EQ(ResolveSpawn(State, Driftwood), 0);
    State.Opening.RecoveryState = WaterRecoveryState::Overboard;
    CHECK_EQ(ResolveSpawn(State, Driftwood), 1);  // arriving over the rail: in the water, not on the beach
    State.Opening.RecoveryState = WaterRecoveryState::Recovered;
    State.SpawnId = "Spawn.Moran.DriftwoodBeach.Recovery";
    CHECK_EQ(ResolveSpawn(State, Driftwood), 2);
    State.SpawnId = "Spawn.Elsewhere";
    CHECK_EQ(ResolveSpawn(State, Driftwood), 0);
    CHECK_EQ(ResolveSpawn(State, {}), -1);
    CHECK_EQ(ResolveSpawn(State, {{"", SpawnRule::Arrival}}), -1);
}
