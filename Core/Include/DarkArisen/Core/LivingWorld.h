#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** Ordinals match the Unreal ENPCMemoryKind/Emotion/LivingMood/ConnectionKind and are persisted. */
    enum class NpcMemoryKind : std::uint8_t { DirectEncounter, WitnessedEvent, ReportedEvent, CulturalReputation };
    enum class NpcMemoryEmotion : std::uint8_t { Negative, Neutral, Positive };
    enum class NpcMood : std::uint8_t { Content, Stressed, Happy, Sad, Angry, Excited };
    enum class NpcConnectionKind : std::uint8_t { Family, Friend, Professional, Community };

    struct NpcMemory
    {
        std::string EventId;
        NpcMemoryKind Kind = NpcMemoryKind::DirectEncounter;
        NpcMemoryEmotion Emotion = NpcMemoryEmotion::Neutral;
        int OriginalWeight = 1;
        double EffectiveWeight = 1.0;
        std::int64_t OccurredAtGameMinute = 0;
        double DecayPerGameDay = 0.0;
        std::string SourceNpcId;
        bool Permanent = false;
        bool operator==(const NpcMemory&) const = default;
    };

    struct NpcConnection
    {
        std::string OtherNpcId;
        NpcConnectionKind Kind = NpcConnectionKind::Community;
        bool operator==(const NpcConnection&) const = default;
    };

    /**
     * One story-relevant person. Port of FNPCLivingRecord plus the persistent facts the story needs
     * (alive, available, resolved interactions), which the Unreal record did not carry.
     */
    struct NpcRecord
    {
        std::string CommunityId;
        std::string ScheduleAnchorId;
        int PersonalReputation = 0;
        int SpecificTrust = 0;
        NpcMood Mood = NpcMood::Content;
        std::vector<NpcMemory> Memories;
        std::vector<NpcConnection> Connections;
        bool Alive = true;
        bool Available = true;
        std::set<std::string, std::less<>> ResolvedInteractions;
        bool operator==(const NpcRecord&) const = default;
    };

    /** Persistent living population (FNPCLivingWorldSnapshot). Authored ids only; never generates people or quests. */
    struct LivingWorldState
    {
        std::int64_t LastSimulatedGameMinute = 0;
        std::map<std::string, NpcRecord, std::less<>> Records;
        bool operator==(const LivingWorldState&) const = default;
    };

    /** Port of UNPCLivingWorldSubsystem (which lived on the world and was lost on travel). */
    namespace LivingWorld
    {
        bool RegisterNpc(LivingWorldState& World, std::string_view NpcId, std::string_view CommunityId);
        bool UpdateScheduleAnchor(LivingWorldState& World, std::string_view NpcId, std::string_view AnchorId);
        bool SetMood(LivingWorldState& World, std::string_view NpcId, NpcMood Mood);
        bool Connect(LivingWorldState& World, std::string_view A, std::string_view B, NpcConnectionKind Kind);
        /** A stronger memory of the same event and kind replaces the weaker one; reputation follows. */
        bool RecordMemory(LivingWorldState& World, std::string_view NpcId, const NpcMemory& Memory);
        /** Word of an event travels along authored connections and, if major, through the community. */
        int PropagateMemory(LivingWorldState& World, std::string_view OriginNpcId, std::string_view EventId);
        /** Decays memories to the given game minute; never runs backwards. */
        bool SimulateToGameMinute(LivingWorldState& World, std::int64_t GameMinute);

        /** A death is permanent: the dead are never available and never come back. */
        bool SetAlive(LivingWorldState& World, std::string_view NpcId, bool Alive);
        bool SetAvailable(LivingWorldState& World, std::string_view NpcId, bool Available);
        bool ResolveInteraction(LivingWorldState& World, std::string_view NpcId, std::string_view InteractionId);
        const NpcRecord* Find(const LivingWorldState& World, std::string_view NpcId);

        bool Validate(const LivingWorldState& World, std::vector<std::string>& OutErrors);
    }
}
