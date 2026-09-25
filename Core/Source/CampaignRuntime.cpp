#include "DarkArisen/Core/CampaignRuntime.h"

#include "DarkArisen/Core/Facts.h"
#include "DarkArisen/Core/SaveRules.h"

#include <algorithm>
#include <utility>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr std::string_view LaLiberacionShipId = "ship.la_liberacion";

        struct CompletionFactRow
        {
            std::string_view MissionId;
            std::vector<std::string_view> Facts;
        };

        /**
         * Authored completion facts: union of the Unreal ApplyMissionFacts and
         * CompleteAuthoredMission tables plus the Chapter 1/2 facts that
         * MAIN_STORY_AUTHORITY_2026_09 section 4 requires but the Unreal runtime never set.
         * Special cases (ship ownership, Draven outcome) are handled in code.
         */
        const std::vector<CompletionFactRow>& CompletionFacts()
        {
            static const std::vector<CompletionFactRow> Rows = {
                {"Main.C01.01.HomeWater", {Facts::FamilyIntact, Facts::EthanPresent}},
                {"Main.C01.02.BlackSails", {Facts::DravenIdentified}},
                {"Main.C01.03.TheTaking",
                    {Facts::MarcDead, Facts::DeniseDead, Facts::EthanAbducted, Facts::JakeOverboard}},
                {"Main.C01.04.Undertow",
                    {"World.MoranEntered", "World.DriftwoodBeachReached", Facts::JakeSurvived, Facts::EthanAliveLead,
                        "Chapter.01.Complete"}},
                {"Main.C02.01.ShatteredCoast",
                    {"World.MoranOpeningRouteKnown", "Crew.MiraKnown", "Crew.BigTomKnown", "Crew.EstebanKnown"}},
                {"Main.C02.02.AShipToTake", {"World.GalleonCoveCleared", Facts::LaLiberacionOwned, "Crew.CoreThreeJoined"}},
                {"Main.C02.03.FirstWake", {"World.RexaEntered", "Chapter.02.Complete", "Travel.FreeSailingSeeded"}},
                {"Main.C03.01.RexaHarbor", {"Story.EthanAliveConfirmed"}},
                {"Main.C03.02.SafeRoutes", {"Story.EthanRouteMarksFound"}},
                {"Main.C03.03.TheFirstHolder", {"Story.FirstHolderCrossed", "Chapter.03.Complete"}},
                {"Main.C04.01.SaltAndIron", {"War.VisibleToJake"}},
                {"Main.C04.02.BrokenCompact", {"War.Playable"}},
                {"Main.C04.03.HerrerasFall", {"Story.HarlowChartRecovered", "Chapter.04.Complete"}},
                {"Main.C05.01.WarCurrent", {"Story.NavigatorAliasKnown"}},
                {"Main.C05.02.HoldersWake", {"Assassins.Unlocked"}},
                {"Main.C05.03.NoSafeHarbor", {"Story.DeepRouteKnown", "Chapter.05.Complete"}},
                {"Main.C06.01.HighmooreRoad", {"Holding.FirstOwned"}},
                {"Main.C06.02.CrystalPassage", {"Holding.IntelligenceReady"}},
                {"Main.C06.03.TheNorthernOath", {"Story.EthanResisting", "Chapter.06.Complete"}},
                {"Main.C07.01.FalseBearings", {"Story.RetaliationPaid"}},
                {"Main.C07.02.EthansMarks", {"Story.EthanUsedAsNavigator"}},
                {"Main.C07.03.ThroughTheNet", {"Story.RescueBeforeRevenge", "Chapter.07.Complete"}},
                {"Main.C08.01.ThePrisonCourse", {"Armada.Revealed"}},
                {"Main.C08.02.BrothersAlive", {Facts::EthanSignalUnderstood}},
                {"Main.C08.03.HomewardBearing",
                    {Facts::EthanRecovered, Facts::EthanNoLongerCaptive, "Chapter.08.Complete"}},
                {"Main.C09.01.EthansGrove", {"Story.DravenMotiveKnown"}},
                {"Main.C09.02.MemoryOfABrother", {"Story.GroveVisited"}},
                {"Main.C09.03.DreamFight", {Facts::DreamResolved}},
                {"Main.C09.04.WakingCourse", {Facts::RacheUnlocked, "Story.FinalRouteKnown", "Chapter.09.Complete"}},
                {"Main.C10.01.Armada", {"Finale.EntryStateLocked", "Story.ArmadaBreachOpen"}},
                {"Main.C10.02.BreakTheChain", {"Armada.LogisticsBroken", "Story.ChainBroken"}},
                {"Main.C10.03.BlackDeck", {"Finale.DravenReached", "Story.BlackDeckReached"}},
                {"Main.C10.04.DravenVoss", {Facts::DravenDefeated}},
                {"Main.C10.05.TheWakeAfter",
                    {Facts::MainComplete, Facts::MainCampaignComplete, Facts::CreditsReachable, "Chapter.10.Complete"}},
            };
            return Rows;
        }

        /** Bosses whose fight belongs to one mission; Holders appear across the campaign. */
        std::string_view BossOwningMission(const std::string_view BossId)
        {
            if (BossId == "boss.dream_ethan") return "Main.C09.03.DreamFight";
            if (BossId == "boss.draven_voss") return "Main.C10.04.DravenVoss";
            return {};
        }
    }

    CampaignRuntime::CampaignRuntime()
    {
        NewGame();
    }

    void CampaignRuntime::NewGame()
    {
        Current = CampaignState::NewGame();
        AutosaveError.clear();
    }

    const std::vector<std::string_view>& CampaignRuntime::OpeningCrewIds()
    {
        static const std::vector<std::string_view> Ids = {"crew.mira", "crew.big_tom", "crew.esteban"};
        return Ids;
    }

    const std::vector<std::string_view>& CampaignRuntime::CanonicalBossIds()
    {
        // The Nine Who Hold (Docs/M7_TIER1_BOSS_REGISTER.md), the Chapter-9 dream figure and Draven.
        // Real Ethan ("character.ethan") is deliberately absent: he can never be a boss.
        static const std::vector<std::string_view> Ids = {"boss.herrera", "boss.reyes", "boss.cruz",
            "boss.de_silva", "boss.vega", "boss.blackwood", "boss.sterling", "boss.ashcroft", "boss.thorne",
            "boss.dream_ethan", "boss.draven_voss"};
        return Ids;
    }

    bool CampaignRuntime::LoadState(CampaignState Candidate, std::vector<std::string>& OutErrors)
    {
        OutErrors.clear();
        if (!MigrateCampaignState(Candidate, OutErrors) || !ValidateCampaignState(Candidate, OutErrors))
        {
            return false;
        }
        Current = std::move(Candidate);
        RefreshAvailability();
        return true;
    }

    bool CampaignRuntime::PrepareSave(CampaignState& OutState, std::vector<std::string>& OutErrors)
    {
        OutErrors.clear();
        if (World.CaptureWorld && !World.CaptureWorld(*this))
        {
            OutErrors.emplace_back("Live world capture failed.");
            return false;
        }
        if (!ValidateCampaignState(Current, OutErrors))
        {
            return false;
        }
        OutState = Current;
        return true;
    }

    bool CampaignRuntime::Validate(std::vector<std::string>& OutErrors) const
    {
        return ValidateCampaignState(Current, OutErrors);
    }

    MissionRuntime* CampaignRuntime::FindMission(const std::string_view MissionId)
    {
        const auto Found = std::find_if(Current.Missions.begin(), Current.Missions.end(),
            [MissionId](const MissionRuntime& Runtime) { return Runtime.MissionId == MissionId; });
        return Found == Current.Missions.end() ? nullptr : &*Found;
    }

    bool CampaignRuntime::ActivateMission(const std::string_view MissionId)
    {
        MissionRuntime* Runtime = FindMission(MissionId);
        // Failed missions re-enter through authored world state; there is no failure screen or rewind.
        if (Runtime == nullptr ||
            (Runtime->State != MissionState::Available && Runtime->State != MissionState::Failed))
        {
            return false;
        }
        for (const MissionRuntime& Other : Current.Missions)
        {
            if (Other.State == MissionState::Active)
            {
                return false;
            }
        }
        const int Index = MissionCatalog::FindIndex(MissionId);
        if (Index < 0)
        {
            return false;
        }
        Runtime->State = MissionState::Active;
        Current.CurrentMission = std::string(MissionId);
        Current.CurrentChapter = MissionCatalog::Missions()[static_cast<std::size_t>(Index)].Chapter;
        BroadcastMission(MissionId, MissionState::Active);
        return true;
    }

    bool CampaignRuntime::CompleteMission(const std::string_view MissionId)
    {
        MissionRuntime* Runtime = FindMission(MissionId);
        if (Runtime == nullptr || Runtime->State != MissionState::Active)
        {
            return false;
        }
        // The prize ship cannot leave harbor without the physically recruited core crew.
        if (MissionId == "Main.C02.02.AShipToTake" && !AreOpeningCrewRecruited())
        {
            return false;
        }

        const int PreviousChapter = Current.CurrentChapter;
        Runtime->State = MissionState::Completed;
        ApplyCompletionFacts(MissionId);
        RefreshAvailability();

        const bool ChapterBoundary = Current.CurrentChapter > PreviousChapter || MissionId == "Main.C10.05.TheWakeAfter";
        if (ChapterBoundary)
        {
            // Wary greetings recover by chapter progression only.
            for (int Chapter = PreviousChapter; Chapter < Current.CurrentChapter; ++Chapter) Progression::AdvanceSocialChapter(Current.Progress);
            if (World.NotifyChapterBoundary)
            {
                World.NotifyChapterBoundary(Current.CurrentChapter);
            }
            TryAutosave(DesignLaws::AutosaveTrigger::ChapterBoundary);
        }
        // Listeners may start map travel; every fact and legal save precedes them.
        BroadcastMission(MissionId, MissionState::Completed);
        return true;
    }

    bool CampaignRuntime::FailMission(const std::string_view MissionId)
    {
        MissionRuntime* Runtime = FindMission(MissionId);
        if (Runtime == nullptr || Runtime->State != MissionState::Active)
        {
            return false;
        }
        Runtime->State = MissionState::Failed;
        BroadcastMission(MissionId, MissionState::Failed);
        return true;
    }

    bool CampaignRuntime::MarkDravenCaptured()
    {
        if (GetMissionState("Main.C10.04.DravenVoss") != MissionState::Active || HasFact(Facts::DravenKilled))
        {
            return false;
        }
        AddFact(Facts::DravenCaptured);
        return true;
    }

    void CampaignRuntime::RefreshAvailability()
    {
        const auto& Catalog = MissionCatalog::Missions();
        for (std::size_t Index = 0; Index < Catalog.size() && Index < Current.Missions.size(); ++Index)
        {
            MissionRuntime& Runtime = Current.Missions[Index];
            if (Runtime.State != MissionState::Locked)
            {
                continue;
            }
            const std::string_view Prerequisite = Catalog[Index].Prerequisite;
            if (Prerequisite.empty() || Current.GetMissionState(Prerequisite) == MissionState::Completed)
            {
                Runtime.State = MissionState::Available;
            }
        }
        for (std::size_t Index = 0; Index < Current.Missions.size(); ++Index)
        {
            const MissionState State = Current.Missions[Index].State;
            if (State == MissionState::Available || State == MissionState::Active || State == MissionState::Failed)
            {
                Current.CurrentMission = Current.Missions[Index].MissionId;
                Current.CurrentChapter = Catalog[Index].Chapter;
                break;
            }
        }
    }

    void CampaignRuntime::AddFact(const std::string_view Fact)
    {
        auto& Facts = Facts::IsStoryFact(Fact) ? Current.StoryFacts : Current.WorldFacts;
        if (Facts.insert(std::string(Fact)).second)
        {
            for (const FactListener& Listener : FactListeners)
            {
                Listener(Fact, true);
            }
        }
    }

    void CampaignRuntime::ApplyCompletionFacts(const std::string_view MissionId)
    {
        for (const CompletionFactRow& Row : CompletionFacts())
        {
            if (Row.MissionId != MissionId)
            {
                continue;
            }
            for (const std::string_view Fact : Row.Facts)
            {
                AddFact(Fact);
            }
        }
        if (MissionId == "Main.C02.02.AShipToTake")
        {
            Current.CurrentShip = std::string(LaLiberacionShipId);
            for (CrewRelationship& Crew : Current.Crew)
            {
                Crew.Aboard = Crew.Aboard || Crew.Recruited;
            }
        }
        else if (MissionId == "Main.C10.04.DravenVoss" && !HasFact(Facts::DravenCaptured))
        {
            AddFact(Facts::DravenKilled);
        }
    }

    bool CampaignRuntime::TryAutosave(const DesignLaws::AutosaveTrigger Trigger)
    {
        AutosaveError.clear();
        if (!World.WriteAutosave || (World.IsAutosaveSuppressed && World.IsAutosaveSuppressed()))
        {
            return false;
        }
        CampaignState Candidate;
        std::vector<std::string> Errors;
        if (!PrepareSave(Candidate, Errors))
        {
            AutosaveError = Errors.empty() ? "Autosave validation failed." : Errors.front();
            return false;
        }
        if (!World.WriteAutosave(Candidate, Trigger))
        {
            AutosaveError = "Autosave write failed.";
            return false;
        }
        return true;
    }

    bool CampaignRuntime::CompleteRest()
    {
        return TryAutosave(DesignLaws::AutosaveTrigger::Rest);
    }

    template <typename MutationType>
    bool CampaignRuntime::CommitValidated(MutationType&& Mutation)
    {
        CampaignState Candidate = Current;
        if (!Mutation(Candidate))
        {
            return false;
        }
        std::vector<std::string> Errors;
        if (!ValidateCampaignState(Candidate, Errors))
        {
            return false;
        }
        Current = std::move(Candidate);
        return true;
    }

    bool CampaignRuntime::SetWorldFact(const std::string_view Fact, const bool Enabled)
    {
        if (Fact.empty() || Facts::IsStoryFact(Fact))
        {
            return false; // Story facts are authored by mission completion only.
        }
        const bool Had = Current.WorldFacts.count(std::string(Fact)) != 0;
        const bool Committed = CommitValidated([&](CampaignState& Candidate)
        {
            if (Enabled) Candidate.WorldFacts.insert(std::string(Fact));
            else Candidate.WorldFacts.erase(std::string(Fact));
            return true;
        });
        if (Committed && Had != Enabled)
        {
            for (const FactListener& Listener : FactListeners)
            {
                Listener(Fact, Enabled);
            }
        }
        return Committed;
    }

    bool CampaignRuntime::SetQuestOutcome(const std::string_view Key, const std::string_view Value)
    {
        if (Key.empty() || Value.empty())
        {
            return false;
        }
        const auto Existing = Current.QuestOutcomes.find(std::string(Key));
        if (Existing != Current.QuestOutcomes.end())
        {
            return Existing->second == Value; // Outcomes are final once recorded.
        }
        Current.QuestOutcomes.emplace(std::string(Key), std::string(Value));
        return true;
    }

    CrewRelationship* CampaignRuntime::FindCrew(const std::string_view CrewId)
    {
        const auto Found = std::find_if(Current.Crew.begin(), Current.Crew.end(),
            [CrewId](const CrewRelationship& Entry) { return Entry.CharacterId == CrewId; });
        return Found == Current.Crew.end() ? nullptr : &*Found;
    }

    const CrewRelationship* CampaignRuntime::FindCrew(const std::string_view CrewId) const
    {
        const auto Found = std::find_if(Current.Crew.begin(), Current.Crew.end(),
            [CrewId](const CrewRelationship& Entry) { return Entry.CharacterId == CrewId; });
        return Found == Current.Crew.end() ? nullptr : &*Found;
    }

    bool CampaignRuntime::MeetCrew(const std::string_view CrewId)
    {
        if (CrewId.empty())
        {
            return false;
        }
        if (CrewRelationship* Crew = FindCrew(CrewId))
        {
            Crew->Met = true;
            return true;
        }
        CrewRelationship Entry;
        Entry.CharacterId = std::string(CrewId);
        Entry.Met = true;
        Current.Crew.push_back(std::move(Entry));
        return true;
    }

    bool CampaignRuntime::SetCrewAvailable(const std::string_view CrewId, const std::string_view Role)
    {
        CrewRelationship* Crew = FindCrew(CrewId);
        if (Role.empty() || Crew == nullptr || !Crew->Met || Crew->Recruited)
        {
            return false;
        }
        Crew->Available = true;
        Crew->Role = std::string(Role);
        Current.QuestOutcomes["CrewRole." + std::string(CrewId)] = std::string(Role);
        return true;
    }

    bool CampaignRuntime::RecruitCrew(const std::string_view CrewId, const std::string_view Role)
    {
        CrewRelationship* Crew = FindCrew(CrewId);
        if (Role.empty() || Crew == nullptr || !Crew->Met || !Crew->Available || Crew->Recruited ||
            (!Crew->Role.empty() && Crew->Role != Role))
        {
            return false;
        }
        Crew->Role = std::string(Role);
        Crew->Recruited = true;
        Crew->Aboard = HasFact(Facts::LaLiberacionOwned);
        Crew->Trust = std::max(Crew->Trust, 10);
        Current.QuestOutcomes["CrewRole." + std::string(CrewId)] = std::string(Role);
        return true;
    }

    bool CampaignRuntime::SetCrewAboard(const std::string_view CrewId, const bool Aboard)
    {
        CrewRelationship* Crew = FindCrew(CrewId);
        if (Crew == nullptr || !Crew->Recruited || (Aboard && !HasFact(Facts::LaLiberacionOwned)))
        {
            return false;
        }
        Crew->Aboard = Aboard;
        return true;
    }

    bool CampaignRuntime::IsCrewMet(const std::string_view CrewId) const
    {
        const CrewRelationship* Crew = FindCrew(CrewId);
        return Crew != nullptr && Crew->Met;
    }

    bool CampaignRuntime::IsCrewAvailable(const std::string_view CrewId) const
    {
        const CrewRelationship* Crew = FindCrew(CrewId);
        return Crew != nullptr && Crew->Available;
    }

    bool CampaignRuntime::IsCrewRecruited(const std::string_view CrewId) const
    {
        const CrewRelationship* Crew = FindCrew(CrewId);
        return Crew != nullptr && Crew->Recruited;
    }

    bool CampaignRuntime::AreOpeningCrewRecruited() const
    {
        return std::all_of(OpeningCrewIds().begin(), OpeningCrewIds().end(),
            [this](const std::string_view Id) { return IsCrewRecruited(Id); });
    }

    bool CampaignRuntime::MarkBossDefeated(const std::string_view BossId)
    {
        const auto& Valid = CanonicalBossIds();
        if (std::find(Valid.begin(), Valid.end(), BossId) == Valid.end())
        {
            return false;
        }
        const std::string_view Owner = BossOwningMission(BossId);
        if (!Owner.empty() && GetMissionState(Owner) != MissionState::Active)
        {
            return false;
        }
        Current.DefeatedBosses.insert(std::string(BossId));
        return true;
    }

    bool CampaignRuntime::HasBossDefeated(const std::string_view BossId) const
    {
        return !BossId.empty() && Current.DefeatedBosses.count(std::string(BossId)) != 0;
    }

    bool CampaignRuntime::SetCheckpoint(const std::string_view CheckpointId, const std::string_view SpawnId)
    {
        if (CheckpointId.empty() || SpawnId.empty())
        {
            return false;
        }
        Current.CheckpointId = std::string(CheckpointId);
        Current.SpawnId = std::string(SpawnId);
        return true;
    }

    bool CampaignRuntime::SetOpeningProgress(const OpeningProgress& Progress)
    {
        if (!IsOpeningProgressValid(Progress))
        {
            return false;
        }
        Current.Opening = Progress;
        return true;
    }

    bool CampaignRuntime::DiscoverDungeon(const std::string_view DungeonId)
    {
        return !DungeonId.empty() && Current.DiscoveredDungeons.insert(std::string(DungeonId)).second;
    }

    bool CampaignRuntime::CompleteDungeon(const std::string_view DungeonId)
    {
        if (DungeonId.empty() || Current.CompletedDungeons.count(std::string(DungeonId)) != 0)
        {
            return false;
        }
        Current.DiscoveredDungeons.insert(std::string(DungeonId));
        Current.CompletedDungeons.insert(std::string(DungeonId));
        return true;
    }

    bool CampaignRuntime::RecoverTreasure(const std::string_view TreasureId)
    {
        if (TreasureId.empty() || Current.RecoveredTreasures.size() >= 9 ||
            !Current.RecoveredTreasures.insert(std::string(TreasureId)).second)
        {
            return false;
        }
        if (Current.RecoveredTreasures.size() == 9)
        {
            AddFact(Facts::StateTreasuresComplete);
        }
        return true;
    }

    bool CampaignRuntime::RegisterWarRegion(const std::string_view RegionId, const ColonialFaction InitialController)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            return ColonialWar::RegisterRegion(Candidate.ColonialWar, Candidate.CurrentChapter, RegionId, InitialController);
        });
    }

    bool CampaignRuntime::RecordWarAction(const std::string_view RegionId, const WarActionVerb Verb, const ColonialFaction TargetFaction,
        const int ControlDelta, const int LiberationDelta, const int CrimsonDelta)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            return ColonialWar::RecordResolvedWarAction(
                Candidate.ColonialWar, RegionId, Verb, TargetFaction, ControlDelta, LiberationDelta, CrimsonDelta);
        });
    }

    bool CampaignRuntime::RecordAutonomousWarTick(const std::string_view RegionId, const AutonomousWarTick& Tick)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            return ColonialWar::RecordAutonomousChapterTick(Candidate.ColonialWar, Candidate.CurrentChapter, RegionId, Tick);
        });
    }

    bool CampaignRuntime::RecordFallAssaultCompleted(const std::string_view RegionId)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            return ColonialWar::RecordFallAssaultCompleted(Candidate.ColonialWar, RegionId);
        });
    }

    bool CampaignRuntime::ClaimHolding(const std::string_view HoldingId)
    {
        return CommitValidated([&](CampaignState& Candidate) { return ColonialWar::ClaimHolding(Candidate.ColonialWar, HoldingId); });
    }

    bool CampaignRuntime::RegisterQuest(const QuestDefinition& Definition)
    {
        QuestCatalog Candidate = Quests;
        if (!Candidate.Register(Definition)) return false;
        const bool Adopted = CommitValidated([&](CampaignState& State)
        {
            QuestJournal::AdoptCatalog(State.Journal, Candidate);
            return true;
        });
        if (Adopted) Quests = std::move(Candidate);
        return Adopted;
    }

    bool CampaignRuntime::UpdateJournal(const std::function<bool(QuestJournalState&, const QuestCatalog&)>& Mutation)
    {
        return Mutation && CommitValidated([&](CampaignState& Candidate) { return Mutation(Candidate.Journal, Quests); });
    }

    bool CampaignRuntime::UpdateProgression(const std::function<bool(ProgressionState&, CharacterProgression&, const SkillCatalog&)>& Mutation)
    {
        return Mutation && CommitValidated([&](CampaignState& Candidate) { return Mutation(Candidate.Progress, Candidate.Progression, Skills); });
    }

    bool CampaignRuntime::UpdateLivingWorld(const std::function<bool(LivingWorldState&)>& Mutation)
    {
        return Mutation && CommitValidated([&](CampaignState& Candidate) { return Mutation(Candidate.LivingWorld); });
    }

    std::vector<std::string_view> CampaignRuntime::AuthoredCompletionFacts(const std::string_view MissionId)
    {
        std::vector<std::string_view> Result;
        for (const CompletionFactRow& Row : CompletionFacts())
        {
            if (Row.MissionId == MissionId)
            {
                Result.insert(Result.end(), Row.Facts.begin(), Row.Facts.end());
            }
        }
        return Result;
    }

    bool CampaignRuntime::CapturePlayerRuntime(const PlayerRuntimeSnapshot& Snapshot)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            Candidate.PlayerRuntime = Snapshot;
            Candidate.PlayerRuntime.MissionId = Candidate.CurrentMission;
            return Snapshot.Valid;
        });
    }

    bool CampaignRuntime::CaptureShipVoyage(const ShipVoyageSnapshot& Snapshot)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            Candidate.LaLiberacionVoyage = Snapshot;
            return Snapshot.Valid;
        });
    }

    bool CampaignRuntime::CaptureWorldRules(const WorldRulesSnapshot& Snapshot)
    {
        return CommitValidated([&](CampaignState& Candidate)
        {
            Candidate.WorldRules = Snapshot;
            return Snapshot.Valid;
        });
    }

    void CampaignRuntime::BroadcastMission(const std::string_view MissionId, const MissionState State)
    {
        for (const MissionListener& Listener : MissionListeners)
        {
            Listener(MissionId, State);
        }
    }
}
