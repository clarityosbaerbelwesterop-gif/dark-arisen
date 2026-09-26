#include "DarkArisen/Core/QuestJournal.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <set>

namespace DarkArisen::Core
{
    namespace
    {
        std::string Lower(std::string_view Text)
        {
            std::string Out(Text);
            for (char& Character : Out) Character = static_cast<char>(std::tolower(static_cast<unsigned char>(Character)));
            return Out;
        }

        std::string_view Trim(std::string_view Text)
        {
            while (!Text.empty() && std::isspace(static_cast<unsigned char>(Text.front()))) Text.remove_prefix(1);
            while (!Text.empty() && std::isspace(static_cast<unsigned char>(Text.back()))) Text.remove_suffix(1);
            return Text;
        }

        QuestRuntimeState* FindQuest(QuestJournalState& State, const std::string_view QuestId)
        {
            const auto It = State.Quests.find(QuestId);
            return It == State.Quests.end() ? nullptr : &It->second;
        }

        bool AppendInternal(QuestJournalState& State, const std::string_view QuestId, const std::string_view EntryId,
            const std::string_view JakeText, const std::string_view Directions, const std::int64_t GameMinute, const bool Distorted,
            const bool Correction)
        {
            const QuestRuntimeState* Quest = FindQuest(State, QuestId);
            if (Quest == nullptr || EntryId.empty() || JakeText.empty() || GameMinute < 0 || GameMinute < Quest->ActivationGameMinute ||
                State.NextSequence == std::numeric_limits<std::int64_t>::max() ||
                std::any_of(State.Entries.begin(), State.Entries.end(), [EntryId](const JournalEntry& Entry) { return Entry.EntryId == EntryId; }))
            {
                return false;
            }
            JournalEntry Entry;
            Entry.EntryId = std::string(EntryId);
            Entry.QuestId = std::string(QuestId);
            Entry.JakeText = std::string(JakeText);
            Entry.Directions = std::string(Directions);
            Entry.GameMinute = GameMinute;
            Entry.Sequence = State.NextSequence++;
            Entry.Distorted = Distorted;
            Entry.Correction = Correction;
            State.Entries.push_back(std::move(Entry));
            return true;
        }

        bool IsOpen(const QuestRuntimeState* Quest)
        {
            return Quest != nullptr && (Quest->Lifecycle == QuestLifecycle::Active || Quest->Lifecycle == QuestLifecycle::Resolved);
        }
    }

    bool QuestCatalog::Register(const QuestDefinition& Definition)
    {
        if (Definition.QuestId.empty() || ById.count(Definition.QuestId) != 0 || Definition.ExpirationDurationMinutes < -1 ||
            Definition.ExpirationDurationMinutes == 0 ||
            (Definition.RequiresSpokenAgreement && Definition.Trigger != QuestActivationTrigger::Conversation) ||
            (!Definition.SilentAtActivation && (Definition.InitialEntryId.empty() || Definition.InitialJakeText.empty())))
        {
            return false;
        }
        for (const std::string& Excluded : Definition.MutuallyExclusiveQuestIds)
        {
            if (Excluded.empty() || Excluded == Definition.QuestId) return false;
        }
        for (const auto& [Id, Existing] : ById)
        {
            if (!Definition.InitialEntryId.empty() && Definition.InitialEntryId == Existing.InitialEntryId) return false;
        }
        ById.emplace(Definition.QuestId, Definition);
        return true;
    }

    const QuestDefinition* QuestCatalog::Find(const std::string_view QuestId) const
    {
        const auto It = ById.find(QuestId);
        return It == ById.end() ? nullptr : &It->second;
    }

    namespace QuestJournal
    {
        void AdoptCatalog(QuestJournalState& State, const QuestCatalog& Catalog)
        {
            for (const auto& [Id, Definition] : Catalog.Definitions())
            {
                State.Quests.try_emplace(Id);
            }
        }

