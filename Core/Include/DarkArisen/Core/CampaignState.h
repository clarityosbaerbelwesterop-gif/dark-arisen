#pragma once

#include "DarkArisen/Core/MissionCatalog.h"

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace DarkArisen::Core
{
    struct Vector3
    {
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;
        bool operator==(const Vector3&) const = default;
    };

    struct Quaternion
    {
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;
        double W = 1.0;
        bool operator==(const Quaternion&) const = default;
    };

    /** Map-local transform. Only meaningful inside the level recorded next to it. */
    struct Transform
    {
        Vector3 Translation;
        Quaternion Rotation;
        Vector3 Scale{1.0, 1.0, 1.0};
        bool IsFinite() const;
        bool operator==(const Transform&) const = default;
    };

    struct MissionRuntime
    {
        std::string MissionId;
        MissionState State = MissionState::Locked;
        bool operator==(const MissionRuntime&) const = default;
    };

    struct CharacterProgression
    {
        int Level = 1;
        int Experience = 0;
        std::vector<std::string> Inventory;
        std::vector<std::string> Equipment;
        bool operator==(const CharacterProgression&) const = default;
    };

    struct CrewRelationship
    {
        std::string CharacterId;
        std::string Role;
        int Trust = 0;
        bool Met = false;
        bool Available = false;
        bool Recruited = false;
        bool Aboard = false;
        bool operator==(const CrewRelationship&) const = default;
    };

    /** Ordinals match EOpeningLocation in the Unreal reference and are persisted. */
    enum class OpeningLocation : std::uint8_t
    {
        FamilyMerchantShip,
        OpenWater,
        OuterReef,
        DriftwoodBeach,
        DriftwoodCamp,
        MirasCove,
        MangroveShallows,
        KoasTradingPost,
        GalleonCove,
        LaLiberacion,
        OpenSea,
        RexaHarbor
    };

    /** Ordinals match EOpeningRaidState. DravenAboard is appended for save compatibility. */
    enum class OpeningRaidState : std::uint8_t
    {
        Peaceful,
        FleetDetected,
        Alert,
        Boarding,
        FirstEncounter,
        Taking,
        Overboard,
        Complete,
        DravenAboard
    };

    /** Ordinals match EWaterRecoveryState. */
    enum class WaterRecoveryState : std::uint8_t
    {
        Dry,
        Overboard,
        Swimming,
        OuterReef,
        BeachCrawl,
        Recovered
    };

    /** Map-independent state for the authored Chapter 1/2 physical route. */
    struct OpeningProgress
    {
        OpeningLocation Location = OpeningLocation::FamilyMerchantShip;
        OpeningRaidState RaidState = OpeningRaidState::Peaceful;
        WaterRecoveryState RecoveryState = WaterRecoveryState::Dry;
        std::set<std::string> FamilyInteractions;
        std::string BoardingEncounterId;
        int BoardersRequired = 0;
        int BoardersDefeated = 0;
        bool DravenBoarded = false;
        bool LaLiberacionBoarded = false;
        bool LaLiberacionHelmSecured = false;
        bool LaLiberacionClearedHarbor = false;
        bool operator==(const OpeningProgress&) const = default;
    };

    struct PlayerRuntimeSnapshot
    {
        bool Valid = false;
        std::string MissionId;
        /** Empty for legacy snapshots: the authored arrival spawn is used instead of Transform. */
        std::string SourceLevel;
        Transform WorldTransform;
        float HealthFraction = 1.0f;
        float StaminaFraction = 1.0f;
        bool operator==(const PlayerRuntimeSnapshot&) const = default;
    };

    struct NamedCrewMember
    {
        std::string StableId;
        bool Alive = true;
        bool Aboard = true;
        bool operator==(const NamedCrewMember&) const = default;
    };

    struct PhysicalChart
    {
        std::string ChartId;
        bool Acquired = false;
        bool JakeHasAnnotated = false;
        bool operator==(const PhysicalChart&) const = default;
    };

    struct ShipVoyageSnapshot
    {
        bool Valid = false;
        /** Map-local coordinates never carry into another level. */
        std::string SourceLevel;
        Transform WorldTransform;
        bool OwnedAndUnlocked = false;
        float Throttle = 0.0f;
        float HullIntegrity = 1000.0f;
        float HeadingDegrees = 0.0f;
        float CommandedHeadingDegrees = 0.0f;
        float ForwardSpeedMetresPerSecond = 0.0f;
        float WindDirectionDegrees = 90.0f;
        float WindStrengthMetresPerSecond = 6.0f;
        bool JakeInGreatCabin = false;
        int ActiveHands = 40;
        std::vector<NamedCrewMember> NamedCrew;
        std::vector<PhysicalChart> PhysicalCharts;
        bool operator==(const ShipVoyageSnapshot&) const = default;
    };

    struct WorldRulesSnapshot
    {
        bool Valid = false;
        std::int64_t TotalWorldMinutes = 0;
        int Chapter = 1;
        bool AutosaveSuppressed = false;
        bool operator==(const WorldRulesSnapshot&) const = default;
    };

    /**
     * Complete persistent campaign state. Field-for-field port of UDarkArisenSaveGame v9
     * except the quest-journal, progression-economy and living-NPC snapshots, which are not
     * ported yet (see Docs/O3DE_MIGRATION.md).
     */
    struct CampaignState
    {
        static constexpr int CurrentVersion = 9;

        int SaveVersion = CurrentVersion;
        std::string CurrentMission;
        int CurrentChapter = 1;
        std::vector<MissionRuntime> Missions;
        std::set<std::string> StoryFacts;
        std::set<std::string> WorldFacts;
        CharacterProgression Progression;
        PlayerRuntimeSnapshot PlayerRuntime;
        std::vector<CrewRelationship> Crew;
        std::string CurrentShip;
        ShipVoyageSnapshot LaLiberacionVoyage;
        WorldRulesSnapshot WorldRules;
        std::set<std::string> DefeatedBosses;
        std::set<std::string> ImportantShortcuts;
        std::set<std::string> DiscoveredDungeons;
        std::set<std::string> CompletedDungeons;
        std::set<std::string> RecoveredTreasures;
        std::map<std::string, std::string> QuestOutcomes;
        std::string CheckpointId;
        std::string SpawnId;
        OpeningProgress Opening;

        bool operator==(const CampaignState&) const = default;

        /** Fresh New Game state: first mission available, everything else locked. */
        static CampaignState NewGame();

        MissionState GetMissionState(std::string_view MissionId) const;
        bool HasFact(std::string_view Fact) const;
    };
}
