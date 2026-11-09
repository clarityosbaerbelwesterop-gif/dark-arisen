#include "DarkArisen/Core/NpcSchedule.h"

#include <array>

namespace DarkArisen::Core
{
    namespace
    {
        using A = Activity;
        using W = Anchor;

        // 3.1: morning 5-12, midday 12-15, afternoon 15-18, evening 18-22, night 22-5
        constexpr std::array<ScheduleBlock, 10> Standard = {{
            {Clock(0), A::Sleep, W::Home, false},
            {Clock(6), A::Routine, W::Home, false},
            {Clock(7), A::Travel, W::Workplace, true},
            {Clock(7, 30), A::Work, W::Workplace, false},
            {Clock(12), A::Meal, W::Home, false},
            {Clock(13), A::Work, W::Workplace, false},
            {Clock(16, 30), A::Errands, W::Market, true},
            {Clock(18), A::Home, W::Home, false},
            {Clock(19, 30), A::Social, W::Tavern, false},
            {Clock(22), A::Sleep, W::Home, false},
        }};
        // 3.2 blacksmith: 5:30 wake and light the forge, 6:00 work, 8:00 customers, 12:00 lunch with family,
        // 13:00 work, 17:00 major work ends, 18:00 tavern, 21:00 home, 23:00 sleep
        constexpr std::array<ScheduleBlock, 9> Blacksmith = {{
            {Clock(0), A::Sleep, W::Home, false},
            {Clock(5, 30), A::Routine, W::Workplace, false},
            {Clock(6), A::Work, W::Workplace, false},
            {Clock(12), A::Meal, W::Home, false},
            {Clock(13), A::Work, W::Workplace, false},
            {Clock(17), A::Errands, W::Workplace, false},
            {Clock(18), A::Tavern, W::Tavern, false},
            {Clock(21), A::Home, W::Home, false},
            {Clock(23), A::Sleep, W::Home, false},
        }};
        // 3.2 tavern keeper: 8:00 wake, 9:00 breakfast service, 10-15 slow service and prep, 15-18 afternoon crowd,
        // 18-23 peak, 24:00 last call, 1:00 cleanup, 2:00 sleep
        constexpr std::array<ScheduleBlock, 4> TavernKeeper = {{
            {Clock(0), A::Work, W::Tavern, false},  // last call, then cleanup from 1:00
            {Clock(2), A::Sleep, W::Home, false},
            {Clock(8), A::Routine, W::Home, false},
            {Clock(9), A::Work, W::Tavern, false},
        }};
        // 3.2 fisherman: 4:00 wake, 4:30 boat ready, 5-12 fishing, 13:00 return and sell, 14-16 maintenance,
        // 17:00 family, 19:00 tavern (sometimes), 21:00 sleep
        constexpr std::array<ScheduleBlock, 10> Fisherman = {{
            {Clock(0), A::Sleep, W::Home, false},
            {Clock(4), A::Routine, W::Home, false},
            {Clock(4, 30), A::Work, W::Docks, true},
            {Clock(5), A::Work, W::Boat, true},
            {Clock(12), A::Travel, W::Docks, true},
            {Clock(13), A::Work, W::Market, true},
            {Clock(14), A::Work, W::Docks, true},
            {Clock(17), A::Home, W::Home, false},
            {Clock(19), A::Social, W::Tavern, false},
            {Clock(21), A::Sleep, W::Home, false},
        }};

        ScheduleBlock BlockAt(const std::span<const ScheduleBlock> Day, const int Minute)
        {
            ScheduleBlock Current = Day.front();
            for (const ScheduleBlock& Block : Day)
                if (Block.StartMinute <= Minute) Current = Block;
            return Current;
        }

        int Wrap(const int Minute) { return ((Minute % 1440) + 1440) % 1440; }
    }

    std::span<const ScheduleBlock> DailySchedule(const Occupation Job)
    {
        switch (Job)
        {
        case Occupation::Blacksmith: return Blacksmith;
        case Occupation::TavernKeeper: return TavernKeeper;
        case Occupation::Fisherman: return Fisherman;
        case Occupation::Standard: default: return Standard;
        }
    }

    int PersonalOffsetMinutes(const std::string_view NpcId)
    {
        std::uint32_t Hash = 2166136261u;
        for (const char C : NpcId) Hash = (Hash ^ static_cast<std::uint8_t>(C)) * 16777619u;
        return static_cast<int>(Hash % 31u) - 15;
    }

    ScheduleBlock ActivityAt(const Occupation Job, const int MinuteOfDay, const DayContext& Day, const std::string_view NpcId)
    {
        const int Minute = Wrap(MinuteOfDay);
        // 3.5: a festival suspends the day for the square
        if (Day.Festival && Minute >= Clock(10) && Minute < Clock(22)) return {Clock(10), A::Festival, W::Square, true};
        // 3.3: services on a religious day
        if (Day.ReligiousDay && Day.Devout && Minute >= Clock(9) && Minute < Clock(11)) return {Clock(9), A::Service, W::Church, false};

        // the NPC's own clock: its personal offset, and the season's later start (winter) shifting the morning
        int Shift = -PersonalOffsetMinutes(NpcId);
        if (Day.Time == Season::Winter) Shift -= Minute < Clock(14) ? (Day.Fjordlund ? 120 : 60) : 0;
        ScheduleBlock Block = BlockAt(DailySchedule(Job), Wrap(Minute + Shift));
        // winter: earlier nights, home and asleep an hour (two in Fjordlund) sooner
        if (Day.Time == Season::Winter && Job != Occupation::TavernKeeper)
        {
            const int Early = Day.Fjordlund ? 120 : 60;
            const ScheduleBlock Later = BlockAt(DailySchedule(Job), Wrap(Minute + Early));
            if (Minute >= Clock(14) && (Later.What == A::Sleep || Later.What == A::Home)) Block = Later;
        }
        // summer: the fisherman stays out an hour longer
        if (Day.Time == Season::Summer && Job == Occupation::Fisherman && Minute >= Clock(12) && Minute < Clock(13))
            Block = {Clock(12), A::Work, W::Boat, true};
        // 3.5: a storm stops outdoor work and crowds people indoors
        if (Day.Storm && Block.Outdoors && Block.What != A::Sleep) Block = {Block.StartMinute, A::Shelter, Job == Occupation::Fisherman ? W::Tavern : W::Home, false};
        return Block;
    }
}
