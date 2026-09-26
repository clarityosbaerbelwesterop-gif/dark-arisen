#include "DarkArisen/Core/OpeningRuntime.h"

#include "DarkArisen/Core/SaveRules.h"

#include <utility>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr std::string_view HomeWater = "Main.C01.01.HomeWater";
        constexpr std::string_view BlackSails = "Main.C01.02.BlackSails";
        constexpr std::string_view TheTaking = "Main.C01.03.TheTaking";
        constexpr std::string_view Undertow = "Main.C01.04.Undertow";
        constexpr std::string_view ShatteredCoast = "Main.C02.01.ShatteredCoast";
        constexpr std::string_view AShipToTake = "Main.C02.02.AShipToTake";
        constexpr std::string_view FirstWake = "Main.C02.03.FirstWake";

        bool IsFamily(const std::string_view Id)
        {
            return Id == "character.marc" || Id == "character.denise" || Id == "character.ethan";
        }
    }

    OpeningRuntime::OpeningRuntime(CampaignRuntime& InCampaign, Events InListeners)
        : Campaign(InCampaign), Listeners(std::move(InListeners))
    {
        RestoreProgress();
    }

    void OpeningRuntime::RestoreProgress()
    {
        Current = Campaign.State().Opening;
    }

    std::string_view OpeningRuntime::OpeningCrewRole(const std::string_view CrewId)
    {
        if (CrewId == "crew.mira") return "FirstMate";
        if (CrewId == "crew.big_tom") return "Smith";
        if (CrewId == "crew.esteban") return "Navigator";
        return {};
    }

    bool OpeningRuntime::Commit(const OpeningProgress& Candidate)
    {
        if (!Campaign.SetOpeningProgress(Candidate))
        {
            return false;
        }
        Current = Candidate;
        return true;
    }

    void OpeningRuntime::Cinematic(const std::string_view Id) const
    {
        if (Listeners.CinematicRequested) Listeners.CinematicRequested(Id);
    }

    void OpeningRuntime::Journal(const std::string_view Id) const
    {
        // Persisted before presentation; a repeated observation is already in the notebook.
        Campaign.UpdateJournal([Id](QuestJournalState& State, const QuestCatalog&) { return QuestJournal::RecordObservation(State, Id); });
        if (Listeners.JournalObservation) Listeners.JournalObservation(Id);
    }

    bool OpeningRuntime::AdvanceLocation(const OpeningLocation Expected, const OpeningLocation Next, const std::string_view Fact)
    {
        if (Current.Location != Expected)
        {
            return false;
        }
        if (!Fact.empty() && !Campaign.SetWorldFact(Fact))
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.Location = Next;
        if (!Commit(Candidate))
        {
            return false;
        }
        if (Listeners.LocationChanged) Listeners.LocationChanged(Expected, Next);
        return true;
    }

    bool OpeningRuntime::SignalFamilyInteraction(const std::string_view CharacterId)
    {
        if (Current.Location != OpeningLocation::FamilyMerchantShip || Current.RaidState != OpeningRaidState::Peaceful ||
            !IsFamily(CharacterId))
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.FamilyInteractions.insert(std::string(CharacterId));
        if (!Commit(Candidate))
        {
            return false;
        }
        if (Current.FamilyInteractions.size() == 3)
        {
            if (Campaign.GetMissionState(HomeWater) == MissionState::Available) Campaign.ActivateMission(HomeWater);
            if (Campaign.GetMissionState(HomeWater) == MissionState::Active) Campaign.CompleteMission(HomeWater);
            Journal("Journal.HomeWater.FamilyRoutes");
        }
        return true;
    }

    bool OpeningRuntime::SignalFleetDetected()
    {
        if (Current.RaidState != OpeningRaidState::Peaceful || Current.FamilyInteractions.size() < 3)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::FleetDetected;
        if (!Commit(Candidate))
        {
            return false;
        }
        if (Campaign.GetMissionState(BlackSails) == MissionState::Available) Campaign.ActivateMission(BlackSails);
        Cinematic("Cinematic.Opening.BlackSailsSighted");
        return true;
    }

    bool OpeningRuntime::SignalBoardingStarted()
    {
        if (Current.RaidState != OpeningRaidState::FleetDetected)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::Boarding;
        return Commit(Candidate);
    }

    bool OpeningRuntime::BeginBoardingEncounter(const std::string_view EncounterId, const int RequiredBoarders)
    {
        if (Current.RaidState != OpeningRaidState::Boarding || EncounterId.empty() || RequiredBoarders < 2)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::FirstEncounter;
        Candidate.BoardingEncounterId = std::string(EncounterId);
        Candidate.BoardersRequired = RequiredBoarders;
        Candidate.BoardersDefeated = 0;
        Candidate.DravenBoarded = false;
        return Commit(Candidate);
    }

    bool OpeningRuntime::SignalBoarderDefeated(const std::string_view EncounterId)
    {
        if (Current.RaidState != OpeningRaidState::FirstEncounter || EncounterId.empty() ||
            EncounterId != Current.BoardingEncounterId || Current.BoardersDefeated >= Current.BoardersRequired)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        ++Candidate.BoardersDefeated;
        return Commit(Candidate);
    }

    bool OpeningRuntime::SignalDravenBoarded(const std::string_view EncounterId)
    {
        if (Current.RaidState != OpeningRaidState::FirstEncounter || EncounterId != Current.BoardingEncounterId ||
            Current.BoardersRequired < 2 || Current.BoardersDefeated < Current.BoardersRequired)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::DravenAboard;
        Candidate.DravenBoarded = true;
        if (!Commit(Candidate))
        {
            return false;
        }
        Cinematic("Presentation.Opening.DravenBoards");
        return true;
    }

    bool OpeningRuntime::SignalTakingStarted()
    {
        if (Current.RaidState != OpeningRaidState::DravenAboard || !Current.DravenBoarded ||
            Campaign.GetMissionState(BlackSails) != MissionState::Active)
        {
            return false;
        }
        if (!Campaign.CompleteMission(BlackSails) || !Campaign.ActivateMission(TheTaking))
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::Taking;
        if (!Commit(Candidate))
        {
            return false;
        }
        Cinematic("Cinematic.Opening.TheTaking");
        return true;
    }

    bool OpeningRuntime::SignalTakingCinematicComplete()
    {
        if (Current.RaidState != OpeningRaidState::Taking || Campaign.GetMissionState(TheTaking) != MissionState::Active)
        {
            return false;
        }
        if (!Campaign.CompleteMission(TheTaking) || !Campaign.ActivateMission(Undertow))
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RaidState = OpeningRaidState::Overboard;
        Candidate.RecoveryState = WaterRecoveryState::Overboard;
        if (!Commit(Candidate))
        {
            return false;
        }
        Cinematic("Cinematic.Opening.JakeOverboard");
        return true;
    }

    bool OpeningRuntime::SignalEnteredWater()
    {
        if (Current.RaidState != OpeningRaidState::Overboard || Current.RecoveryState != WaterRecoveryState::Overboard ||
            Current.Location != OpeningLocation::FamilyMerchantShip)
        {
            return false;
        }
        const OpeningProgress Before = Current;
        Current.RecoveryState = WaterRecoveryState::Swimming;
        if (AdvanceLocation(OpeningLocation::FamilyMerchantShip, OpeningLocation::OpenWater))
        {
            return true;
        }
        Current = Before;
        return false;
    }

    bool OpeningRuntime::SignalReachedOuterReef()
    {
        if (Current.RecoveryState != WaterRecoveryState::Swimming)
        {
            return false;
        }
        const OpeningProgress Before = Current;
        Current.RecoveryState = WaterRecoveryState::OuterReef;
        if (AdvanceLocation(OpeningLocation::OpenWater, OpeningLocation::OuterReef, "World.OuterReefReached"))
        {
            return true;
        }
        Current = Before;
        return false;
    }

    bool OpeningRuntime::SignalReachedLocation(const OpeningLocation NewLocation)
    {
        if (NewLocation == OpeningLocation::DriftwoodBeach && Current.Location == OpeningLocation::OuterReef)
        {
            const OpeningProgress Before = Current;
            Current.RecoveryState = WaterRecoveryState::BeachCrawl;
            if (AdvanceLocation(OpeningLocation::OuterReef, NewLocation, "World.DriftwoodBeachReached"))
            {
                return true;
            }
            Current = Before;
            return false;
        }
        // Ship ownership, sea departure and Rexa arrival each have dedicated gameplay gates.
        if (NewLocation == OpeningLocation::LaLiberacion || NewLocation == OpeningLocation::OpenSea ||
            NewLocation == OpeningLocation::RexaHarbor || NewLocation == OpeningLocation::OpenWater ||
            NewLocation == OpeningLocation::OuterReef || NewLocation == OpeningLocation::DriftwoodBeach)
        {
            return false;
        }
        // Moran is walked in order; the beach recovery must finish before moving inland.
        if (static_cast<int>(NewLocation) != static_cast<int>(Current.Location) + 1 ||
            Current.RecoveryState != WaterRecoveryState::Recovered)
        {
            return false;
        }
        // The Moran route is walked one way: nobody who can only be met here may be left behind, or
        // A Ship to Take could never reach the helm with its core crew.
        if ((Current.Location == OpeningLocation::MirasCove && !Campaign.IsCrewRecruited("crew.mira")) ||
            (Current.Location == OpeningLocation::MangroveShallows && !Campaign.IsCrewRecruited("crew.big_tom")))
        {
            return false;
        }
        static constexpr std::string_view LocationFacts[] = {"", "", "", "World.DriftwoodBeachReached",
            "World.DriftwoodCampReached", "World.MirasCoveReached", "World.MangroveShallowsReached",
            "World.KoasTradingPostReached", "World.GalleonCoveReached", "", "", ""};
        return AdvanceLocation(Current.Location, NewLocation, LocationFacts[static_cast<int>(NewLocation)]);
    }

    bool OpeningRuntime::SignalRecoveryComplete()
    {
        if (Current.Location != OpeningLocation::DriftwoodBeach || Current.RecoveryState != WaterRecoveryState::BeachCrawl ||
            Campaign.GetMissionState(Undertow) != MissionState::Active)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.RecoveryState = WaterRecoveryState::Recovered;
        Candidate.RaidState = OpeningRaidState::Complete;
        if (!Commit(Candidate))
        {
            return false;
        }
        Campaign.SetWorldFact("World.MoranEntered");
        Campaign.SetCheckpoint("Checkpoint.Moran.DriftwoodBeach", "Spawn.Moran.DriftwoodBeach.Recovery");
        if (!Campaign.CompleteMission(Undertow))
        {
            return false;
        }
        if (Campaign.GetMissionState(ShatteredCoast) == MissionState::Available) Campaign.ActivateMission(ShatteredCoast);
        Journal("Journal.DriftwoodBeach.SmokeBeyondWreckTimber");
        return true;
    }

    bool OpeningRuntime::SignalCrewMet(const std::string_view CrewId)
    {
        if (OpeningCrewRole(CrewId).empty() || Current.Location < OpeningLocation::MirasCove ||
            Current.Location > OpeningLocation::GalleonCove)
        {
            return false;
        }
        if (CrewId == "crew.mira" && Current.Location != OpeningLocation::MirasCove) return false;
        if (CrewId == "crew.esteban" && Current.Location != OpeningLocation::GalleonCove) return false;
        return Campaign.MeetCrew(CrewId);
    }

    bool OpeningRuntime::SignalCrewRecruitmentAvailable(const std::string_view CrewId)
    {
        const std::string_view Role = OpeningCrewRole(CrewId);
        return !Role.empty() && Campaign.IsCrewMet(CrewId) && Campaign.SetCrewAvailable(CrewId, Role);
    }

    bool OpeningRuntime::SignalCrewRecruited(const std::string_view CrewId)
    {
        const std::string_view Role = OpeningCrewRole(CrewId);
        return !Role.empty() && Campaign.IsCrewAvailable(CrewId) && Campaign.RecruitCrew(CrewId, Role);
    }

    bool OpeningRuntime::SignalGalleonCoveCleared()
    {
        if (Current.Location != OpeningLocation::GalleonCove || !Campaign.SetWorldFact("World.GalleonCoveCleared"))
        {
            return false;
        }
        if (Campaign.GetMissionState(ShatteredCoast) == MissionState::Active && !Campaign.CompleteMission(ShatteredCoast))
        {
            return false;
        }
        if (Campaign.GetMissionState(AShipToTake) == MissionState::Available && !Campaign.ActivateMission(AShipToTake))
        {
            return false;
        }
        return true;
    }

    bool OpeningRuntime::SignalLaLiberacionBoarded()
    {
        if (Current.Location != OpeningLocation::GalleonCove || !Campaign.HasFact("World.GalleonCoveCleared") ||
            Campaign.GetMissionState(AShipToTake) != MissionState::Active || Current.LaLiberacionBoarded)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.LaLiberacionBoarded = true;
        return Commit(Candidate);
    }

    bool OpeningRuntime::SignalLaLiberacionHelmSecured()
    {
        if (!Current.LaLiberacionBoarded || Current.LaLiberacionHelmSecured || !Campaign.AreOpeningCrewRecruited())
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.LaLiberacionHelmSecured = true;
        return Commit(Candidate);
    }

    bool OpeningRuntime::SignalLaLiberacionHarborCleared()
    {
        if (Current.Location != OpeningLocation::GalleonCove || !Current.LaLiberacionHelmSecured ||
            Campaign.GetMissionState(AShipToTake) != MissionState::Active)
        {
            return false;
        }
        OpeningProgress Candidate = Current;
        Candidate.LaLiberacionClearedHarbor = true;
        if (!Commit(Candidate) || !Campaign.CompleteMission(AShipToTake))
        {
            return false;
        }
        return AdvanceLocation(OpeningLocation::GalleonCove, OpeningLocation::LaLiberacion);
    }

    bool OpeningRuntime::CanUseHelm() const
    {
        return Current.LaLiberacionHelmSecured && Campaign.AreOpeningCrewRecruited() &&
            (Current.Location == OpeningLocation::GalleonCove || Current.Location >= OpeningLocation::LaLiberacion);
    }

    bool OpeningRuntime::BeginFirstWake()
    {
        if (Current.Location != OpeningLocation::LaLiberacion || !CanUseHelm() ||
            Campaign.GetMissionState(FirstWake) != MissionState::Available || !Campaign.ActivateMission(FirstWake))
        {
            return false;
        }
        return AdvanceLocation(OpeningLocation::LaLiberacion, OpeningLocation::OpenSea,
            "Route.Moran.GalleonCove.OpenSea.RexaHarbor");
    }

    bool OpeningRuntime::ReachRexaHarbor()
    {
        if (Current.Location != OpeningLocation::OpenSea || Campaign.GetMissionState(FirstWake) != MissionState::Active)
        {
            return false;
        }
        return AdvanceLocation(OpeningLocation::OpenSea, OpeningLocation::RexaHarbor, "World.RexaEntered") &&
            Campaign.CompleteMission(FirstWake);
    }
}
