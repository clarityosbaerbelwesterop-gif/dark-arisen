#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/ColonialWar.h"
#include "DarkArisen/Core/NavalCombat.h"
#include "DarkArisen/Core/SaveCodec.h"
#include "DarkArisen/Core/SaveRules.h"
#include "DarkArisen/Core/StoryWorld.h"

using namespace DarkArisen::Core;

namespace
{
    StoryActorSpec Actor(const StoryActorKind Kind, const std::string& Name, const std::string& Anchor, const std::string& MissionId)
    {
        StoryActorSpec Spec;
        Spec.Kind = Kind;
        Spec.Name = Name;
        Spec.Anchor = Anchor;
        Spec.MissionId = MissionId;
        Spec.CheckpointId = "Checkpoint.Test";
        Spec.SpawnId = "Spawn.Test";
        return Spec;
    }

    /** A minimal valid contract for Main.C06.02.CrystalPassage (entry trigger + completing evidence). */
    StoryMissionContract CrystalPassage()
    {
        StoryMissionContract Contract;
        Contract.SourcePath = "test";
        Contract.MissionId = "Main.C06.02.CrystalPassage";
        Contract.MapId = "L_CrystalPassage";
        Contract.EntryAnchor = "Courtyard";
        Contract.CheckpointId = "Checkpoint.Test";
        Contract.SpawnId = "Spawn.Test";
        Contract.NextMission = "Main.C06.03.TheNorthernOath";
        Contract.Anchors = {{"Courtyard", {0, 0, 0}}, {"Room", {900, 0, 0}}};
        Contract.Actors.push_back(Actor(StoryActorKind::LocationTrigger, "Entry", "Courtyard", Contract.MissionId));
        StoryActorSpec Evidence = Actor(StoryActorKind::Evidence, "Report", "Room", Contract.MissionId);
        Evidence.Title = "Report";
        Evidence.Body = "Body";
        Evidence.CompletesMission = true;
        Contract.Actors.push_back(Evidence);
        return Contract;
    }

    bool HasErrorContaining(const std::vector<std::string>& Errors, const std::string& Text)
    {
        for (const std::string& Error : Errors)
        {
            if (Error.find(Text) != std::string::npos) return true;
        }
        return false;
    }
}

TEST_CASE("Colonial war: region seeding and resolved actions match the Unreal subsystem")
{
    ColonialWarState War;
    CHECK(ColonialWar::RegisterRegion(War, 4, "Rexa", ColonialFaction::Imperial));
    CHECK(!ColonialWar::RegisterRegion(War, 4, "Rexa", ColonialFaction::Albion));
    CHECK(!ColonialWar::RegisterRegion(War, 4, "", ColonialFaction::Albion));
    CHECK(ColonialWar::Snapshot(War, "Rexa").Outcome == RegionalWarOutcome::ImperialControlled);

    CHECK(ColonialWar::RecordResolvedWarAction(War, "Rexa", WarActionVerb::Break, ColonialFaction::Imperial, 18, 4, 2));
    const ColonialRegionState& Region = War.Regions.at("Rexa");
    CHECK_EQ(Region.ImperialControl, 52);
    CHECK_EQ(Region.LiberationStrength, 4);
    CHECK_EQ(Region.CrimsonThreat, 2);
    CHECK(!ColonialWar::RecordResolvedWarAction(War, "Rexa", WarActionVerb::Support, ColonialFaction::Imperial, -1, 0, 0));
    CHECK(!ColonialWar::RecordResolvedWarAction(War, "Nowhere", WarActionVerb::Support, ColonialFaction::Imperial, 1, 0, 0));

    // Break the colonial powers below 30: crisis until the Fall assault resolves it.
    CHECK(ColonialWar::RecordResolvedWarAction(War, "Rexa", WarActionVerb::Break, ColonialFaction::Imperial, 40, 0, 0));
    CHECK(ColonialWar::Snapshot(War, "Rexa").Crisis);
    CHECK(ColonialWar::RecordResolvedWarAction(War, "Rexa", WarActionVerb::Liberate, ColonialFaction::Imperial, 0, 60, 0));
    CHECK(!ColonialWar::IsRegionLiberated(War, "Rexa"));
    CHECK(ColonialWar::RecordFallAssaultCompleted(War, "Rexa"));
    CHECK(ColonialWar::IsRegionLiberated(War, "Rexa"));
    CHECK(ColonialWar::Snapshot(War, "Rexa").DominantFaction == ColonialFaction::Liberation);

    std::vector<std::string> Errors;
    CHECK(ColonialWar::Validate(War, Errors));
    War.Regions.at("Rexa").Outcome = RegionalWarOutcome::ImperialControlled;
    CHECK(!ColonialWar::Validate(War, Errors));
}

