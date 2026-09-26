#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** Ordinals match EQuestActivationTrigger and are persisted. */
    enum class QuestActivationTrigger : std::uint8_t { Conversation, Overheard, Object, Proximity, WorldState, Absence };

    /** Ordinals match EQuestLifecycle and are persisted. */
    enum class QuestLifecycle : std::uint8_t { Dormant, Active, Resolved, Unavailable };

    /** Authored quest activation (FQuestActivationDefinition). Never persisted: it is content. */
    struct QuestDefinition
    {
        std::string QuestId;
        QuestActivationTrigger Trigger = QuestActivationTrigger::Conversation;
        /** Some authored quests begin without a journal entry. */
        bool SilentAtActivation = false;
        /** Spoken contract agreement; never an accept button. Conversation quests only. */
        bool RequiresSpokenAgreement = false;
        std::string InitialEntryId;
        std::string InitialJakeText;
        std::string InitialDirections;
        bool InitialEntryDistorted = false;
        /** -1: never expires. The deadline is internal and never shown as a countdown. */
        std::int64_t ExpirationDurationMinutes = -1;
        std::vector<std::string> MutuallyExclusiveQuestIds;
    };

    struct QuestRuntimeState
    {
        QuestLifecycle Lifecycle = QuestLifecycle::Dormant;
        QuestActivationTrigger ActivationTrigger = QuestActivationTrigger::Conversation;
        std::int64_t ActivationGameMinute = -1;
        std::int64_t ExpirationGameMinute = -1;
        std::int64_t ResolutionGameMinute = -1;
        std::string OutcomeId;
        bool operator==(const QuestRuntimeState&) const = default;
    };

    /** One page of Jake's notebook, in his words. */
    struct JournalEntry
    {
        std::string EntryId;
        std::string QuestId;
        std::string JakeText;
        std::string Directions;
        std::int64_t GameMinute = -1;
        std::int64_t Sequence = -1;
        bool Distorted = false;
        bool Correction = false;
        bool operator==(const JournalEntry&) const = default;
    };

    /** A physical document Jake has read (letter, ledger, chart), kept as he read it. */
    struct EvidenceRecord
    {
        std::string EvidenceId;
        std::string MissionId;
        std::string Title;
        std::string Body;
        std::int64_t GameMinute = 0;
        bool operator==(const EvidenceRecord&) const = default;
    };

    /**
     * Persistent notebook: port of FQuestJournalSnapshot plus the physical evidence Jake read and the
     * authored observations the opening records (neither was saved by the Unreal reference).
     */
    struct QuestJournalState
    {
        std::map<std::string, QuestRuntimeState, std::less<>> Quests;
        std::vector<JournalEntry> Entries;
        std::int64_t NextSequence = 0;
        std::vector<EvidenceRecord> Evidence;
        std::vector<std::string> Observations;
        bool operator==(const QuestJournalState&) const = default;
    };

    /** Authored quest definitions registered at runtime (UQuestJournalComponent::AuthoredDefinitions). */
    class QuestCatalog
    {
    public:
        /** Validates an authored definition (unique ids, spoken agreement only in conversation, ...). */
        bool Register(const QuestDefinition& Definition);
        const QuestDefinition* Find(std::string_view QuestId) const;
        const std::map<std::string, QuestDefinition, std::less<>>& Definitions() const { return ById; }

    private:
        std::map<std::string, QuestDefinition, std::less<>> ById;
    };

    /** Markerless quest state and Jake's chronological notebook (port of UQuestJournalComponent). */
    namespace QuestJournal
    {
        /** Adds a dormant runtime state for every registered quest the notebook does not know yet. */
        void AdoptCatalog(QuestJournalState& State, const QuestCatalog& Catalog);
        bool Activate(QuestJournalState& State, const QuestCatalog& Catalog, std::string_view QuestId, QuestActivationTrigger Trigger,
            std::int64_t GameMinute, bool SpokenAgreementConfirmed);
        bool RevealSilentQuest(QuestJournalState& State, const QuestCatalog& Catalog, std::string_view QuestId, std::string_view EntryId,
            std::string_view JakeText, std::string_view Directions, std::int64_t GameMinute, bool Distorted);
        bool AppendNote(QuestJournalState& State, std::string_view QuestId, std::string_view EntryId, std::string_view JakeText,
            std::string_view Directions, std::int64_t GameMinute, bool Distorted);
        /** Only for a quest with a distorted entry: Jake corrects what he wrote. */
        bool AppendCorrection(QuestJournalState& State, std::string_view QuestId, std::string_view EntryId, std::string_view JakeText,
            std::string_view Directions, std::int64_t GameMinute);
        bool Resolve(QuestJournalState& State, std::string_view QuestId, std::string_view OutcomeId, std::int64_t GameMinute);
        /** Resolves overdue quests as "Expired" silently; returns how many. */
        int ProcessExpirations(QuestJournalState& State, std::int64_t GameMinute);
        bool HasEntryForQuest(const QuestJournalState& State, std::string_view QuestId);
        /** The notebook's one tracking concession: plain, case-insensitive text search. */
        std::vector<JournalEntry> Search(const QuestJournalState& State, std::string_view Query);

        /** Each document once; kept verbatim. */
        bool RecordEvidence(QuestJournalState& State, EvidenceRecord Record);
        bool RecordObservation(QuestJournalState& State, std::string_view ObservationId);

        /** Structural invariants (RestoreSnapshot rules): unique entries, known quests, sequence order. */
        bool Validate(const QuestJournalState& State, std::vector<std::string>& OutErrors);
    }
}
