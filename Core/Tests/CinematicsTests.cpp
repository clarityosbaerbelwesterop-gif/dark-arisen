#include "TestHarness.h"

#include "DarkArisen/Core/Cinematics.h"
#include "DarkArisen/Core/DesignLaws.h"

#include <string>

using namespace DarkArisen::Core;

namespace
{
    int CountKind(const CinematicKind Kind)
    {
        int Count = 0;
        for (const Cinematic& Entry : CinematicRegister()) Count += Entry.Kind == Kind ? 1 : 0;
        return Count;
    }

    const CinematicPlayer::FactQuery NoFacts = [](std::string_view) { return false; };
}

TEST_CASE("Cinematics: the register holds the catalog's nineteen, five inserts and the two standalone pushes")
{
    CHECK(CountKind(CinematicKind::Cutscene) == DesignLaws::AuthoredCutsceneCount);
    CHECK(CountKind(CinematicKind::Insert) == 5);
    CHECK(CountKind(CinematicKind::SlowPush) == 2);
    CHECK(static_cast<int>(ProhibitedCinematicMoments().size()) == DesignLaws::ProhibitedCutsceneMomentCount);
    for (const Cinematic& Entry : CinematicRegister())
    {
        CHECK(!IsProhibitedCinematicMoment(Entry.Id));
        CHECK(Entry.PushMetresPerSecond <= MaxPushMetresPerSecond);
        CHECK(Entry.ControlTakenSeconds <= Entry.LengthSeconds);
        CHECK(!Entry.Source.empty());
    }
}

TEST_CASE("Cinematics: a prohibited moment can never start")
{
    CinematicPlayer Player;
    for (const std::string_view Moment : ProhibitedCinematicMoments()) CHECK(Player.Start(Moment, NoFacts) == CinematicStart::Prohibited);
    CHECK(!Player.IsPlaying());
}

TEST_CASE("Cinematics: #1 follows the canon lock, Ethan is taken alive")
{
    const Cinematic* Brother = FindCinematic("Cutscene.01.TheBrother");
    CHECK(Brother != nullptr);
    if (!Brother) return;
    const std::string Summary(Brother->Summary);
    CHECK(Summary.find("taken alive") != std::string::npos);
    CHECK(Summary.find("Ethan's death") == std::string::npos);
    CHECK(Brother->Music == CinematicMusic::NoneUntilLastEightSeconds);
    CHECK(Brother->Animation == Performance::P1);
}

TEST_CASE("Cinematics: the endgame five are registered without content and cannot start")
{
    CinematicPlayer Player;
    for (int Number = 15; Number <= 19; ++Number)
    {
        const std::string Id = "Cutscene." + std::to_string(Number) + ".Endgame";
        const Cinematic* Entry = FindCinematic(Id);
        CHECK(Entry != nullptr && !Entry->Specified);
        CHECK(Player.Start(Id, NoFacts) == CinematicStart::NotSpecified);
    }
}

TEST_CASE("Cinematics: the arrow takes control for 1.5 seconds and leaves the music running")
{
    CinematicPlayer Player;
    CHECK(Player.Start("Cutscene.11.TheArrow", NoFacts) == CinematicStart::Started);
    CHECK(Player.MusicToStart() == CinematicMusic::RunningCueContinues);
    CinematicFrame Frame = Player.Tick(1.0f);
    CHECK(Frame.Playing && !Frame.PlayerHasControl);
    CHECK(Frame.Camera == CinematicCamera::Unmoving);
    CHECK_NEAR(Frame.PushDistance, 0.0f, 1e-6f);
    Frame = Player.Tick(0.6f);
    CHECK(Frame.Finished && Frame.PlayerHasControl);
    CHECK(!Player.IsPlaying());
}

TEST_CASE("Cinematics: the grove returns control after its six-second anchored wide")
{
    CinematicPlayer Player;
    CHECK(Player.Start("Cutscene.04.TheGrove", NoFacts) == CinematicStart::Started);
    CHECK(!Player.Tick(5.9f).PlayerHasControl);
    const CinematicFrame Free = Player.Tick(0.2f);
    CHECK(Free.Playing && Free.PlayerHasControl);
    CHECK(Player.Start("Cutscene.07.TheEmergence", NoFacts) == CinematicStart::AlreadyPlaying);
}

TEST_CASE("Cinematics: a slow push moves at 0.15 m/s and an insert lasts two seconds")
{
    CinematicPlayer Player;
    CHECK(Player.Start("Cutscene.12.TheLetterReal", NoFacts) == CinematicStart::Started);
    CHECK_NEAR(Player.Tick(4.0f).PushDistance, 0.6f, 1e-5f);
    CHECK(Player.Tick(4.0f).Finished);
    CHECK(Player.Start("Insert.3.TheBlackCandle", NoFacts) == CinematicStart::Started);
    CHECK(!Player.Tick(1.9f).PlayerHasControl);
    CHECK(Player.Tick(0.1f).Finished);
}

TEST_CASE("Cinematics: the boundary stone needs new game plus and the real letter")
{
    CinematicPlayer Player;
    const CinematicPlayer::FactQuery LetterRead = [](std::string_view Fact) { return Fact == "Highmoore.RealLetterRead"; };
    CHECK(Player.Start("SlowPush.5.TheBoundaryStone", LetterRead, false) == CinematicStart::ConditionUnmet);
    CHECK(Player.Start("SlowPush.5.TheBoundaryStone", NoFacts, true) == CinematicStart::ConditionUnmet);
    CHECK(Player.Start("SlowPush.5.TheBoundaryStone", LetterRead, true) == CinematicStart::Started);
}

TEST_CASE("Cinematics: silence is the norm, as section 7.1 counts it")
{
    // eleven of nineteen carry no music of their own; of the fourteen specified so far, eight do (the arrow's cue is
    // the one already running), so at most three of the five endgame sequences may be silent
    int Silent = 0;
    for (const Cinematic& Entry : CinematicRegister())
        if (Entry.Kind == CinematicKind::Cutscene && Entry.Specified)
            Silent += Entry.Music == CinematicMusic::None || Entry.Music == CinematicMusic::RunningCueContinues ? 1 : 0;
    CHECK(Silent == 8);
    CHECK(Silent <= 11);
}
