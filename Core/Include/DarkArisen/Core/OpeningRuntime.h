#pragma once

#include "DarkArisen/Core/CampaignRuntime.h"

#include <functional>
#include <string_view>

namespace DarkArisen::Core
{
    /**
     * Native Chapter 1/2 orchestration: Harlow merchant ship -> Draven raid -> The Taking ->
     * overboard -> open water -> Outer Reef -> Driftwood Beach -> Moran route -> Galleon Cove ->
     * La Liberacion -> open sea -> Rexa Harbor. Port of UOpeningRuntimeComponent.
     *
     * Level geometry reports authored physical contacts (triggers, interactions, defeats)
     * through the Signal* calls. Every step is gated on the previous one and on the campaign
     * mission state; progress is committed to the campaign so it survives map travel.
     */
    class OpeningRuntime
    {
    public:
        struct Events
        {
            std::function<void(OpeningLocation Previous, OpeningLocation Current)> LocationChanged;
            std::function<void(std::string_view CinematicId)> CinematicRequested;
            std::function<void(std::string_view JournalId)> JournalObservation;
        };

        explicit OpeningRuntime(CampaignRuntime& Campaign, Events Listeners = {});

        /** Re-reads committed progress, e.g. after load or map travel. */
        void RestoreProgress();
        const OpeningProgress& Progress() const { return Current; }

        bool SignalFamilyInteraction(std::string_view CharacterId);
        bool SignalFleetDetected();
        bool SignalBoardingStarted();
        bool BeginBoardingEncounter(std::string_view EncounterId, int RequiredBoarders);
        bool SignalBoarderDefeated(std::string_view EncounterId);
        bool SignalDravenBoarded(std::string_view EncounterId);
        bool SignalTakingStarted();
        bool SignalTakingCinematicComplete();
        bool SignalEnteredWater();
        bool SignalReachedOuterReef();
        bool SignalReachedLocation(OpeningLocation NewLocation);
        bool SignalRecoveryComplete();
        bool SignalCrewMet(std::string_view CrewId);
        bool SignalCrewRecruitmentAvailable(std::string_view CrewId);
        bool SignalCrewRecruited(std::string_view CrewId);
        bool SignalGalleonCoveCleared();
        bool SignalLaLiberacionBoarded();
        bool SignalLaLiberacionHelmSecured();
        bool SignalLaLiberacionHarborCleared();
        bool CanUseHelm() const;
        bool BeginFirstWake();
        bool ReachRexaHarbor();

        static std::string_view OpeningCrewRole(std::string_view CrewId);

    private:
        CampaignRuntime& Campaign;
        Events Listeners;
        OpeningProgress Current;

        bool Commit(const OpeningProgress& Candidate);
        bool AdvanceLocation(OpeningLocation Expected, OpeningLocation Next, std::string_view Fact = {});
        void Cinematic(std::string_view Id) const;
        void Journal(std::string_view Id) const;
    };
}
