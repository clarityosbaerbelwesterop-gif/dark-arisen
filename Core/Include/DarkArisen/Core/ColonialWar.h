#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** Ordinals match EColonialFaction in the Unreal reference and are persisted. */
    enum class ColonialFaction : std::uint8_t { Imperial, Albion, Liberation, Crimson };

    /** Ordinals match EWarActionVerb. */
    enum class WarActionVerb : std::uint8_t { Support, Break, Liberate };

    /** Ordinals match ERegionalWarOutcome and are persisted. */
    enum class RegionalWarOutcome : std::uint8_t
    {
        Contested,
        Crisis,
        ImperialControlled,
        AlbionControlled,
        Liberated,
        Vacuum,
        CrimsonOccupied
    };

    enum class WarMomentumPhase : std::uint8_t { Alpha, Beta, Gamma };

    /** Hidden simulation state for one region. Numbers never reach the player as a meter. */
    struct ColonialRegionState
    {
        int ImperialControl = 0;
        int AlbionControl = 0;
        int LiberationStrength = 0;
        int CrimsonThreat = 0;
        RegionalWarOutcome Outcome = RegionalWarOutcome::Contested;
        bool FallAssaultCompleted = false;
        /** Chapter of the last autonomous tick; one authored tick per region and chapter. */
        int LastAutonomousTickChapter = 0;
        bool operator==(const ColonialRegionState&) const = default;
    };

    /** Qualitative world read of a region (what physical content may react to). */
    struct RegionalWarSnapshot
    {
        std::string RegionId;
        RegionalWarOutcome Outcome = RegionalWarOutcome::Contested;
        ColonialFaction DominantFaction = ColonialFaction::Imperial;
        bool Crisis = false;
        bool FallAssaultCompleted = false;
    };

    struct AutonomousWarTick
    {
        int ImperialDelta = 0;
        int AlbionDelta = 0;
        int LiberationDelta = 0;
        int CrimsonDelta = 0;
    };

    /**
     * Persistent colonial-war state: port of UColonialWarStateSubsystem plus the holdings Jake has
     * claimed. The Unreal subsystem lived on the world and was lost on every map travel; here it
     * is part of the campaign state and the save.
     */
    struct ColonialWarState
    {
        std::map<std::string, ColonialRegionState, std::less<>> Regions;
        std::set<std::string, std::less<>> OwnedHoldings;
        bool operator==(const ColonialWarState&) const = default;
    };

    namespace ColonialWar
    {
        inline constexpr int MinimumControl = 0;
        inline constexpr int MaximumControl = 100;

        /** Seeds a region under its initial controller. Fails for an empty or known region. */
        bool RegisterRegion(ColonialWarState& War, int CurrentChapter, std::string_view RegionId, ColonialFaction InitialController);

        /** Reports an already completed physical Support/Break/Liberate action. Deltas are non-negative. */
        bool RecordResolvedWarAction(ColonialWarState& War, std::string_view RegionId, WarActionVerb Verb,
            ColonialFaction TargetFaction, int ControlDelta, int LiberationDelta, int CrimsonDelta);

        /** The war also moves without Jake: at most one authored tick per region and chapter. */
        bool RecordAutonomousChapterTick(ColonialWarState& War, int CurrentChapter, std::string_view RegionId, const AutonomousWarTick& Tick);

        /** A Fall cannot resolve without this authored assault completion. */
        bool RecordFallAssaultCompleted(ColonialWarState& War, std::string_view RegionId);

        /** Jake takes responsibility for a holding (ClaimHolding). Each holding once. */
        bool ClaimHolding(ColonialWarState& War, std::string_view HoldingId);

        WarMomentumPhase MomentumPhase(int Chapter);
        RegionalWarSnapshot Snapshot(const ColonialWarState& War, std::string_view RegionId);
        bool IsRegionLiberated(const ColonialWarState& War, std::string_view RegionId);

        /** Ranges and the outcome each region's numbers imply; used by save validation. */
        bool Validate(const ColonialWarState& War, std::vector<std::string>& OutErrors);

        /** Strict parsers for authored data: unknown names are errors, never a default faction. */
        std::optional<ColonialFaction> ParseFaction(std::string_view Name);
        std::optional<WarActionVerb> ParseVerb(std::string_view Name);
    }
}
