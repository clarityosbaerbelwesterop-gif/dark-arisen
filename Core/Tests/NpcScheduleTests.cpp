#include "TestHarness.h"

#include "DarkArisen/Core/NpcSchedule.h"

using namespace DarkArisen::Core;

namespace
{
    constexpr std::string_view Smith = "npc.test.smith";
}

TEST_CASE("Schedule: the blacksmith's day follows npc_system_core.md 3.2")
{
    const DayContext Day;
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(6, 40), Day, Smith).What == Activity::Work);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(12, 30), Day, Smith).What == Activity::Meal);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(18, 30), Day, Smith).Where == Anchor::Tavern);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(21, 30), Day, Smith).What == Activity::Home);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(23, 30), Day, Smith).What == Activity::Sleep);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(3), Day, Smith).What == Activity::Sleep);
}

TEST_CASE("Schedule: the tavern keeper works past midnight and sleeps late (3.2)")
{
    const DayContext Day;
    CHECK(ActivityAt(Occupation::TavernKeeper, Clock(0, 40), Day, "npc.test.keeper").Where == Anchor::Tavern);
    CHECK(ActivityAt(Occupation::TavernKeeper, Clock(4), Day, "npc.test.keeper").What == Activity::Sleep);
    CHECK(ActivityAt(Occupation::TavernKeeper, Clock(20), Day, "npc.test.keeper").What == Activity::Work);
}

TEST_CASE("Schedule: a storm keeps the fisherman ashore and a festival empties the workshops (3.5)")
{
    DayContext Day;
    CHECK(ActivityAt(Occupation::Fisherman, Clock(8), Day, "npc.test.fisher").Where == Anchor::Boat);
    Day.Storm = true;
    const ScheduleBlock Ashore = ActivityAt(Occupation::Fisherman, Clock(8), Day, "npc.test.fisher");
    CHECK(Ashore.What == Activity::Shelter && !Ashore.Outdoors);
    DayContext Fiesta;
    Fiesta.Festival = true;
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(14), Fiesta, Smith).What == Activity::Festival);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(23, 30), Fiesta, Smith).What == Activity::Sleep);
}

TEST_CASE("Schedule: the devout attend the service on a religious day (3.3)")
{
    DayContext Day;
    Day.ReligiousDay = true;
    CHECK(ActivityAt(Occupation::Standard, Clock(10), Day, "npc.test.a").What != Activity::Service);
    Day.Devout = true;
    CHECK(ActivityAt(Occupation::Standard, Clock(10), Day, "npc.test.a").Where == Anchor::Church);
}

TEST_CASE("Schedule: winter starts later and ends earlier, most of all in Fjordlund (3.4)")
{
    DayContext Winter;
    Winter.Time = Season::Winter;
    // at 6:40 the smith is at work in spring, still waking in winter
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(6, 40), DayContext{}, Smith).What == Activity::Work);
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(6, 40), Winter, Smith).What != Activity::Work);
    // at 20:30 he is in the tavern in spring, home in a Fjordlund winter
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(20, 30), DayContext{}, Smith).Where == Anchor::Tavern);
    Winter.Fjordlund = true;
    CHECK(ActivityAt(Occupation::Blacksmith, Clock(20, 30), Winter, Smith).Where == Anchor::Home);
}

TEST_CASE("Schedule: each NPC keeps its own fixed offset of at most fifteen minutes (3.6)")
{
    CHECK(PersonalOffsetMinutes("npc.a") == PersonalOffsetMinutes("npc.a"));
    for (const std::string_view Id : {"npc.a", "npc.b", "npc.rexa.smith", "npc.kaldvik.fisher"})
        CHECK(PersonalOffsetMinutes(Id) >= -15 && PersonalOffsetMinutes(Id) <= 15);
}
