#pragma once

#include "DarkArisen/Core/Traversal.h"

#include <cstdint>
#include <span>
#include <string_view>

/**
 * The world's sound rules after docs/design/physics/sound_physics.md: the speed of sound (2.1), distance attenuation
 * and audible ranges (2.2), muffling through materials (2.4), acoustic spaces and reverb times (4.1, 4.3, with
 * `highmoore music.md` 8's five-second cathedral), and the regional soundscapes (5). Footsteps per surface follow
 * traversal.md 2.3's audio column. Music obeys `cutscene catalog.md` 7.2: it starts, runs its length and stops; it
 * never reacts to events, so the director below has no stop, duck or layer.
 *
 * The documents describe muffling and footsteps qualitatively; the gains and filters here are tuning values chosen to
 * that description and are marked as such.
 */
namespace DarkArisen::Core
{
    inline constexpr float SpeedOfSoundMetresPerSecond = 343.0f;
    /** 2.1: "see cannon fire, hear sound after delay"; three seconds is about a kilometre. */
    float ArrivalDelaySeconds(float DistanceMetres);

    enum class SoundReach : std::uint8_t { Whisper, Speech, Shout, Cannon, Eruption };
    /** 2.2: whisper 3 m, speech 15 m, shout 50 m, cannon 2 km, volcanic eruption 50 km. */
    float AudibleRangeMetres(SoundReach Reach);
    /** 2.2: halved with every doubling of distance (from 1 m), and silent beyond the source's audible range. */
    float DistanceGain(SoundReach Reach, float DistanceMetres);

    /** 2.4: cloth muffles lightly, wood lets some through, stone muffles heavily (tuning values). */
    enum class Barrier : std::uint8_t { None, Cloth, Wood, Stone };
    struct Muffling
    {
        float Gain = 1.0f;
        float LowPassHertz = 20000.0f;
    };
    Muffling MufflingThrough(Barrier Between);

    /** 4.1 acoustic spaces. */
    enum class AcousticSpace : std::uint8_t { OpenField, Forest, SmallRoom, LargeRoom, Cellar, Cave, Cathedral, OpenOcean };
    /** 4.3: short 0.5 s (small rooms), medium 1-2 s, long 3-5 s (caves, cathedrals); the cathedral's is five seconds. */
    float ReverbSeconds(AcousticSpace Space);

    enum class DayPart : std::uint8_t { Any, Dawn, Day, Dusk, Night };

    struct AmbienceLayer
    {
        std::string_view Id;
        std::string_view Description;
        DayPart When = DayPart::Any;
        bool Occasional = false;  // "sometimes": not a constant bed
    };
    /** Section 5's layers for a region. Highmoore has none in the sound document: its silence is `highmoore music.md`'s. */
    std::span<const AmbienceLayer> AmbienceFor(Region Where);

    struct Footstep
    {
        std::string_view Event;
        std::string_view Character;  // traversal.md 2.3's description of the sound
        float Loudness = 1.0f;       // relative (tuning value): carpet and cloth are quiet, stone is sharp
    };
    Footstep FootstepFor(Surface Ground);

    /** Where score may exist at all (`cutscene catalog.md` 7.1). */
    enum class ScoreContext : std::uint8_t { World, Cutscene, BossFight, Battle, Dungeon, Belos, ArionFalls };
    bool ScoreAllowed(ScoreContext Context);

    /**
     * One running cue at a time. A cue starts, runs its full length and stops by itself; a second cue cannot cut in
     * and nothing can stop or alter the one playing ("the arrow lands and the cue does not notice").
     */
    class MusicDirector
    {
    public:
        bool Start(std::string_view CueId, float LengthSeconds, ScoreContext Context);
        void Tick(float RealDeltaSeconds);
        bool IsPlaying() const { return Remaining > 0.0f; }
        std::string_view GetCue() const { return IsPlaying() ? Cue : std::string_view(); }

    private:
        std::string_view Cue;
        float Remaining = 0.0f;
    };
}
