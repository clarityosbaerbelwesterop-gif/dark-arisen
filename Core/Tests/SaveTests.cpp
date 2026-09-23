#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/Facts.h"
#include "DarkArisen/Core/SaveCodec.h"
#include "DarkArisen/Core/SaveRules.h"
#include "DarkArisen/Core/ShipVoyage.h"

#include <limits>

using namespace DarkArisen::Core;
using DarkArisen::Test::AdvanceUntil;
using DarkArisen::Test::CompleteCampaign;

namespace
{
    CampaignState MidCampaignState()
    {
        CampaignRuntime Campaign;
        AdvanceUntil(Campaign, "Main.C03.02.SafeRoutes");
        Campaign.ActivateMission("Main.C03.02.SafeRoutes");
        PlayerRuntimeSnapshot Player;
        Player.Valid = true;
        Player.SourceLevel = "Levels/Rexa/RexaHarbor";
        Player.WorldTransform.Translation = {1204.25, -88.5, 12.0};
        Player.WorldTransform.Rotation = {0.0, 0.0, 0.3826834, 0.9238795};
        Player.HealthFraction = 0.62f;
        Player.StaminaFraction = 0.4f;
        Campaign.CapturePlayerRuntime(Player);
        ShipVoyage Ship;
        Ship.SetOwnedAndUnlocked();
        Ship.SetJakeAtHelm(true);
        Ship.SetThrottle(0.75f);
        Ship.AcquirePhysicalChart("chart.harlow.family");
        Campaign.CaptureShipVoyage(Ship.Capture("Levels/Rexa/RexaHarbor", Transform{{50.0, 60.0, 0.0}, {}, {1, 1, 1}}));
        Campaign.SetQuestOutcome("Rexa.Broker", "Spared");
        Campaign.DiscoverDungeon("dungeon.rexa.cistern");
        return Campaign.State();
    }

    std::vector<std::string> LoadErrors(const CampaignState& Candidate)
    {
        CampaignRuntime Campaign;
        std::vector<std::string> Errors;
        Campaign.LoadState(Candidate, Errors);
        return Errors;
    }
}

TEST_CASE("Save: encode/decode round-trips byte-identically")
{
    const CampaignState Original = MidCampaignState();
    const std::string Encoded = SaveCodec::Encode(Original);
    CampaignState Decoded;
    std::vector<std::string> Errors;
    CHECK(SaveCodec::Decode(Encoded, Decoded, Errors));
    CHECK(Errors.empty());
    CHECK(Decoded == Original);
    CHECK_EQ(SaveCodec::Encode(Decoded), Encoded);

    CampaignRuntime Loaded;
    CHECK(Loaded.LoadState(Decoded, Errors));
    CHECK(Loaded.State() == Original);
}

TEST_CASE("Save: full campaign state survives encode/decode/load")
{
    CampaignRuntime Campaign;
    CHECK(CompleteCampaign(Campaign));
    CampaignState Decoded;
    std::vector<std::string> Errors;
    CHECK(SaveCodec::Decode(SaveCodec::Encode(Campaign.State()), Decoded, Errors));
    CampaignRuntime Loaded;
    CHECK(Loaded.LoadState(Decoded, Errors));
    CHECK(Loaded.HasFact(Facts::MainComplete));
    CHECK(Loaded.HasFact(Facts::EthanRecovered));
}

