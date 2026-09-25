#include "DarkArisen/Core/Presentation.h"

#include <cmath>
#include <set>
#include <utility>

namespace DarkArisen::Core
{
    bool ValidatePresentation(const std::vector<PresentationSequence>& Sequences, std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        std::set<std::string> Ids;
        for (const PresentationSequence& Sequence : Sequences)
        {
            if (Sequence.Id.empty() || !Ids.insert(Sequence.Id).second)
            {
                OutErrors.push_back("presentation id empty or duplicated: '" + Sequence.Id + "'");
            }
            if (!std::isfinite(Sequence.Duration) || Sequence.Duration <= 0.0)
            {
                OutErrors.push_back(Sequence.Id + ": duration must be positive");
            }
            double Previous = 0.0;
            for (const PresentationBeat& Beat : Sequence.Beats)
            {
                if (!std::isfinite(Beat.Time) || Beat.Time < Previous || Beat.Time > Sequence.Duration)
                {
                    OutErrors.push_back(Sequence.Id + ": beat times must be sorted within the duration");
                }
                if (Beat.Kind != BeatKind::Actors && Beat.Text.empty())
                {
                    OutErrors.push_back(Sequence.Id + ": beat without content");
                }
                Previous = std::isfinite(Beat.Time) ? Beat.Time : Previous;
            }
        }
        return OutErrors.size() == Before;
    }

    void PresentationPlayer::Start(const PresentationSequence& Sequence)
    {
        Active = &Sequence;
        Elapsed = 0.0;
        NextBeat = 0;
    }

    void PresentationPlayer::Stop()
    {
        Active = nullptr;
        Elapsed = 0.0;
        NextBeat = 0;
    }

    void PresentationPlayer::Tick(const double DeltaSeconds, const std::function<void(const PresentationBeat&)>& OnBeat)
    {
        if (!Active || !std::isfinite(DeltaSeconds) || DeltaSeconds < 0.0)
        {
            return;
        }
        Elapsed += DeltaSeconds;
        // A beat may stop or restart the player (a callback can request the next presentation).
        const PresentationSequence* Playing = Active;
        while (Active == Playing && NextBeat < Playing->Beats.size() && Playing->Beats[NextBeat].Time <= Elapsed)
        {
            const PresentationBeat& Beat = Playing->Beats[NextBeat++];
            if (OnBeat) OnBeat(Beat);
        }
        if (Active == Playing && Elapsed >= Playing->Duration && NextBeat >= Playing->Beats.size())
        {
            Stop();
        }
    }

    OpeningDirector::OpeningDirector(OpeningRuntime& InOpening, std::vector<PresentationSequence> InSequences, Encounter InConfig,
                                     Events InListeners)
        : Opening(InOpening), Sequences(std::move(InSequences)), Config(std::move(InConfig)), Listeners(std::move(InListeners))
    {
        std::vector<std::string> Problems;
        if (!ValidatePresentation(Sequences, Problems))
        {
            for (std::string& Problem : Problems) Reject(std::move(Problem));
            Sequences.clear();  // fail closed: an invalid timeline never drives the story
        }
    }

    const PresentationSequence* OpeningDirector::Find(const std::string_view Id) const
    {
        for (const PresentationSequence& Sequence : Sequences)
        {
            if (Sequence.Id == Id) return &Sequence;
        }
        return nullptr;
    }

    void OpeningDirector::Reject(std::string Message)
    {
        Rejected.push_back(std::move(Message));
    }

    bool OpeningDirector::OnCinematicRequested(const std::string_view Id)
    {
        const PresentationSequence* Sequence = Find(Id);
        if (!Sequence)
        {
            Reject("no authored presentation for " + std::string(Id));
            return false;
        }
        Player.Start(*Sequence);
        return true;
    }

    void OpeningDirector::Resume()
    {
        const OpeningProgress& Progress = Opening.Progress();
        switch (Progress.RaidState)
        {
        case OpeningRaidState::FleetDetected: OnCinematicRequested("Cinematic.Opening.BlackSailsSighted"); break;
        case OpeningRaidState::FirstEncounter:
            if (Listeners.ReleaseBoarders && Progress.BoardersDefeated < Progress.BoardersRequired)
            {
                Listeners.ReleaseBoarders(Progress.BoardersRequired - Progress.BoardersDefeated);
            }
            break;
        case OpeningRaidState::DravenAboard: OnCinematicRequested("Presentation.Opening.DravenBoards"); break;
        case OpeningRaidState::Taking: OnCinematicRequested("Cinematic.Opening.TheTaking"); break;
        default: break;
        }
    }

    void OpeningDirector::HandleBeat(const PresentationBeat& Beat)
    {
        switch (Beat.Kind)
        {
        case BeatKind::Subtitle:
            if (Listeners.Subtitle) Listeners.Subtitle(Beat);
            return;
        case BeatKind::Shot:
        case BeatKind::Actors:
            return;  // camera and staging belong to the presentation layer
        case BeatKind::Event:
            if (Beat.Text == "MarcKilled") { if (Listeners.FamilyFate) Listeners.FamilyFate("character.marc", Fate::Killed); }
            else if (Beat.Text == "DeniseKilled") { if (Listeners.FamilyFate) Listeners.FamilyFate("character.denise", Fate::Killed); }
            else if (Beat.Text == "EthanSeizedAlive") { if (Listeners.FamilyFate) Listeners.FamilyFate("character.ethan", Fate::SeizedAlive); }
            else if (Beat.Text == "JakeForcedToOverboard") { if (Listeners.JakeForcedOverboard) Listeners.JakeForcedOverboard(); }
            else Reject("unknown presentation event " + Beat.Text);
            return;
        case BeatKind::Callback:
            if (Beat.Text == "FleetPresentationComplete")
            {
                if (!Opening.SignalBoardingStarted() || !Opening.BeginBoardingEncounter(Config.Id, Config.RequiredBoarders))
                {
                    Reject("boarding could not start from FleetPresentationComplete");
                    return;
                }
                if (Listeners.ReleaseBoarders) Listeners.ReleaseBoarders(Config.RequiredBoarders);
            }
            else if (Beat.Text == "ReadyForTaking")
            {
                if (!Opening.SignalTakingStarted()) Reject("ReadyForTaking rejected by the opening runtime");
            }
            else if (Beat.Text == "TakingPresentationComplete")
            {
                if (!Opening.SignalTakingCinematicComplete()) Reject("TakingPresentationComplete rejected by the opening runtime");
            }
            else
            {
                Reject("unknown presentation callback " + Beat.Text);
            }
            return;
        }
    }

    void OpeningDirector::Tick(const double DeltaSeconds)
    {
        Player.Tick(DeltaSeconds, [this](const PresentationBeat& Beat) { HandleBeat(Beat); });

        // Draven comes aboard once the deck is cleared (Unreal: SignalDravenArrived, never wired).
        const OpeningProgress& Progress = Opening.Progress();
        if (Progress.RaidState == OpeningRaidState::FirstEncounter && Progress.BoardingEncounterId == Config.Id &&
            Progress.BoardersRequired >= 2 && Progress.BoardersDefeated >= Progress.BoardersRequired && !Player.IsPlaying() &&
            !DravenRejected)
        {
            if (!Opening.SignalDravenBoarded(Config.Id))
            {
                DravenRejected = true;  // report once; the campaign refused the commit
                Reject("Draven boarding rejected by the opening runtime");
            }
        }
    }
}
