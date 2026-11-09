#include "TestHarness.h"

#include "DarkArisen/Core/Soundscape.h"

#include <string>

using namespace DarkArisen::Core;

TEST_CASE("Sound: cannon fire arrives after the light, a kilometre in about three seconds (2.1)")
{
    CHECK_NEAR(ArrivalDelaySeconds(343.0f), 1.0f, 1e-6f);
    CHECK_NEAR(ArrivalDelaySeconds(1000.0f), 2.915f, 0.01f);
    CHECK_NEAR(ArrivalDelaySeconds(-5.0f), 0.0f, 1e-6f);
}

TEST_CASE("Sound: gain halves with every doubling and stops at the source's range (2.2)")
{
    CHECK_NEAR(DistanceGain(SoundReach::Speech, 2.0f), 0.5f, 1e-6f);
    CHECK_NEAR(DistanceGain(SoundReach::Speech, 4.0f), 0.25f, 1e-6f);
    CHECK_NEAR(DistanceGain(SoundReach::Whisper, 2.5f), 0.4f, 1e-6f);
    CHECK_NEAR(DistanceGain(SoundReach::Whisper, 4.0f), 0.0f, 1e-6f);
    CHECK_NEAR(DistanceGain(SoundReach::Shout, 60.0f), 0.0f, 1e-6f);
    CHECK(DistanceGain(SoundReach::Cannon, 1900.0f) > 0.0f);
    CHECK_NEAR(AudibleRangeMetres(SoundReach::Eruption), 50000.0f, 1e-3f);
}

TEST_CASE("Sound: stone muffles more than wood, wood more than cloth (2.4)")
{
    CHECK(MufflingThrough(Barrier::Stone).Gain < MufflingThrough(Barrier::Wood).Gain);
    CHECK(MufflingThrough(Barrier::Wood).Gain < MufflingThrough(Barrier::Cloth).Gain);
    CHECK(MufflingThrough(Barrier::Stone).LowPassHertz < MufflingThrough(Barrier::Wood).LowPassHertz);
    CHECK_NEAR(MufflingThrough(Barrier::None).Gain, 1.0f, 1e-6f);
}

TEST_CASE("Sound: reverb times by space, the cathedral's five seconds (4.3)")
{
    CHECK_NEAR(ReverbSeconds(AcousticSpace::Cathedral), 5.0f, 1e-6f);
    CHECK(ReverbSeconds(AcousticSpace::Cave) >= 3.0f && ReverbSeconds(AcousticSpace::Cave) <= 5.0f);
    CHECK_NEAR(ReverbSeconds(AcousticSpace::SmallRoom), 0.5f, 1e-6f);
    CHECK(ReverbSeconds(AcousticSpace::LargeRoom) >= 1.0f && ReverbSeconds(AcousticSpace::LargeRoom) <= 2.0f);
    CHECK_NEAR(ReverbSeconds(AcousticSpace::OpenOcean), 0.0f, 1e-6f);
}

TEST_CASE("Sound: every region but Highmoore has its document's soundscape (5)")
{
    bool Howlers = false;
    for (const AmbienceLayer& Layer : AmbienceFor(Region::Rexa))
        Howlers = Howlers || (Layer.Id == "Ambience.Caribbean.HowlerMonkeys" && Layer.When == DayPart::Dawn);
    CHECK(Howlers);
    CHECK(AmbienceFor(Region::QuietCoast).size() == 6);
    CHECK(AmbienceFor(Region::TheMaw).size() == 2);
    CHECK(AmbienceFor(Region::OpenSea).size() == 4);
    CHECK(AmbienceFor(Region::Ashenmoor).size() == 4);
    CHECK(AmbienceFor(Region::Highmoore).empty());
}

TEST_CASE("Sound: a footstep for every surface, cloth the quietest (traversal.md 2.3)")
{
    for (int S = 0; S <= static_cast<int>(Surface::WetDeck); ++S)
    {
        const Footstep Step = FootstepFor(static_cast<Surface>(S));
        CHECK(std::string(Step.Event).rfind("Footstep.", 0) == 0);
        CHECK(Step.Loudness >= FootstepFor(Surface::Cloth).Loudness);
    }
    CHECK(FootstepFor(Surface::Stone).Character == "Sharp heel-toe clicks");
}

TEST_CASE("Sound: music runs its length and nothing cuts in (cutscene catalog 7.2)")
{
    MusicDirector Music;
    CHECK(Music.Start("Cue.Highmoore.RainDance", 90.0f, ScoreContext::World));
    CHECK(!Music.Start("Cue.Other", 10.0f, ScoreContext::World));
    Music.Tick(60.0f);
    CHECK(Music.IsPlaying());
    CHECK(Music.GetCue() == "Cue.Highmoore.RainDance");
    Music.Tick(30.0f);
    CHECK(!Music.IsPlaying());
    CHECK(!Music.Start("Cue.Dungeon", 30.0f, ScoreContext::Dungeon));
    CHECK(!Music.Start("Cue.Belos", 30.0f, ScoreContext::Belos));
    CHECK(Music.Start("Cue.Battle.Opening", 20.0f, ScoreContext::Battle));
}
