#include "DarkArisen/Core/Soundscape.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace DarkArisen::Core
{
    float ArrivalDelaySeconds(const float DistanceMetres) { return std::max(0.0f, DistanceMetres) / SpeedOfSoundMetresPerSecond; }

    float AudibleRangeMetres(const SoundReach Reach)
    {
        switch (Reach)
        {
        case SoundReach::Whisper: return 3.0f;
        case SoundReach::Speech: return 15.0f;
        case SoundReach::Shout: return 50.0f;
        case SoundReach::Cannon: return 2000.0f;
        case SoundReach::Eruption: default: return 50000.0f;
        }
    }

    float DistanceGain(const SoundReach Reach, const float DistanceMetres)
    {
        if (DistanceMetres > AudibleRangeMetres(Reach)) return 0.0f;
        return DistanceMetres <= 1.0f ? 1.0f : 1.0f / DistanceMetres;
    }

    Muffling MufflingThrough(const Barrier Between)
    {
        switch (Between)
        {
        case Barrier::Cloth: return {0.8f, 8000.0f};
        case Barrier::Wood: return {0.5f, 2500.0f};
        case Barrier::Stone: return {0.15f, 600.0f};
        case Barrier::None: default: return {};
        }
    }

    float ReverbSeconds(const AcousticSpace Space)
    {
        switch (Space)
        {
        case AcousticSpace::OpenField: return 0.2f;   // minimal echo, the sound dissipates
        case AcousticSpace::Forest: return 0.3f;      // muffled, dampened
        case AcousticSpace::SmallRoom: return 0.5f;
        case AcousticSpace::LargeRoom: return 1.5f;
        case AcousticSpace::Cellar: return 1.2f;      // moderate echo
        case AcousticSpace::Cave: return 4.0f;
        case AcousticSpace::Cathedral: return 5.0f;
        case AcousticSpace::OpenOcean: default: return 0.0f;  // no echo
        }
    }

    namespace
    {
        constexpr std::array<AmbienceLayer, 4> Caribbean = {{
            {"Ambience.Caribbean.HowlerMonkeys", "Howler monkey symphony at dawn", DayPart::Dawn, false},
            {"Ambience.Caribbean.Macaws", "Macaw flights", DayPart::Day, true},
            {"Ambience.Caribbean.WavesOnCoral", "Ocean waves on coral", DayPart::Any, false},
            {"Ambience.Caribbean.TropicalInsects", "Tropical insects", DayPart::Any, false},
        }};
        // the Quiet Coast is the document's Northern Atlantic (Fjordlund, 5.2) with the Quiet Coast's birdsong (5.4)
        constexpr std::array<AmbienceLayer, 6> QuietCoast = {{
            {"Ambience.Fjordlund.Seabirds", "Seabird cries", DayPart::Day, false},
            {"Ambience.Fjordlund.DistantWhales", "Whale songs, distant", DayPart::Any, true},
            {"Ambience.Fjordlund.WindInPines", "Wind through pines", DayPart::Any, false},
            {"Ambience.Fjordlund.RockyShore", "Waves on a rocky shore", DayPart::Any, false},
            {"Ambience.Fjordlund.IceCracking", "Ice cracking", DayPart::Any, true},
            {"Ambience.QuietCoast.Birdsong", "Familiar temperate bird songs", DayPart::Day, false},
        }};
        constexpr std::array<AmbienceLayer, 4> Ashenmoor = {{
            {"Ambience.Ashenmoor.VolcanicWind", "Wind through volcanic features", DayPart::Any, false},
            {"Ambience.Ashenmoor.DistantRumbles", "Distant rumbles", DayPart::Any, true},
            {"Ambience.Ashenmoor.SandWhisper", "Sand whisper", DayPart::Any, false},
            {"Ambience.Ashenmoor.CulturalDrums", "Cultural drums, sometimes", DayPart::Any, true},
        }};
        constexpr std::array<AmbienceLayer, 2> Maw = {{
            {"Ambience.Maw.ImpossibleSounds", "Impossible sounds", DayPart::Any, true},
            {"Ambience.Maw.SourcelessWhispers", "Whispers without source", DayPart::Any, true},
        }};
        constexpr std::array<AmbienceLayer, 4> Ocean = {{
            {"Ambience.Ocean.Waves", "Waves, constant", DayPart::Any, false},
            {"Ambience.Ocean.WindInRigging", "Wind through rigging", DayPart::Any, false},
            {"Ambience.Ocean.ShipCreaking", "Ship creaking", DayPart::Any, false},
            {"Ambience.Ocean.DistantWhales", "Distant whales", DayPart::Any, true},
        }};
    }

    std::span<const AmbienceLayer> AmbienceFor(const Region Where)
    {
        switch (Where)
        {
        case Region::Moran:
        case Region::Rexa:
        case Region::Mire: return Caribbean;
        case Region::QuietCoast: return QuietCoast;
        case Region::Ashenmoor: return Ashenmoor;
        case Region::TheMaw: return Maw;
        case Region::OpenSea:
        case Region::FortCarrion: return Ocean;
        case Region::Highmoore: default: return {};
        }
    }

    Footstep FootstepFor(const Surface Ground)
    {
        switch (Ground)
        {
        case Surface::Stone: return {"Footstep.Stone", "Sharp heel-toe clicks", 1.0f};
        case Surface::Wood:
        case Surface::WetDeck: return {"Footstep.Wood", "Deep wooden thuds", 0.9f};
        case Surface::Sand: return {"Footstep.Sand", "Soft crunching", 0.55f};
        case Surface::Mud: return {"Footstep.Mud", "Wet squelching", 0.7f};
        case Surface::ShallowWater: return {"Footstep.ShallowWater", "Water displacement", 0.85f};
        case Surface::Snow:
        case Surface::DeepSnow: return {"Footstep.Snow", "Compacting snow", 0.6f};
        case Surface::Ice: return {"Footstep.Ice", "Slight sliding", 0.5f};
        case Surface::Grass:
        case Surface::DeepGrass: return {"Footstep.Grass", "Muffled footsteps", 0.45f};
        case Surface::Metal: return {"Footstep.Metal", "Clanging", 1.0f};
        case Surface::Cloth: default: return {"Footstep.Cloth", "Quiet", 0.2f};
        }
    }

    bool ScoreAllowed(const ScoreContext Context)
    {
        // dungeons carry no score; Belos and Arion Falls are total silence (`highmoore music.md` 1.5, 8)
        return Context != ScoreContext::Dungeon && Context != ScoreContext::Belos && Context != ScoreContext::ArionFalls;
    }

    bool MusicDirector::Start(const std::string_view CueId, const float LengthSeconds, const ScoreContext Context)
    {
        if (IsPlaying() || !ScoreAllowed(Context) || CueId.empty() || LengthSeconds <= 0.0f) return false;
        Cue = CueId;
        Remaining = LengthSeconds;
        return true;
    }

    void MusicDirector::Tick(const float RealDeltaSeconds) { Remaining = std::max(0.0f, Remaining - std::max(0.0f, RealDeltaSeconds)); }
}