        bool Activate(QuestJournalState& State, const QuestCatalog& Catalog, const std::string_view QuestId,
            const QuestActivationTrigger Trigger, const std::int64_t GameMinute, const bool SpokenAgreementConfirmed)
        {
            const QuestDefinition* Definition = Catalog.Find(QuestId);
            QuestRuntimeState* Quest = FindQuest(State, QuestId);
            if (Definition == nullptr || Quest == nullptr || Quest->Lifecycle != QuestLifecycle::Dormant || Trigger != Definition->Trigger ||
                GameMinute < 0 || (Definition->RequiresSpokenAgreement && !SpokenAgreementConfirmed))
            {
                return false;
            }
            if (Definition->ExpirationDurationMinutes > 0 &&
                Definition->ExpirationDurationMinutes > std::numeric_limits<std::int64_t>::max() - GameMinute)
            {
                return false;
            }
            const QuestRuntimeState Before = *Quest;
            Quest->Lifecycle = QuestLifecycle::Active;
            Quest->ActivationTrigger = Trigger;
            Quest->ActivationGameMinute = GameMinute;
            Quest->ExpirationGameMinute = Definition->ExpirationDurationMinutes > 0 ? GameMinute + Definition->ExpirationDurationMinutes : -1;
            Quest->ResolutionGameMinute = -1;
            Quest->OutcomeId.clear();
            if (!Definition->SilentAtActivation &&
                !AppendInternal(State, QuestId, Definition->InitialEntryId, Definition->InitialJakeText, Definition->InitialDirections, GameMinute,
                    Definition->InitialEntryDistorted, false))
            {
                *Quest = Before;
                return false;
            }
            const auto MarkUnavailable = [&State](const std::string_view Id)
            {
                QuestRuntimeState* Other = FindQuest(State, Id);
                if (Other != nullptr && Other->Lifecycle == QuestLifecycle::Dormant) Other->Lifecycle = QuestLifecycle::Unavailable;
            };
            for (const std::string& Excluded : Definition->MutuallyExclusiveQuestIds) MarkUnavailable(Excluded);
            for (const auto& [Id, Other] : Catalog.Definitions())
            {
                if (std::find(Other.MutuallyExclusiveQuestIds.begin(), Other.MutuallyExclusiveQuestIds.end(), QuestId) !=
                    Other.MutuallyExclusiveQuestIds.end())
                {
                    MarkUnavailable(Id);
                }
            }
            return true;
        }

        bool RevealSilentQuest(QuestJournalState& State, const QuestCatalog& Catalog, const std::string_view QuestId,
            const std::string_view EntryId, const std::string_view JakeText, const std::string_view Directions, const std::int64_t GameMinute,
            const bool Distorted)
        {
            const QuestDefinition* Definition = Catalog.Find(QuestId);
            const QuestRuntimeState* Quest = FindQuest(State, QuestId);
            if (Definition == nullptr || !Definition->SilentAtActivation || Quest == nullptr || Quest->Lifecycle != QuestLifecycle::Active ||
                HasEntryForQuest(State, QuestId))
            {
                return false;
            }
            return AppendInternal(State, QuestId, EntryId, JakeText, Directions, GameMinute, Distorted, false);
        }

        bool AppendNote(QuestJournalState& State, const std::string_view QuestId, const std::string_view EntryId,
            const std::string_view JakeText, const std::string_view Directions, const std::int64_t GameMinute, const bool Distorted)
        {
            return IsOpen(FindQuest(State, QuestId)) && AppendInternal(State, QuestId, EntryId, JakeText, Directions, GameMinute, Distorted, false);
        }

        bool AppendCorrection(QuestJournalState& State, const std::string_view QuestId, const std::string_view EntryId,
            const std::string_view JakeText, const std::string_view Directions, const std::int64_t GameMinute)
        {
            const bool HasDistorted = std::any_of(State.Entries.begin(), State.Entries.end(),
                [QuestId](const JournalEntry& Entry) { return Entry.QuestId == QuestId && Entry.Distorted; });
            return HasDistorted && IsOpen(FindQuest(State, QuestId)) &&
                AppendInternal(State, QuestId, EntryId, JakeText, Directions, GameMinute, false, true);
        }

        bool Resolve(QuestJournalState& State, const std::string_view QuestId, const std::string_view OutcomeId, const std::int64_t GameMinute)
        {
            QuestRuntimeState* Quest = FindQuest(State, QuestId);
            if (Quest == nullptr || Quest->Lifecycle != QuestLifecycle::Active || OutcomeId.empty() || GameMinute < Quest->ActivationGameMinute)
            {
                return false;
            }
            Quest->Lifecycle = QuestLifecycle::Resolved;
            Quest->ResolutionGameMinute = GameMinute;
            Quest->OutcomeId = std::string(OutcomeId);
            return true;
        }

