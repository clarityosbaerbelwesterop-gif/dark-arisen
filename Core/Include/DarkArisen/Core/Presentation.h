#pragma once

#include "DarkArisen/Core/OpeningRuntime.h"

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** One timed beat of an authored presentation (ContentSource/Opening presentation JSON). */
    enum class BeatKind : std::uint8_t { Shot, Subtitle, Event, Callback, Actors };

    struct PresentationBeat
    {
        double Time = 0.0;
        BeatKind Kind = BeatKind::Shot;
        std::string Speaker;  // subtitles only
        std::string Text;     // shot description, subtitle line, event or callback name
    };

    struct PresentationSequence
    {
        std::string Id;
        double Duration = 0.0;
        std::string PlayerControl;  // brief_hold, authored_takeover
        std::string Camera;         // layout anchor, may be empty
        std::vector<PresentationBeat> Beats;
    };

    /** Rejects empty or duplicate ids, non-finite or out-of-range times and unsorted beats. */
    bool ValidatePresentation(const std::vector<PresentationSequence>& Sequences, std::vector<std::string>& OutErrors);

    /** Plays one sequence; every beat fires exactly once, in order, whatever the tick length. */
    class PresentationPlayer
    {
    public:
        void Start(const PresentationSequence& Sequence);
        void Stop();
        bool IsPlaying() const { return Active != nullptr; }
        const PresentationSequence* Current() const { return Active; }
        /** Fires beats due up to the new time; the sequence ends once its duration has elapsed. */
        void Tick(double DeltaSeconds, const std::function<void(const PresentationBeat&)>& OnBeat);

    private:
        const PresentationSequence* Active = nullptr;
        double Elapsed = 0.0;
        std::size_t NextBeat = 0;
    };

    /**
     * Chapter 1 raid orchestration: plays the presentation the opening runtime requests and turns
     * its callbacks into the gated story signals. It also releases the boarders and reports the
     * family's fate to the level. Unknown callbacks and events are rejected and recorded. Real
     * Ethan is only ever seized alive; there is no beat that can harm him.
     */
    class OpeningDirector
    {
    public:
        struct Encounter
        {
            std::string Id = "Encounter.HarlowRaid.MainDeck";
            int RequiredBoarders = 3;
        };

        enum class Fate : std::uint8_t { Killed, SeizedAlive };

        struct Events
        {
            /** The boarders still required by the encounter must become active now. */
            std::function<void(int Remaining)> ReleaseBoarders;
            std::function<void(std::string_view CharacterId, Fate What)> FamilyFate;
            std::function<void()> JakeForcedOverboard;
            std::function<void(const PresentationBeat&)> Subtitle;
        };

        OpeningDirector(OpeningRuntime& Opening, std::vector<PresentationSequence> Sequences, Encounter Config, Events Listeners);

        /** Connect to OpeningRuntime::Events::CinematicRequested. False when nothing is authored for Id. */
        bool OnCinematicRequested(std::string_view Id);
        /**
         * Re-enters an interrupted raid after a load: replays the presentation owed by the saved
         * state, or releases the boarders still to be defeated.
         */
        void Resume();
        void Tick(double DeltaSeconds);

        bool IsPresenting() const { return Player.IsPlaying(); }
        const std::vector<std::string>& Errors() const { return Rejected; }

    private:
        OpeningRuntime& Opening;
        std::vector<PresentationSequence> Sequences;
        Encounter Config;
        Events Listeners;
        PresentationPlayer Player;
        std::vector<std::string> Rejected;
        bool DravenRejected = false;

        const PresentationSequence* Find(std::string_view Id) const;
        void HandleBeat(const PresentationBeat& Beat);
        void Reject(std::string Message);
    };
}
