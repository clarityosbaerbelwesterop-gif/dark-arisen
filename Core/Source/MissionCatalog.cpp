#include "DarkArisen/Core/MissionCatalog.h"

#include <set>

namespace DarkArisen::Core
{
    const std::array<MissionDefinition, MissionCatalog::MissionCount>& MissionCatalog::Missions()
    {
        static constexpr std::array<MissionDefinition, MissionCount> Catalog = {{
            {"Main.C01.01.HomeWater", 1, ""},
            {"Main.C01.02.BlackSails", 1, "Main.C01.01.HomeWater"},
            {"Main.C01.03.TheTaking", 1, "Main.C01.02.BlackSails"},
            {"Main.C01.04.Undertow", 1, "Main.C01.03.TheTaking"},
            {"Main.C02.01.ShatteredCoast", 2, "Main.C01.04.Undertow"},
            {"Main.C02.02.AShipToTake", 2, "Main.C02.01.ShatteredCoast"},
            {"Main.C02.03.FirstWake", 2, "Main.C02.02.AShipToTake"},
            {"Main.C03.01.RexaHarbor", 3, "Main.C02.03.FirstWake"},
            {"Main.C03.02.SafeRoutes", 3, "Main.C03.01.RexaHarbor"},
            {"Main.C03.03.TheFirstHolder", 3, "Main.C03.02.SafeRoutes"},
            {"Main.C04.01.SaltAndIron", 4, "Main.C03.03.TheFirstHolder"},
            {"Main.C04.02.BrokenCompact", 4, "Main.C04.01.SaltAndIron"},
            {"Main.C04.03.HerrerasFall", 4, "Main.C04.02.BrokenCompact"},
            {"Main.C05.01.WarCurrent", 5, "Main.C04.03.HerrerasFall"},
            {"Main.C05.02.HoldersWake", 5, "Main.C05.01.WarCurrent"},
            {"Main.C05.03.NoSafeHarbor", 5, "Main.C05.02.HoldersWake"},
            {"Main.C06.01.HighmooreRoad", 6, "Main.C05.03.NoSafeHarbor"},
            {"Main.C06.02.CrystalPassage", 6, "Main.C06.01.HighmooreRoad"},
            {"Main.C06.03.TheNorthernOath", 6, "Main.C06.02.CrystalPassage"},
            {"Main.C07.01.FalseBearings", 7, "Main.C06.03.TheNorthernOath"},
            {"Main.C07.02.EthansMarks", 7, "Main.C07.01.FalseBearings"},
            {"Main.C07.03.ThroughTheNet", 7, "Main.C07.02.EthansMarks"},
            {"Main.C08.01.ThePrisonCourse", 8, "Main.C07.03.ThroughTheNet"},
            {"Main.C08.02.BrothersAlive", 8, "Main.C08.01.ThePrisonCourse"},
            {"Main.C08.03.HomewardBearing", 8, "Main.C08.02.BrothersAlive"},
            {"Main.C09.01.EthansGrove", 9, "Main.C08.03.HomewardBearing"},
            {"Main.C09.02.MemoryOfABrother", 9, "Main.C09.01.EthansGrove"},
            {"Main.C09.03.DreamFight", 9, "Main.C09.02.MemoryOfABrother"},
            {"Main.C09.04.WakingCourse", 9, "Main.C09.03.DreamFight"},
            {"Main.C10.01.Armada", 10, "Main.C09.04.WakingCourse"},
            {"Main.C10.02.BreakTheChain", 10, "Main.C10.01.Armada"},
            {"Main.C10.03.BlackDeck", 10, "Main.C10.02.BreakTheChain"},
            {"Main.C10.04.DravenVoss", 10, "Main.C10.03.BlackDeck"},
            {"Main.C10.05.TheWakeAfter", 10, "Main.C10.04.DravenVoss"},
        }};
        return Catalog;
    }

    int MissionCatalog::FindIndex(const std::string_view MissionId)
    {
        const auto& Catalog = Missions();
        for (int Index = 0; Index < MissionCount; ++Index)
        {
            if (Catalog[static_cast<std::size_t>(Index)].Id == MissionId)
            {
                return Index;
            }
        }
        return -1;
    }