TEST_CASE("Save: corrupt, truncated and tampered files are rejected")
{
    const std::string Encoded = SaveCodec::Encode(MidCampaignState());
    CampaignState Out;
    std::vector<std::string> Errors;

    std::string FlippedByte = Encoded;
    FlippedByte[FlippedByte.size() / 2] ^= 0x01;
    CHECK(!SaveCodec::Decode(FlippedByte, Out, Errors));

    CHECK(!SaveCodec::Decode(Encoded.substr(0, Encoded.size() - 7), Out, Errors));
    CHECK(!SaveCodec::Decode("", Out, Errors));
    CHECK(!SaveCodec::Decode("DARKARISEN-SAVE 2\nchecksum 0000000000000000\n", Out, Errors));

    // A consistent checksum over a body with an unknown record is still rejected.
    const std::string Header = "DARKARISEN-SAVE 1\nchecksum ";
    const std::string Body = Encoded.substr(Encoded.find('\n', Header.size()) + 1) + "cheat\tgold\n";
    char Checksum[17];
    std::snprintf(Checksum, sizeof(Checksum), "%016llx", static_cast<unsigned long long>(SaveCodec::Checksum(Body)));
    CHECK(!SaveCodec::Decode(Header + Checksum + "\n" + Body, Out, Errors));
    CHECK(!Errors.empty());

    const std::string Duplicate = Encoded.substr(Encoded.find('\n', Header.size()) + 1) + "current_chapter\t9\n";
    std::snprintf(Checksum, sizeof(Checksum), "%016llx", static_cast<unsigned long long>(SaveCodec::Checksum(Duplicate)));
    CHECK(!SaveCodec::Decode(Header + Checksum + "\n" + Duplicate, Out, Errors));
}

TEST_CASE("Save: skipped prerequisites and wrong mission order are rejected")
{
    CampaignState Skipped = CampaignState::NewGame();
    Skipped.Missions[3].State = MissionState::Completed;
    CHECK(!LoadErrors(Skipped).empty());

    CampaignState Reordered = CampaignState::NewGame();
    std::swap(Reordered.Missions[1], Reordered.Missions[2]);
    CHECK(!LoadErrors(Reordered).empty());

    CampaignState Renamed = CampaignState::NewGame();
    Renamed.Missions[9].MissionId = "Main.C09.03.EthanBoss";
    CHECK(!LoadErrors(Renamed).empty());

    CampaignState Short = CampaignState::NewGame();
    Short.Missions.pop_back();
    CHECK(!LoadErrors(Short).empty());

    CampaignState WrongCurrent = CampaignState::NewGame();
    WrongCurrent.CurrentMission = "Main.C10.04.DravenVoss";
    WrongCurrent.CurrentChapter = 10;
    CHECK(!LoadErrors(WrongCurrent).empty());

    CampaignState LockedNext = CampaignState::NewGame();
    LockedNext.Missions[0].State = MissionState::Locked;
    CHECK(!LoadErrors(LockedNext).empty());

    CampaignState TwoOpen = CampaignState::NewGame();
    TwoOpen.Missions[1].State = MissionState::Available;
    CHECK(!LoadErrors(TwoOpen).empty());
}

TEST_CASE("Save: non-finite or impossible vitals and ship state are rejected")
{
    CampaignState Base = MidCampaignState();
    CHECK(LoadErrors(Base).empty());

    CampaignState NaNTransform = Base;
    NaNTransform.PlayerRuntime.WorldTransform.Translation.X = std::numeric_limits<double>::quiet_NaN();
    CHECK(!LoadErrors(NaNTransform).empty());

    CampaignState DeadOnLoad = Base;
    DeadOnLoad.PlayerRuntime.HealthFraction = 0.0f;
    CHECK(!LoadErrors(DeadOnLoad).empty());

    CampaignState Overfull = Base;
    Overfull.PlayerRuntime.StaminaFraction = 1.5f;
    CHECK(!LoadErrors(Overfull).empty());

    CampaignState BadThrottle = Base;
    BadThrottle.LaLiberacionVoyage.Throttle = 3.0f;
    CHECK(!LoadErrors(BadThrottle).empty());

    CampaignState TooManyHands = Base;
    TooManyHands.LaLiberacionVoyage.ActiveHands = 400;
    CHECK(!LoadErrors(TooManyHands).empty());

    CampaignState NegativeClock = Base;
    NegativeClock.WorldRules.Valid = true;
    NegativeClock.WorldRules.TotalWorldMinutes = -5;
    CHECK(!LoadErrors(NegativeClock).empty());
}