TEST_CASE("Colonial war: one autonomous tick per region and chapter; strict parsing")
{
    ColonialWarState War;
    CHECK(ColonialWar::RegisterRegion(War, 4, "Quiet Coast", ColonialFaction::Albion));
    CHECK(ColonialWar::RecordAutonomousChapterTick(War, 4, "Quiet Coast", {-5, 5, 0, 0}));
    CHECK(!ColonialWar::RecordAutonomousChapterTick(War, 4, "Quiet Coast", {-5, 5, 0, 0}));
    CHECK(ColonialWar::RecordAutonomousChapterTick(War, 5, "Quiet Coast", {0, 0, 0, 101}) == false);
    CHECK(ColonialWar::MomentumPhase(6) == WarMomentumPhase::Alpha);
    CHECK(ColonialWar::MomentumPhase(8) == WarMomentumPhase::Beta);
    CHECK(ColonialWar::MomentumPhase(9) == WarMomentumPhase::Gamma);
    CHECK(ColonialWar::ParseFaction("CrimsonArmada") == ColonialFaction::Crimson);
    CHECK(!ColonialWar::ParseFaction("Neutral").has_value());
    CHECK(!ColonialWar::ParseVerb("support").has_value());
}

TEST_CASE("Colonial war persists in the save (v10) and v9 saves migrate")
{
    CampaignRuntime Campaign;
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, "Main.C04.02.BrokenCompact"));
    CHECK(Campaign.RegisterWarRegion("Rexa", ColonialFaction::Imperial));
    CHECK(Campaign.RecordWarAction("Rexa", WarActionVerb::Support, ColonialFaction::Imperial, 12, 0, 0));
    CHECK(Campaign.ClaimHolding("Holding.C06.First"));
    CHECK(!Campaign.ClaimHolding("Holding.C06.First"));

    const std::string Encoded = SaveCodec::Encode(Campaign.State());
    CampaignState Decoded;
    std::vector<std::string> Errors;
    CHECK(SaveCodec::Decode(Encoded, Decoded, Errors));
    CHECK(Decoded == Campaign.State());
    CHECK_EQ(Decoded.ColonialWar.Regions.at("Rexa").ImperialControl, 82);

    CampaignState Legacy = Campaign.State();
    Legacy.SaveVersion = 9;
    Legacy.ColonialWar = ColonialWarState{};
    Errors.clear();
    CHECK(MigrateCampaignState(Legacy, Errors));
    CHECK_EQ(Legacy.SaveVersion, 10);

    CampaignState Corrupt = Campaign.State();
    Corrupt.ColonialWar.Regions.at("Rexa").AlbionControl = 140;
    CHECK(!ValidateCampaignState(Corrupt, Errors));
}

TEST_CASE("Story actions: location triggers activate, completion waits for living duelists")
{
    CampaignRuntime Campaign;
    const std::string Mission = "Main.C07.01.FalseBearings";
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, Mission));
    StoryActorSpec Entry = Actor(StoryActorKind::LocationTrigger, "Entry", "AlarmYard", Mission);
    StoryActorSpec Exit = Actor(StoryActorKind::LocationTrigger, "Exit", "Fallback", Mission);
    Exit.Action = LocationAction::CompleteMission;
    Exit.CheckpointId = "Checkpoint.C07.Exit";
    Exit.SpawnId = "Spawn.C07.Exit";

    CHECK(!StoryActions::OnLocationEntered(Campaign, Exit, false)); // not active yet
    CHECK(StoryActions::OnLocationEntered(Campaign, Entry, false));
    CHECK(Campaign.GetMissionState(Mission) == MissionState::Active);
    CHECK(!StoryActions::OnLocationEntered(Campaign, Entry, false)); // one activation
    CHECK(!StoryActions::OnLocationEntered(Campaign, Exit, true));   // fight still on
    CHECK(Campaign.GetMissionState(Mission) == MissionState::Active);
    CHECK(StoryActions::OnLocationEntered(Campaign, Exit, false));
    CHECK(Campaign.GetMissionState(Mission) == MissionState::Completed);
    CHECK_EQ(Campaign.State().CheckpointId, std::string("Checkpoint.C07.Exit"));
}

