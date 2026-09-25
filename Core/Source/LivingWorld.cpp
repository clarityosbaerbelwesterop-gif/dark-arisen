#include "DarkArisen/Core/LivingWorld.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core::LivingWorld
{
    namespace
    {
        NpcRecord* FindMutable(LivingWorldState& World, const std::string_view NpcId)
        {
            const auto It = World.Records.find(NpcId);
            return It == World.Records.end() ? nullptr : &It->second;
        }

        bool IsValidMemory(const NpcMemory& Memory)
        {
            return !Memory.EventId.empty() && Memory.OriginalWeight >= 1 && Memory.OriginalWeight <= 100 && Memory.EffectiveWeight >= 0.0 &&
                Memory.EffectiveWeight <= 100.0 && Memory.OccurredAtGameMinute >= 0 && std::isfinite(Memory.DecayPerGameDay) &&
                Memory.DecayPerGameDay >= 0.0 && Memory.Kind <= NpcMemoryKind::CulturalReputation && Memory.Emotion <= NpcMemoryEmotion::Positive;
        }

        int ReputationDelta(const NpcMemory& Memory)
        {
            if (Memory.Emotion == NpcMemoryEmotion::Neutral || Memory.EffectiveWeight < 0.5) return 0;
            const int Magnitude = std::clamp(static_cast<int>(std::lround(Memory.EffectiveWeight * 0.5)), 1, 50);
            return Memory.Emotion == NpcMemoryEmotion::Positive ? Magnitude : -Magnitude;
        }

        double Transfer(const NpcConnectionKind Kind)
        {
            switch (Kind)
            {
            case NpcConnectionKind::Family: return 0.85;
            case NpcConnectionKind::Friend: return 0.70;
            case NpcConnectionKind::Professional: return 0.55;
            case NpcConnectionKind::Community: break;
            }
            return 0.40;
        }

        void Upsert(NpcRecord& Record, const std::string_view Other, const NpcConnectionKind Kind)
        {
            for (NpcConnection& Connection : Record.Connections)
            {
                if (Connection.OtherNpcId == Other)
                {
                    Connection.Kind = Kind;
                    return;
                }
            }
            Record.Connections.push_back({std::string(Other), Kind});
        }

        NpcMemory Report(const NpcMemory& Source, const std::string_view OriginNpcId, const double Factor, const bool KeepPermanent)
        {
            NpcMemory Memory = Source;
            Memory.Kind = NpcMemoryKind::ReportedEvent;
            Memory.SourceNpcId = std::string(OriginNpcId);
            Memory.OriginalWeight = std::clamp(static_cast<int>(std::lround(Source.OriginalWeight * Factor)), 1, 100);
            Memory.EffectiveWeight = std::clamp(Source.EffectiveWeight * Factor, 1.0, 100.0);
            Memory.Permanent = KeepPermanent && Source.Permanent && Memory.EffectiveWeight >= 50.0;
            return Memory;
        }
    }

    bool RegisterNpc(LivingWorldState& World, const std::string_view NpcId, const std::string_view CommunityId)
    {
        if (NpcId.empty()) return false;
        NpcRecord& Record = World.Records[std::string(NpcId)];
        if (!CommunityId.empty()) Record.CommunityId = std::string(CommunityId);
        return true;
    }

    bool UpdateScheduleAnchor(LivingWorldState& World, const std::string_view NpcId, const std::string_view AnchorId)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        if (Record == nullptr || AnchorId.empty() || !Record->Alive) return false;
        Record->ScheduleAnchorId = std::string(AnchorId);
        return true;
    }

    bool SetMood(LivingWorldState& World, const std::string_view NpcId, const NpcMood Mood)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        if (Record == nullptr) return false;
        Record->Mood = Mood;
        return true;
    }

    bool Connect(LivingWorldState& World, const std::string_view A, const std::string_view B, const NpcConnectionKind Kind)
    {
        NpcRecord* Left = FindMutable(World, A);
        NpcRecord* Right = FindMutable(World, B);
        if (A.empty() || B.empty() || A == B || Left == nullptr || Right == nullptr) return false;
        Upsert(*Left, B, Kind);
        Upsert(*Right, A, Kind);
        return true;
    }

    bool RecordMemory(LivingWorldState& World, const std::string_view NpcId, const NpcMemory& Memory)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        if (Record == nullptr || !IsValidMemory(Memory)) return false;
        for (NpcMemory& Existing : Record->Memories)
        {
            if (Existing.EventId == Memory.EventId && Existing.Kind == Memory.Kind)
            {
                if (Memory.EffectiveWeight <= Existing.EffectiveWeight) return false;
                Record->PersonalReputation = std::clamp(Record->PersonalReputation - ReputationDelta(Existing) + ReputationDelta(Memory), -100, 100);
                Existing = Memory;
                return true;
            }
        }
        Record->Memories.push_back(Memory);
        Record->PersonalReputation = std::clamp(Record->PersonalReputation + ReputationDelta(Memory), -100, 100);
        return true;
    }

    int PropagateMemory(LivingWorldState& World, const std::string_view OriginNpcId, const std::string_view EventId)
    {
        const NpcRecord* Origin = FindMutable(World, OriginNpcId);
        if (Origin == nullptr || EventId.empty()) return 0;
        const NpcMemory* Source = nullptr;
        for (const NpcMemory& Memory : Origin->Memories)
        {
            if (Memory.EventId == EventId && (Source == nullptr || Memory.EffectiveWeight > Source->EffectiveWeight)) Source = &Memory;
        }
        if (Source == nullptr || Source->EffectiveWeight < 5.0) return 0;
        const NpcMemory Original = *Source;
        const std::vector<NpcConnection> Connections = Origin->Connections;
        const std::string Community = Origin->CommunityId;
        int Spread = 0;
        std::set<std::string> Reached;
        for (const NpcConnection& Connection : Connections)
        {
            if (FindMutable(World, Connection.OtherNpcId) == nullptr) continue;
            if (RecordMemory(World, Connection.OtherNpcId, Report(Original, OriginNpcId, Transfer(Connection.Kind), true)))
            {
                ++Spread;
                Reached.insert(Connection.OtherNpcId);
            }
        }
        // Major events reach the local community even without an authored personal edge.
        if (Original.EffectiveWeight >= 20.0 && !Community.empty())
        {
            for (auto& [Id, Record] : World.Records)
            {
                if (Id == OriginNpcId || Reached.count(Id) || Record.CommunityId != Community) continue;
                if (RecordMemory(World, Id, Report(Original, OriginNpcId, 0.35, false))) ++Spread;
            }
        }
        return Spread;
    }

    bool SimulateToGameMinute(LivingWorldState& World, const std::int64_t GameMinute)
    {
        if (GameMinute < World.LastSimulatedGameMinute) return false;
        const double Days = static_cast<double>(GameMinute - World.LastSimulatedGameMinute) / 1440.0;
        if (Days <= 0.0) return true;
        for (auto& [Id, Record] : World.Records)
        {
            int Reputation = 0;
            for (NpcMemory& Memory : Record.Memories)
            {
                if (!Memory.Permanent && Memory.OriginalWeight < 100)
                {
                    Memory.EffectiveWeight = std::max(0.0, Memory.EffectiveWeight - Memory.DecayPerGameDay * Days);
                }
                Reputation += ReputationDelta(Memory);
            }
            Record.Memories.erase(std::remove_if(Record.Memories.begin(), Record.Memories.end(),
                                      [](const NpcMemory& Memory) { return !Memory.Permanent && Memory.EffectiveWeight < 0.5; }),
                Record.Memories.end());
            Record.PersonalReputation = std::clamp(Reputation, -100, 100);
        }
        World.LastSimulatedGameMinute = GameMinute;
        return true;
    }

    bool SetAlive(LivingWorldState& World, const std::string_view NpcId, const bool Alive)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        if (Record == nullptr || (Alive && !Record->Alive)) return false;
        Record->Alive = Alive;
        if (!Alive) Record->Available = false;
        return true;
    }

    bool SetAvailable(LivingWorldState& World, const std::string_view NpcId, const bool Available)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        if (Record == nullptr || (Available && !Record->Alive)) return false;
        Record->Available = Available;
        return true;
    }

    bool ResolveInteraction(LivingWorldState& World, const std::string_view NpcId, const std::string_view InteractionId)
    {
        NpcRecord* Record = FindMutable(World, NpcId);
        return Record != nullptr && !InteractionId.empty() && Record->ResolvedInteractions.emplace(InteractionId).second;
    }

    const NpcRecord* Find(const LivingWorldState& World, const std::string_view NpcId)
    {
        const auto It = World.Records.find(NpcId);
        return It == World.Records.end() ? nullptr : &It->second;
    }

    bool Validate(const LivingWorldState& World, std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        if (World.LastSimulatedGameMinute < 0) OutErrors.emplace_back("Living world clock is negative.");
        for (const auto& [Id, Record] : World.Records)
        {
            if (Id.empty() || Record.PersonalReputation < -100 || Record.PersonalReputation > 100 || Record.SpecificTrust < -100 ||
                Record.SpecificTrust > 100 || Record.Mood > NpcMood::Excited || (!Record.Alive && Record.Available))
            {
                OutErrors.push_back("Living NPC " + Id + " has an invalid record.");
            }
            std::set<std::string> Connections;
            for (const NpcConnection& Connection : Record.Connections)
            {
                if (Connection.OtherNpcId == Id || !World.Records.count(Connection.OtherNpcId) || !Connections.insert(Connection.OtherNpcId).second ||
                    Connection.Kind > NpcConnectionKind::Community)
                {
                    OutErrors.push_back("Living NPC " + Id + " has an invalid connection.");
                }
            }
            std::set<std::pair<std::string, int>> Events;
            for (const NpcMemory& Memory : Record.Memories)
            {
                if (!IsValidMemory(Memory) || !Events.emplace(Memory.EventId, static_cast<int>(Memory.Kind)).second)
                {
                    OutErrors.push_back("Living NPC " + Id + " has an invalid or duplicated memory.");
                }
            }
        }
        return OutErrors.size() == Before;
    }
}
