#include "TestHarness.h"

#include "DarkArisen/Core/MissionCatalog.h"

using namespace DarkArisen::Core;

TEST_CASE("Catalog: exactly 34 deterministic missions across ten chapters")
{
    std::vector<std::string> Errors;
    CHECK(MissionCatalog::Validate(Errors));
    CHECK(Errors.empty());
    CHECK_EQ(MissionCatalog::Missions().size(), std::size_t{34});
    CHECK_EQ(MissionCatalog::Missions().front().Id, std::string_view("Main.C01.01.HomeWater"));
    CHECK_EQ(MissionCatalog::Missions().back().Id, std::string_view("Main.C10.05.TheWakeAfter"));
    CHECK(MissionCatalog::Missions().front().Prerequisite.empty());
}

TEST_CASE("Catalog: every mission requires exactly its predecessor")
{
    const auto& Missions = MissionCatalog::Missions();
    for (std::size_t Index = 1; Index < Missions.size(); ++Index)
    {
        CHECK_EQ(Missions[Index].Prerequisite, Missions[Index - 1].Id);
        CHECK(Missions[Index].Chapter >= Missions[Index - 1].Chapter);
    }
}

TEST_CASE("Catalog: finale and Ethan chapters use the canonical IDs")
{
    CHECK_EQ(MissionCatalog::FindIndex("Main.C08.03.HomewardBearing"), 24);
    CHECK_EQ(MissionCatalog::FindIndex("Main.C09.03.DreamFight"), 27);
    CHECK_EQ(MissionCatalog::FindIndex("Main.C10.04.DravenVoss"), 32);
    CHECK_EQ(MissionCatalog::FindIndex("Main.C09.03.EthanBoss"), -1);
    CHECK_EQ(MissionCatalog::FindIndex(""), -1);
}