TEST_CASE("Story actions: contacts talk, evidence waits for its boss and completes")
{
    CampaignRuntime Campaign;
    const std::string Mission = "Main.C04.03.HerrerasFall";
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, Mission));
    StoryActorSpec Contact = Actor(StoryActorKind::Contact, "Courier", "Gate", Mission);
    Contact.ContactId = "contact.courier";
    Contact.DisplayName = "Courier";
    Contact.Text = "The fort burned from the inside.";
    CHECK(!StoryActions::CanInteract(Campaign, Contact));
    CHECK(Campaign.ActivateMission(Mission));
    const StoryInteraction Talk = StoryActions::Interact(Campaign, Contact);
    CHECK(Talk.Accepted);
    CHECK_EQ(Talk.Speaker, std::string("Courier"));

    StoryActorSpec Chart = Actor(StoryActorKind::Evidence, "Chart", "Archive", Mission);
    Chart.Title = "Harlow chart";
    Chart.Body = "Marc's hand.";
    Chart.RequiredBossId = "boss.herrera";
    Chart.CompletesMission = true;
    CHECK(!StoryActions::Interact(Campaign, Chart).Accepted);
    CHECK(Campaign.MarkBossDefeated("boss.herrera"));
    const StoryInteraction Read = StoryActions::Interact(Campaign, Chart);
    CHECK(Read.Accepted);
    CHECK(Read.MissionCompleted);
    CHECK_EQ(Read.DocumentTitle, std::string("Harlow chart"));
    CHECK(Campaign.GetMissionState(Mission) == MissionState::Completed);
    CHECK(StoryActions::CanInteract(Campaign, Contact)); // contacts still talk after completion
}

TEST_CASE("Story actions: route outcomes are final, war actions move the war then complete")
{
    CampaignRuntime Campaign;
    const std::string Mission = "Main.C04.02.BrokenCompact";
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, Mission));
    CHECK(Campaign.ActivateMission(Mission));
    StoryActorSpec Break = Actor(StoryActorKind::WarAction, "Break", "BreakAction", Mission);
    Break.RegionId = "Rexa";
    Break.InitialController = ColonialFaction::Imperial;
    Break.Verb = WarActionVerb::Break;
    Break.TargetFaction = ColonialFaction::Imperial;
    Break.ControlDelta = 18;
    Break.LiberationDelta = 4;
    Break.CrimsonDelta = 2;
    Break.OutcomeKey = "WarChoice.Main.C04.02.BrokenCompact";
    Break.OutcomeValue = "BrokeImperialSupply";
    Break.Label = "Destroy the supply chain";
    const StoryInteraction Result = StoryActions::Interact(Campaign, Break);
    CHECK(Result.Accepted);
    CHECK(Result.MissionCompleted);
    CHECK_EQ(Campaign.GetColonialWar().Regions.at("Rexa").ImperialControl, 52);
    CHECK_EQ(Campaign.State().QuestOutcomes.at("WarChoice.Main.C04.02.BrokenCompact"), std::string("BrokeImperialSupply"));

    // Chapter 6: the holding claim variant.
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, "Main.C06.01.HighmooreRoad"));
    const std::string Holding = "Main.C06.01.HighmooreRoad";
    StoryActorSpec Claim = Actor(StoryActorKind::WarAction, "Claim", "HoldingHeart", Holding);
    Claim.HoldingId = "Holding.C06.First";
    Claim.CompletesMission = true;
    CHECK(!StoryActions::CanInteract(Campaign, Claim));
    CHECK(Campaign.ActivateMission(Holding));
    CHECK(StoryActions::Interact(Campaign, Claim).MissionCompleted);
    CHECK(Campaign.GetColonialWar().OwnedHoldings.count("Holding.C06.First") == 1);
    CHECK(Campaign.HasFact("Holding.FirstOwned"));
}

TEST_CASE("Story actions: naval gates need every hostile sunk, the fact and the ship in reach")
{
    CampaignRuntime Campaign;
    const std::string Mission = "Main.C03.02.SafeRoutes";
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, Mission));
    CHECK(Campaign.ActivateMission(Mission));
    StoryActorSpec Gate = Actor(StoryActorKind::NavalEncounterGate, "Gate", "Exit", Mission);
    Gate.RequiredDefeats = 1;
    Gate.RequiredWorldFact = "Evidence.C03.EthanShorthandRead";
    Gate.CompletionRadiusMetres = 18.0;
    CHECK(!StoryActions::UpdateNavalGate(Campaign, Gate, 0, 0, 0.0));
    CHECK(!StoryActions::UpdateNavalGate(Campaign, Gate, 1, 0, 0.0));
    CHECK(!StoryActions::UpdateNavalGate(Campaign, Gate, 1, 1, 0.0)); // fact missing
    CHECK(Campaign.SetWorldFact("Evidence.C03.EthanShorthandRead"));
    CHECK(!StoryActions::UpdateNavalGate(Campaign, Gate, 1, 1, 40.0)); // ship too far
    CHECK(StoryActions::UpdateNavalGate(Campaign, Gate, 1, 1, 10.0));
    CHECK(Campaign.GetMissionState(Mission) == MissionState::Completed);
}

