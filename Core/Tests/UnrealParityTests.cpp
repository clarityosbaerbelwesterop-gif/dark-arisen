#include "TestHarness.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/Combat.h"
#include "DarkArisen/Core/DesignLaws.h"
#include "DarkArisen/Core/MissionCatalog.h"

#include <fstream>
#include <regex>
#include <sstream>

/**
 * The Unreal implementation stays the reference until O3DE proves itself. These tests
 * read its sources and fail when a number or ID in the engine-neutral core drifts.
 */
using namespace DarkArisen::Core;

namespace
{
    std::string ReadRepoFile(const std::string& RelativePath)
    {
        std::ifstream Stream(std::string(DARKARISEN_REPO_ROOT) + "/" + RelativePath, std::ios::binary);
        std::ostringstream Buffer;
        Buffer << Stream.rdbuf();
        return Buffer.str();
    }

    double FloatConstant(const std::string& Source, const std::string& Name)
    {
        const std::regex Pattern(Name + R"(\s*=\s*([0-9.]+)f?\s*;)");
        std::smatch Match;
        return std::regex_search(Source, Match, Pattern) ? std::stod(Match[1].str()) : -1.0;
    }
}

TEST_CASE("Parity: mission catalog matches MainStoryMissionCatalog.cpp")
{
    const std::string Source = ReadRepoFile("Source/DarkArisen/Story/MainStoryMissionCatalog.cpp");
    CHECK(!Source.empty());
    const std::regex Entry(R"re(Mission\(TEXT\("([^"]+)"\),\s*(\d+)(?:,\s*TEXT\("([^"]+)"\))?\))re");
    std::size_t Index = 0;
    for (auto It = std::sregex_iterator(Source.begin(), Source.end(), Entry); It != std::sregex_iterator(); ++It, ++Index)
    {
        if (Index >= MissionCatalog::Missions().size())
        {
            break;
        }
        const MissionDefinition& Core = MissionCatalog::Missions()[Index];
        CHECK_EQ(std::string(Core.Id), (*It)[1].str());
        CHECK_EQ(Core.Chapter, std::stoi((*It)[2].str()));
        CHECK_EQ(std::string(Core.Prerequisite), (*It)[3].str());
    }
    CHECK_EQ(Index, std::size_t{34});
}

TEST_CASE("Parity: design laws and tuning match DesignLaws.h / CoreLoopTuning.h")
{
    const std::string Laws = ReadRepoFile("Source/DarkArisen/DesignLaws.h");
    const std::string Tuning = ReadRepoFile("Source/DarkArisen/CoreLoopTuning.h");
    CHECK(!Laws.empty() && !Tuning.empty());
    CHECK_EQ(FloatConstant(Laws, "TargetFramesPerSecond"), 60.0);
    CHECK_EQ(FloatConstant(Laws, "DeflectionWindowFrames"), 6.0);
    CHECK_NEAR(FloatConstant(Laws, "RacheTimeScale"), DesignLaws::RacheTimeScale, 1e-6);
    CHECK_NEAR(FloatConstant(Laws, "RacheMaximumRealSeconds"), DesignLaws::RacheMaximumRealSeconds, 1e-6);
    CHECK_NEAR(FloatConstant(Laws, "InteractionPromptRangeMetres"), DesignLaws::InteractionPromptRangeMetres, 1e-6);

    using namespace CoreLoopTuning;
    const std::pair<const char*, double> Values[] = {{"PostureSetThreshold", PostureSetThreshold},
        {"PosturePressedThreshold", PosturePressedThreshold}, {"PostureFailingThreshold", PostureFailingThreshold},
        {"PostureBrokenImminentThreshold", PostureBrokenImminentThreshold},
        {"PostureBreakVulnerabilitySeconds", PostureBreakVulnerabilitySeconds},
        {"PostureResetFraction", 0.5}, {"PostureRegenDelaySeconds", PostureRegenDelaySeconds},
        {"PostureRegenPerSecond", PostureRegenPerSecond}, {"LightStartupFrames", LightStartupFrames},
        {"LightRecoveryFrames", LightRecoveryFrames}, {"MediumStartupFrames", MediumStartupFrames},
        {"MediumRecoveryFrames", MediumRecoveryFrames}, {"HeavyStartupFrames", HeavyStartupFrames},
        {"HeavyRecoveryFrames", HeavyRecoveryFrames}, {"GreatStartupFrames", GreatStartupFrames},
        {"GreatRecoveryFrames", GreatRecoveryFrames}, {"PolearmStartupFrames", PolearmStartupFrames},
        {"PolearmRecoveryFrames", PolearmRecoveryFrames}};
    for (const auto& [Name, Value] : Values)
    {
        CHECK_NEAR(FloatConstant(Tuning, Name), Value, 1e-6);
    }
}

TEST_CASE("Parity: stamina costs, hit profiles and dodge frames match CombatComponent.cpp")
{
    const std::string Source = ReadRepoFile("Source/DarkArisen/Components/CombatComponent.cpp");
    CHECK_NEAR(FloatConstant(Source, "LightAttackStaminaCost"), CombatModel::LightAttackStaminaCost, 1e-6);
    CHECK_NEAR(FloatConstant(Source, "HeavyAttackStaminaCost"), CombatModel::HeavyAttackStaminaCost, 1e-6);
    CHECK_NEAR(FloatConstant(Source, "ParryStaminaCost"), CombatModel::ParryStaminaCost, 1e-6);
    CHECK_NEAR(FloatConstant(Source, "DodgeStaminaCost"), CombatModel::DodgeStaminaCost, 1e-6);
    CHECK_NEAR(FloatConstant(Source, "BackstepStaminaCost"), CombatModel::BackstepStaminaCost, 1e-6);
    CHECK_NEAR(FloatConstant(Source, "RacheDrainPerRealSecond"), CombatModel::RacheDrainPerRealSecond, 1e-6);
    CHECK(Source.find("FramesToSeconds(" + std::to_string(CoreLoopTuning::DodgeInvulnerabilityFrames) + ")") !=
        std::string::npos);

    const std::pair<const char*, HitKind> Kinds[] = {{"Light", HitKind::Light}, {"Heavy", HitKind::Heavy},
        {"ParryStrike", HitKind::ParryStrike}, {"Critical", HitKind::Critical}};
    for (const auto& [Name, Kind] : Kinds)
    {
        const std::regex Pattern(std::string(R"(ECombatHitKind::)") + Name +
            R"(:\s*return\s*\{([0-9.]+)f,\s*([0-9.]+)f,\s*([0-9.]+)f\})");
        std::smatch Match;
        CHECK(std::regex_search(Source, Match, Pattern));
        if (Match.size() == 4)
        {
            const HitProfile Profile = CombatModel::GetHitProfile(Kind);
            CHECK_NEAR(std::stod(Match[1].str()), Profile.HealthDamage, 1e-6);
            CHECK_NEAR(std::stod(Match[2].str()), Profile.PostureDamage, 1e-6);
            CHECK_NEAR(std::stod(Match[3].str()), Profile.DeflectedPostureDamage, 1e-6);
        }
    }
}

TEST_CASE("Parity: canonical boss registry matches MainStorySubsystem.cpp")
{
    const std::string Source = ReadRepoFile("Source/DarkArisen/Story/MainStorySubsystem.cpp");
    for (const std::string_view Id : CampaignRuntime::CanonicalBossIds())
    {
        CHECK(Source.find("TEXT(\"" + std::string(Id) + "\")") != std::string::npos);
    }
}

TEST_CASE("Parity: mission levels match MainStoryMapCatalog.cpp")
{
    const std::string Source = ReadRepoFile("Source/DarkArisen/Story/MainStoryMapCatalog.cpp");
    CHECK(!Source.empty());
    const std::regex Entry(R"re(\{TEXT\("(Main\.[^"]+)"\),TEXT\("([^"]+)"\)\})re");
    std::size_t Count = 0;
    for (auto It = std::sregex_iterator(Source.begin(), Source.end(), Entry); It != std::sregex_iterator(); ++It, ++Count)
    {
        CHECK_EQ(std::string(MissionCatalog::LevelFor((*It)[1].str())), (*It)[2].str());
    }
    CHECK_EQ(Count, std::size_t{34});
    for (const MissionDefinition& Mission : MissionCatalog::Missions())
    {
        CHECK(!MissionCatalog::LevelFor(Mission.Id).empty());
    }
    CHECK(MissionCatalog::LevelFor("Main.C99.01.Unknown").empty());
}
