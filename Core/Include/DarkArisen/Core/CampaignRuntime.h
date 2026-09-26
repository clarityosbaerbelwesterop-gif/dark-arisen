#pragma once

#include "DarkArisen/Core/CampaignState.h"
#include "DarkArisen/Core/DesignLaws.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /**
     * Engine hooks the campaign needs from the world. The engine adapter (O3DE system
     * component, Unreal subsystem, headless test) supplies them; the core never touches
     * disk, levels or actors itself.
     */
    struct CampaignWorldServices
    {
        /** Copies live world/player/ship state into the runtime (CapturePlayerRuntime etc.). */
        std::function<bool(class CampaignRuntime&)> CaptureWorld;
        std::function<void(int NewChapter)> NotifyChapterBoundary;
        std::function<bool()> IsAutosaveSuppressed;
        /** Persists an already validated state. Returns false on IO failure. */
        std::function<bool(const CampaignState&, DesignLaws::AutosaveTrigger)> WriteAutosave;
    };

    /**
     * Authoritative story runtime: mission order, authored completion facts, crew,
     * bosses, checkpoints and autosave policy. Port of UMainStorySubsystem with the
     * PR #52 ordering: all authored facts and legal autosaves happen before listeners
     * that may start map travel.
     */
    class CampaignRuntime
    {
    public:
        using MissionListener = std::function<void(std::string_view MissionId, MissionState NewState)>;
        using FactListener = std::function<void(std::string_view Fact, bool Enabled)>;

        CampaignRuntime();

        void NewGame();
        const CampaignState& State() const { return Current; }
        /** Migrates and validates a loaded state; the current state is untouched on failure. */
        bool LoadState(CampaignState Candidate, std::vector<std::string>& OutErrors);
        /** Captures the live world and returns a validated copy ready to be written. */
        bool PrepareSave(CampaignState& OutState, std::vector<std::string>& OutErrors);
        bool Validate(std::vector<std::string>& OutErrors) const;

        void SetWorldServices(CampaignWorldServices Services) { World = std::move(Services); }
        void AddMissionListener(MissionListener Listener) { MissionListeners.push_back(std::move(Listener)); }
        void AddFactListener(FactListener Listener) { FactListeners.push_back(std::move(Listener)); }

        // Missions.
        MissionState GetMissionState(std::string_view MissionId) const { return Current.GetMissionState(MissionId); }
        bool ActivateMission(std::string_view MissionId);
        bool CompleteMission(std::string_view MissionId);
        bool FailMission(std::string_view MissionId);
        /** Authored Chapter-10 mercy route; only legal while the Draven duel is active. */
        bool MarkDravenCaptured();

        // Facts.
        bool HasFact(std::string_view Fact) const { return Current.HasFact(Fact); }
        bool SetWorldFact(std::string_view Fact, bool Enabled = true);
        bool SetQuestOutcome(std::string_view Key, std::string_view Value);

        // Crew.
        bool MeetCrew(std::string_view CrewId);
        bool SetCrewAvailable(std::string_view CrewId, std::string_view Role);
        bool RecruitCrew(std::string_view CrewId, std::string_view Role);
        bool SetCrewAboard(std::string_view CrewId, bool Aboard);
        bool IsCrewMet(std::string_view CrewId) const;
        bool IsCrewAvailable(std::string_view CrewId) const;
        bool IsCrewRecruited(std::string_view CrewId) const;
        bool AreOpeningCrewRecruited() const;

        // Bosses, checkpoints, discoveries.
        bool MarkBossDefeated(std::string_view BossId);
        bool HasBossDefeated(std::string_view BossId) const;
        bool SetCheckpoint(std::string_view CheckpointId, std::string_view SpawnId);
        bool SetOpeningProgress(const OpeningProgress& Progress);
        bool DiscoverDungeon(std::string_view DungeonId);
        bool CompleteDungeon(std::string_view DungeonId);
        bool RecoverTreasure(std::string_view TreasureId);

        // Colonial war (hidden strategic state; physical missions report resolved actions).
        const ColonialWarState& GetColonialWar() const { return Current.ColonialWar; }
        bool RegisterWarRegion(std::string_view RegionId, ColonialFaction InitialController);
        bool RecordWarAction(std::string_view RegionId, WarActionVerb Verb, ColonialFaction TargetFaction, int ControlDelta,
            int LiberationDelta, int CrimsonDelta);
        bool RecordAutonomousWarTick(std::string_view RegionId, const AutonomousWarTick& Tick);
        bool RecordFallAssaultCompleted(std::string_view RegionId);
        bool ClaimHolding(std::string_view HoldingId);

        // Quest journal (Jake's notebook, evidence he read, authored observations).
        const QuestJournalState& GetJournal() const { return Current.Journal; }
        const QuestCatalog& GetQuestCatalog() const { return Quests; }
        /** Registers an authored quest; the journal adopts it as dormant. */
        bool RegisterQuest(const QuestDefinition& Definition);
        bool UpdateJournal(const std::function<bool(QuestJournalState&, const QuestCatalog&)>& Mutation);

        // BODY/CRAFT/STANDING, money, services and the chapter ledger.
        const ProgressionState& GetProgression() const { return Current.Progress; }
        const SkillCatalog& GetSkillCatalog() const { return Skills; }
        bool UpdateProgression(const std::function<bool(ProgressionState&, CharacterProgression&, const SkillCatalog&)>& Mutation);

        // Story-relevant living NPC state.
        const LivingWorldState& GetLivingWorld() const { return Current.LivingWorld; }
        bool UpdateLivingWorld(const std::function<bool(LivingWorldState&)>& Mutation);
        /** Transient seat/conversation presence; never saved. */
        SocialPresence& Social() { return Presence; }

        // World capture (called by CampaignWorldServices::CaptureWorld or map travel).
        bool CapturePlayerRuntime(const PlayerRuntimeSnapshot& Snapshot);
        bool CaptureShipVoyage(const ShipVoyageSnapshot& Snapshot);
        bool CaptureWorldRules(const WorldRulesSnapshot& Snapshot);

        /** The second legal autosave source. Returns true when a save was written. */
        bool CompleteRest();

        /** Last autosave failure, empty when the last autosave succeeded or none ran. */
        const std::string& LastAutosaveError() const { return AutosaveError; }

        static const std::vector<std::string_view>& OpeningCrewIds();
        static const std::vector<std::string_view>& CanonicalBossIds();
        /** Facts completing MissionId sets (empty for an unknown mission). */
        static std::vector<std::string_view> AuthoredCompletionFacts(std::string_view MissionId);

    private:
        CampaignState Current;
        CampaignWorldServices World;
        std::vector<MissionListener> MissionListeners;
        std::vector<FactListener> FactListeners;
        std::string AutosaveError;
        QuestCatalog Quests;
        SkillCatalog Skills = SkillCatalog::Canonical();
        SocialPresence Presence;

        MissionRuntime* FindMission(std::string_view MissionId);
        CrewRelationship* FindCrew(std::string_view CrewId);
        const CrewRelationship* FindCrew(std::string_view CrewId) const;
        void AddFact(std::string_view Fact);
        void RefreshAvailability();
        void ApplyCompletionFacts(std::string_view MissionId);
        bool TryAutosave(DesignLaws::AutosaveTrigger Trigger);
        /** Applies Mutation to a copy, validates it and commits only a valid result. */
        template <typename MutationType>
        bool CommitValidated(MutationType&& Mutation);
        void BroadcastMission(std::string_view MissionId, MissionState State);
    };
}