TEST_CASE("Naval combat: broadsides, reload, sinking and side selection")
{
    NavalCombatant Player(NavalCombatProfile::LaLiberacion());
    NavalCombatant Escort(NavalCombatProfile::HostileEscort());
    NavalCombatant Ally(NavalCombatProfile::LaLiberacion());
    CHECK_NEAR(Player.FireBroadside(Broadside::Starboard, Escort), 192.0f, 1e-4f);
    CHECK_NEAR(Player.FireBroadside(Broadside::Starboard, Escort), 0.0f, 1e-6f); // reloading
    CHECK_NEAR(Player.FireBroadside(Broadside::Port, Ally), 0.0f, 1e-6f);        // no friendly fire
    for (int Volley = 0; Volley < 4; ++Volley)
    {
        Player.Tick(7.5f);
        Player.FireBroadside(Broadside::Starboard, Escort);
    }
    CHECK(Escort.IsSunk());
    Escort.Repair(100.0f);
    CHECK(Escort.IsSunk());

    // Ship at the origin facing +Y (right = +X): a hostile at +X is to starboard only.
    const std::vector<NavalContact> Contacts = {{50.0, 5.0, 0.0, false, false}, {-30.0, 0.0, 0.0, false, true}, {20.0, 0.0, 0.0, true, false}};
    CHECK_EQ(SelectBroadsideTarget({0, 0, 0}, {1.0, 0.0}, true, Broadside::Starboard, 90.0f, Contacts), 0);
    CHECK_EQ(SelectBroadsideTarget({0, 0, 0}, {1.0, 0.0}, true, Broadside::Port, 90.0f, Contacts), -1);
    CHECK_EQ(SelectBroadsideTarget({0, 0, 0}, {1.0, 0.0}, true, Broadside::Starboard, 40.0f, Contacts), -1);
}

TEST_CASE("Story contracts fail closed")
{
    std::vector<std::string> Errors;
    CHECK(ValidateStoryContract(CrystalPassage(), Errors));
    CHECK(Errors.empty());

    StoryMissionContract Unknown = CrystalPassage();
    Unknown.MissionId = "Main.C11.01.Radiant";
    CHECK(!ValidateStoryContract(Unknown, Errors));

    StoryMissionContract WrongMap = CrystalPassage();
    WrongMap.MapId = "L_Elsewhere";
    Errors.clear();
    CHECK(!ValidateStoryContract(WrongMap, Errors));
    CHECK(HasErrorContaining(Errors, "campaign loads L_CrystalPassage"));

    StoryMissionContract NoEntry = CrystalPassage();
    NoEntry.EntryAnchor = "Missing";
    Errors.clear();
    CHECK(!ValidateStoryContract(NoEntry, Errors));

    StoryMissionContract NoWayOut = CrystalPassage();
    NoWayOut.Actors.pop_back();
    Errors.clear();
    CHECK(!ValidateStoryContract(NoWayOut, Errors));
    CHECK(HasErrorContaining(Errors, "no physical way to complete"));

    StoryMissionContract Skip = CrystalPassage();
    Skip.NextMission = "Main.C10.05.TheWakeAfter";
    Errors.clear();
    CHECK(!ValidateStoryContract(Skip, Errors));

    StoryMissionContract Facts = CrystalPassage();
    Facts.CompletionFacts = {"Story.Invented"};
    Errors.clear();
    CHECK(!ValidateStoryContract(Facts, Errors));

    StoryMissionContract EarlyEthan = CrystalPassage();
    StoryActorSpec Ethan = Actor(StoryActorKind::EthanHarlow, "Ethan", "Room", EarlyEthan.MissionId);
    EarlyEthan.Actors.push_back(Ethan);
    Errors.clear();
    CHECK(!ValidateStoryContract(EarlyEthan, Errors));
    CHECK(HasErrorContaining(Errors, "real Ethan"));

    StoryMissionContract DreamOutside = CrystalPassage();
    StoryActorSpec Dream = Actor(StoryActorKind::HolderBoss, "Dream", "Room", DreamOutside.MissionId);
    Dream.BossId = "boss.dream_ethan";
    Dream.OutcomeKey = "DreamEthan.Outcome";
    Dream.OutcomeValue = "Accepted";
    DreamOutside.Actors.push_back(Dream);
    Errors.clear();
    CHECK(!ValidateStoryContract(DreamOutside, Errors));

    StoryMissionContract RealEthanBoss = CrystalPassage();
    StoryActorSpec Real = Actor(StoryActorKind::HolderBoss, "Real", "Room", RealEthanBoss.MissionId);
    Real.BossId = "character.ethan";
    Real.OutcomeKey = "x";
    Real.OutcomeValue = "y";
    RealEthanBoss.Actors.push_back(Real);
    Errors.clear();
    CHECK(!ValidateStoryContract(RealEthanBoss, Errors));
}
