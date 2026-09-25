#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** Ordinals match EMainMissionState in the Unreal reference and are persisted. */
    enum class MissionState : std::uint8_t
    {
        Locked,
        Available,
        Active,
        Completed,
        Failed
    };

    struct MissionDefinition
    {
        std::string_view Id;
        int Chapter = 0;
        /** Single linear predecessor. Empty only for the first mission. */
        std::string_view Prerequisite;
    };

    /**
     * The one technical ID registry for the finite 34-mission campaign.
     * Mirrors Source/DarkArisen/Story/MainStoryMissionCatalog.cpp exactly (checked by
     * UnrealParityTests). Never generates or shuffles missions.
     */
    class MissionCatalog
    {
    public:
        static constexpr int MissionCount = 34;
        static constexpr int ChapterCount = 10;
        static constexpr std::array<int, ChapterCount> MissionsPerChapter = {4, 3, 3, 3, 3, 3, 3, 3, 4, 5};

        static const std::array<MissionDefinition, MissionCount>& Missions();
        /** Returns -1 for unknown IDs. */
        static int FindIndex(std::string_view MissionId);
        static bool Validate(std::vector<std::string>& OutErrors);
        /**
         * Level that hosts a mission (Unreal FMainStoryMapCatalog names, kept verbatim so saves and
         * reports line up across engines). Empty for unknown IDs: callers must fail closed.
         */
        static std::string_view LevelFor(std::string_view MissionId);
    };

    std::string_view ToString(MissionState State);
}
