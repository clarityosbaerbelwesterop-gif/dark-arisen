#pragma once

#include <cstdint>
#include <span>
#include <string_view>

/**
 * Daily schedules after docs/design/npcs/npc_system_core.md section 3: the standard day (3.1), the blacksmith, the
 * tavern keeper and the fisherman (3.2) to the minute, weekly patterns (3.3), seasons (3.4), storms and festivals (3.5),
 * and "predictable but not robotic" (3.6): each NPC keeps its own small, fixed offset. The engine moves an NPC to the
 * anchor of its current block; nothing here generates people.
 *
 * The document leaves some numbers open; the values chosen for them are marked as tuning values.
 */
namespace DarkArisen::Core
{
    enum class Activity : std::uint8_t
    {
        Sleep, Routine, Travel, Work, Meal, Rest, Errands, Social, Home, Tavern, Service, Festival, Shelter,
    };

    /** Where an activity happens; the settlement maps anchors to places. */
    enum class Anchor : std::uint8_t { Home, Workplace, Tavern, Church, Market, Boat, Docks, Square };

    struct ScheduleBlock
    {
        int StartMinute = 0;  // minute of the day the block begins, 0-1439
        Activity What = Activity::Sleep;
        Anchor Where = Anchor::Home;
        bool Outdoors = false;
    };

    enum class Occupation : std::uint8_t { Standard, Blacksmith, TavernKeeper, Fisherman };

    /** The blocks of an occupation's day, in order from midnight. */
    std::span<const ScheduleBlock> DailySchedule(Occupation Job);

    enum class Season : std::uint8_t { Spring, Summer, Autumn, Winter };

    struct DayContext
    {
        Season Time = Season::Spring;
        bool Storm = false;        // 3.5: outdoor work stops, people crowd indoors
        bool Festival = false;     // 3.5: schedules suspend, the community gathers
        bool ReligiousDay = false; // 3.3: services; the devout attend, some shops close
        bool Devout = false;
        bool Fjordlund = false;    // 3.4: winter bites hardest in Fjordlund
    };

    /** 3.6: a per-NPC offset of up to fifteen minutes either way, fixed by its id (tuning value). */
    int PersonalOffsetMinutes(std::string_view NpcId);

    /**
     * What the NPC does at a minute of the day. Seasons shift the day (winter: later starts and earlier nights,
     * an hour, two in Fjordlund; summer: the fisherman stays out an hour longer: tuning values), a storm sends outdoor
     * work to shelter, a festival suspends work for the square from 10:00 to 22:00, and on a religious day the devout
     * attend the service from 9:00 to 11:00 (the times are tuning values).
     */
    ScheduleBlock ActivityAt(Occupation Job, int MinuteOfDay, const DayContext& Day, std::string_view NpcId);

    constexpr int Clock(const int Hour, const int Minute = 0) { return Hour * 60 + Minute; }
}