        int ProcessExpirations(QuestJournalState& State, const std::int64_t GameMinute)
        {
            if (GameMinute < 0) return 0;
            int Resolved = 0;
            for (auto& [Id, Quest] : State.Quests)
            {
                if (Quest.Lifecycle != QuestLifecycle::Active || Quest.ExpirationGameMinute < 0 || Quest.ExpirationGameMinute > GameMinute) continue;
                Quest.Lifecycle = QuestLifecycle::Resolved;
                Quest.ResolutionGameMinute = GameMinute;
                Quest.OutcomeId = "Expired";
                ++Resolved;
            }
            return Resolved;
        }

        bool HasEntryForQuest(const QuestJournalState& State, const std::string_view QuestId)
        {
            return std::any_of(State.Entries.begin(), State.Entries.end(), [QuestId](const JournalEntry& Entry) { return Entry.QuestId == QuestId; });
        }

        std::vector<JournalEntry> Search(const QuestJournalState& State, const std::string_view Query)
        {
            const std::string Needle = Lower(Trim(Query));
            if (Needle.empty()) return State.Entries;
            std::vector<JournalEntry> Matches;
            for (const JournalEntry& Entry : State.Entries)
            {
                if (Lower(Entry.JakeText).find(Needle) != std::string::npos || Lower(Entry.Directions).find(Needle) != std::string::npos)
                {
                    Matches.push_back(Entry);
                }
            }
            return Matches;
        }

        bool RecordEvidence(QuestJournalState& State, EvidenceRecord Record)
        {
            if (Record.EvidenceId.empty() || Record.Title.empty() || Record.GameMinute < 0 ||
                std::any_of(State.Evidence.begin(), State.Evidence.end(),
                    [&Record](const EvidenceRecord& Existing) { return Existing.EvidenceId == Record.EvidenceId; }))
            {
                return false;
            }
            State.Evidence.push_back(std::move(Record));
            return true;
        }

        bool RecordObservation(QuestJournalState& State, const std::string_view ObservationId)
        {
            if (ObservationId.empty() || std::find(State.Observations.begin(), State.Observations.end(), ObservationId) != State.Observations.end())
            {
                return false;
            }
            State.Observations.emplace_back(ObservationId);
            return true;
        }

        bool Validate(const QuestJournalState& State, std::vector<std::string>& OutErrors)
        {
            const std::size_t Before = OutErrors.size();
            if (State.NextSequence < 0) OutErrors.emplace_back("Journal sequence is negative.");
            for (const auto& [Id, Quest] : State.Quests)
            {
                if (Id.empty()) OutErrors.emplace_back("Journal quest without an id.");
                if (Quest.Lifecycle == QuestLifecycle::Resolved && Quest.OutcomeId.empty())
                {
                    OutErrors.push_back("Journal quest " + Id + " resolved without an outcome.");
                }
            }
            std::set<std::string> EntryIds;
            std::int64_t MaxSequence = -1;
            for (const JournalEntry& Entry : State.Entries)
            {
                if (Entry.EntryId.empty() || Entry.QuestId.empty() || State.Quests.find(Entry.QuestId) == State.Quests.end() ||
                    Entry.Sequence < 0 || !EntryIds.insert(Entry.EntryId).second)
                {
                    OutErrors.push_back("Journal entry " + Entry.EntryId + " is malformed, duplicated or belongs to an unknown quest.");
                }
                MaxSequence = std::max(MaxSequence, Entry.Sequence);
            }
            if (State.NextSequence <= MaxSequence) OutErrors.emplace_back("Journal sequence does not follow its entries.");
            std::set<std::string> EvidenceIds;
            for (const EvidenceRecord& Record : State.Evidence)
            {
                if (Record.EvidenceId.empty() || !EvidenceIds.insert(Record.EvidenceId).second)
                {
                    OutErrors.push_back("Evidence record " + Record.EvidenceId + " is empty or duplicated.");
                }
            }
            std::set<std::string> ObservationIds(State.Observations.begin(), State.Observations.end());
            if (ObservationIds.size() != State.Observations.size() || ObservationIds.count(""))
            {
                OutErrors.emplace_back("Journal observations are empty or duplicated.");
            }
            return OutErrors.size() == Before;
        }
    }
}