    bool MissionCatalog::Validate(std::vector<std::string>& OutErrors)
    {
        OutErrors.clear();
        std::set<std::string_view> Seen;
        std::array<int, ChapterCount> Distribution{};
        int PreviousChapter = 1;
        for (const MissionDefinition& Mission : Missions())
        {
            if (Mission.Id.empty() || !Seen.insert(Mission.Id).second)
            {
                OutErrors.emplace_back("Mission IDs must be unique and non-empty.");
            }
            if (Mission.Chapter < 1 || Mission.Chapter > ChapterCount)
            {
                OutErrors.emplace_back("Invalid chapter.");
                continue;
            }
            if (Mission.Chapter < PreviousChapter)
            {
                OutErrors.emplace_back("Chapters must be non-decreasing in catalog order.");
            }
            PreviousChapter = Mission.Chapter;
            ++Distribution[static_cast<std::size_t>(Mission.Chapter - 1)];
            if (!Mission.Prerequisite.empty() && Seen.count(Mission.Prerequisite) == 0)
            {
                OutErrors.emplace_back(std::string(Mission.Id) + " has an orphan/forward prerequisite.");
            }
            std::string Lower(Mission.Id);
            for (char& Character : Lower)
            {
                Character = static_cast<char>(Character >= 'A' && Character <= 'Z' ? Character + 32 : Character);
            }
            if (Lower.find("ethanboss") != std::string::npos || Lower.find("ethan.boss") != std::string::npos ||
                Lower.find("ethanbetrayal") != std::string::npos)
            {
                OutErrors.emplace_back("Legacy Ethan boss path is forbidden.");
            }
        }
        if (Missions().front().Prerequisite.size() != 0)
        {
            OutErrors.emplace_back("The first mission cannot have a prerequisite.");
        }
        if (Distribution != MissionsPerChapter)
        {
            OutErrors.emplace_back("Chapter distribution mismatch.");
        }
        return OutErrors.empty();
    }

    std::string_view MissionCatalog::LevelFor(const std::string_view MissionId)
    {
        struct Entry
        {
            std::string_view Mission;
            std::string_view Level;
        };
        static constexpr Entry Levels[] = {
            {"Main.C01.01.HomeWater", "L_HarlowOpening"},
            {"Main.C01.02.BlackSails", "L_HarlowOpening"},
            {"Main.C01.03.TheTaking", "L_HarlowOpening"},
            {"Main.C01.04.Undertow", "L_DriftwoodBeach"},
            {"Main.C02.01.ShatteredCoast", "L_DriftwoodBeach"},
            {"Main.C02.02.AShipToTake", "L_GalleonCove"},
            {"Main.C02.03.FirstWake", "L_OpenSea_FirstWake"},
            {"Main.C03.01.RexaHarbor", "L_RexaHarbor"},
            {"Main.C03.02.SafeRoutes", "L_RexaSafeRoutes"},
            {"Main.C03.03.TheFirstHolder", "L_CrownCitadelApproach"},
            {"Main.C04.01.SaltAndIron", "L_SaltAndIron_Blockade"},
            {"Main.C04.02.BrokenCompact", "L_BrokenCompact"},
            {"Main.C04.03.HerrerasFall", "L_HerrerasFall"},
            {"Main.C05.01.WarCurrent", "L_WarCurrent"},
            {"Main.C05.02.HoldersWake", "L_HoldersWake"},
            {"Main.C05.03.NoSafeHarbor", "L_NoSafeHarbor"},
            {"Main.C06.01.HighmooreRoad", "L_HighmooreRoad"},
            {"Main.C06.02.CrystalPassage", "L_CrystalPassage"},
            {"Main.C06.03.TheNorthernOath", "L_TheNorthernOath"},
            {"Main.C07.01.FalseBearings", "L_FalseBearings"},
            {"Main.C07.02.EthansMarks", "L_EthansMarks"},
            {"Main.C07.03.ThroughTheNet", "L_ThroughTheNet"},
            {"Main.C08.01.ThePrisonCourse", "L_ThePrisonCourse"},
            {"Main.C08.02.BrothersAlive", "L_BrothersAlive"},
            {"Main.C08.03.HomewardBearing", "L_HomewardBearing"},
            {"Main.C09.01.EthansGrove", "L_EthansGrove"},
            {"Main.C09.02.MemoryOfABrother", "L_MemoryOfABrother"},
            {"Main.C09.03.DreamFight", "L_DreamFight"},
            {"Main.C09.04.WakingCourse", "L_WakingCourse"},
            {"Main.C10.01.Armada", "L_Armada"},
            {"Main.C10.02.BreakTheChain", "L_BreakTheChain"},
            {"Main.C10.03.BlackDeck", "L_BlackDeck"},
            {"Main.C10.04.DravenVoss", "L_DravenVoss"},
            {"Main.C10.05.TheWakeAfter", "L_TheWakeAfter"},
        };
        for (const Entry& Candidate : Levels)
        {
            if (Candidate.Mission == MissionId)
            {
                return Candidate.Level;
            }
        }
        return {};
    }

    std::string_view ToString(const MissionState State)
    {
        switch (State)
        {
        case MissionState::Locked: return "Locked";
        case MissionState::Available: return "Available";
        case MissionState::Active: return "Active";
        case MissionState::Completed: return "Completed";
        case MissionState::Failed: return "Failed";
        }
        return "Unknown";
    }
}