TEST_CASE("Save: future versions are rejected and legacy versions migrate")
{
    CampaignState Future = CampaignState::NewGame();
    Future.SaveVersion = CampaignState::CurrentVersion + 1;
    CHECK(!LoadErrors(Future).empty());

    CampaignState Zero = CampaignState::NewGame();
    Zero.SaveVersion = 0;
    CHECK(!LoadErrors(Zero).empty());

    // v1 stored crew roles as quest outcomes and had no met/available beats.
    CampaignState Legacy = CampaignState::NewGame();
    Legacy.SaveVersion = 1;
    CrewRelationship Mira;
    Mira.CharacterId = "crew.mira";
    Mira.Recruited = true;
    Legacy.Crew.push_back(Mira);
    Legacy.QuestOutcomes["CrewRole.crew.mira"] = "FirstMate";
    CampaignRuntime Campaign;
    std::vector<std::string> Errors;
    CHECK(Campaign.LoadState(Legacy, Errors));
    CHECK_EQ(Campaign.State().SaveVersion, CampaignState::CurrentVersion);
    CHECK(Campaign.IsCrewMet("crew.mira"));
    CHECK_EQ(Campaign.State().Crew.front().Role, std::string("FirstMate"));

    // v8 player snapshots had no source level; their coordinates are dropped.
    CampaignState V8 = MidCampaignState();
    V8.SaveVersion = 8;
    CHECK(Campaign.LoadState(V8, Errors));
    CHECK(!Campaign.State().PlayerRuntime.Valid);
}

TEST_CASE("Save: failed load leaves the running campaign untouched")
{
    CampaignRuntime Campaign;
    CHECK(AdvanceUntil(Campaign, "Main.C02.01.ShatteredCoast"));
    const CampaignState Before = Campaign.State();
    CampaignState Corrupt = Before;
    Corrupt.StoryFacts.insert(std::string(Facts::EthanRecovered));
    std::vector<std::string> Errors;
    CHECK(!Campaign.LoadState(Corrupt, Errors));
    CHECK(Campaign.State() == Before);
}

TEST_CASE("World transfer: live state crosses maps without a disk save")
{
    // Map A: capture the live player and ship into the campaign (no SaveCodec involved).
    CampaignRuntime Campaign;
    CHECK(AdvanceUntil(Campaign, "Main.C02.03.FirstWake"));
    PlayerRuntimeSnapshot Player;
    Player.Valid = true;
    Player.SourceLevel = "Levels/Moran/GalleonCove";
    Player.WorldTransform.Translation = {10.0, 20.0, 1.0};
    Player.HealthFraction = 0.5f;
    CHECK(Campaign.CapturePlayerRuntime(Player));
    ShipVoyage Ship;
    Ship.SetOwnedAndUnlocked();
    CHECK(Campaign.CaptureShipVoyage(Ship.Capture("Levels/Moran/GalleonCove", Player.WorldTransform)));

    // Map B: vitals and voyage state carry over, map-local coordinates do not.
    const CampaignState& Carried = Campaign.State();
    CHECK_NEAR(Carried.PlayerRuntime.HealthFraction, 0.5, 1e-6);
    CHECK(!ShouldRestoreMapLocalTransform(Carried.PlayerRuntime.SourceLevel, "Levels/Rexa/RexaHarbor"));
    ShipVoyage Arrived;
    CHECK(!Arrived.Restore(Carried.LaLiberacionVoyage, "Levels/Rexa/RexaHarbor"));
    CHECK(Arrived.IsOwnedAndUnlocked());

    // Returning to map A restores the exact coordinates.
    CHECK(ShouldRestoreMapLocalTransform(Carried.PlayerRuntime.SourceLevel, "Levels/Moran/GalleonCove"));
    ShipVoyage Returned;
    CHECK(Returned.Restore(Carried.LaLiberacionVoyage, "Levels/Moran/GalleonCove"));
    // Legacy snapshots without a source level never restore coordinates.
    CHECK(!ShouldRestoreMapLocalTransform("", ""));
}
