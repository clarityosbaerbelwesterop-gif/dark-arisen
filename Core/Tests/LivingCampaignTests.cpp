#include "TestHarness.h"
#include "TestSupport.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/LivingWorld.h"
#include "DarkArisen/Core/OpeningRuntime.h"
#include "DarkArisen/Core/ProgressionEconomy.h"
#include "DarkArisen/Core/QuestJournal.h"
#include "DarkArisen/Core/SaveCodec.h"
#include "DarkArisen/Core/SaveRules.h"
#include "DarkArisen/Core/StoryWorld.h"

#include <algorithm>

using namespace DarkArisen::Core;

namespace
{
    QuestDefinition Quest(const std::string& Id, const std::string& EntryId, const std::string& Text)
    {
        QuestDefinition Definition;
        Definition.QuestId = Id;
        Definition.InitialEntryId = EntryId;
        Definition.InitialJakeText = Text;
        Definition.InitialDirections = "Past the fish market, second jetty.";
        return Definition;
    }
}

TEST_CASE("Quest journal: spoken agreement, mutual exclusion, silent reveal, correction and expiry")
{
    QuestCatalog Catalog;
    QuestDefinition Salvage = Quest("quest.salvage", "entry.salvage.1", "Ines wants the bell from the wreck.");
    Salvage.RequiresSpokenAgreement = true;
    Salvage.InitialEntryDistorted = true;
    Salvage.MutuallyExclusiveQuestIds = {"quest.burn"};
    Salvage.ExpirationDurationMinutes = 600;
    CHECK(Catalog.Register(Salvage));
    CHECK(Catalog.Register(Quest("quest.burn", "entry.burn.1", "Burn the wreck before the Crown finds it.")));
    QuestDefinition Whisper;
    Whisper.QuestId = "quest.whisper";
    Whisper.Trigger = QuestActivationTrigger::Overheard;
    Whisper.SilentAtActivation = true;
    CHECK(Catalog.Register(Whisper));
    QuestDefinition Bad = Quest("quest.bad", "entry.bad", "x");
    Bad.Trigger = QuestActivationTrigger::Object;
    Bad.RequiresSpokenAgreement = true; // agreement is spoken, never an object prompt
    CHECK(!Catalog.Register(Bad));
    CHECK(!Catalog.Register(Quest("quest.dup", "entry.burn.1", "Same page twice.")));

    QuestJournalState State;
    QuestJournal::AdoptCatalog(State, Catalog);
    CHECK(!QuestJournal::Activate(State, Catalog, "quest.salvage", QuestActivationTrigger::Conversation, 100, false));
    CHECK(QuestJournal::Activate(State, Catalog, "quest.salvage", QuestActivationTrigger::Conversation, 100, true));
    CHECK(State.Quests.at("quest.burn").Lifecycle == QuestLifecycle::Unavailable);
    CHECK(!QuestJournal::Activate(State, Catalog, "quest.burn", QuestActivationTrigger::Conversation, 110, false));
    CHECK_EQ(State.Quests.at("quest.salvage").ExpirationGameMinute, 700);

    CHECK(QuestJournal::Activate(State, Catalog, "quest.whisper", QuestActivationTrigger::Overheard, 120, false));
    CHECK(!QuestJournal::HasEntryForQuest(State, "quest.whisper"));
    CHECK(QuestJournal::RevealSilentQuest(State, Catalog, "quest.whisper", "entry.whisper.1", "Two sailors spoke of a black sail.", "", 130, false));
    CHECK(!QuestJournal::RevealSilentQuest(State, Catalog, "quest.whisper", "entry.whisper.2", "Again.", "", 131, false));

    CHECK(QuestJournal::AppendCorrection(State, "quest.salvage", "entry.salvage.fix", "It was the ship's bell, not a church bell.", "", 140));
    CHECK(!QuestJournal::AppendCorrection(State, "quest.whisper", "entry.whisper.fix", "Nothing was distorted here.", "", 141));
    CHECK_EQ(QuestJournal::Search(State, "  BELL ").size(), std::size_t{2});
    CHECK_EQ(QuestJournal::Search(State, "").size(), State.Entries.size());

    CHECK_EQ(QuestJournal::ProcessExpirations(State, 699), 0);
    CHECK_EQ(QuestJournal::ProcessExpirations(State, 700), 1);
    CHECK(State.Quests.at("quest.salvage").OutcomeId == "Expired");
    CHECK(!QuestJournal::Resolve(State, "quest.salvage", "Delivered", 710));
    CHECK(QuestJournal::Resolve(State, "quest.whisper", "SailSighted", 720));

    std::vector<std::string> Errors;
    CHECK(QuestJournal::Validate(State, Errors));
    for (std::size_t Index = 1; Index < State.Entries.size(); ++Index) CHECK(State.Entries[Index].Sequence > State.Entries[Index - 1].Sequence);
}

TEST_CASE("Economy: BODY only from authored sources, CRAFT needs marks and teaching, money never unlocks")
{
    const SkillCatalog Catalog = SkillCatalog::Canonical();
    ProgressionState State;
    for (int Index = 0; Index < 12; ++Index) CHECK(Progression::ApplyPhysiciansDraught(State, "draught." + std::to_string(Index)));
    CHECK(!Progression::ApplyPhysiciansDraught(State, "draught.13"));
    CHECK_EQ(State.MaximumHealth, 380);
    CHECK(Progression::ApplyDeepWaterPearl(State, "pearl.harlow"));
    CHECK(!Progression::ApplyDeepWaterPearl(State, "pearl.harlow"));
    CHECK_EQ(State.MaximumStamina, 125);
    CHECK(!Progression::RecordCarryMilestone(State, "carry.bad", 140.0));
    CHECK(Progression::RecordCarryMilestone(State, "carry.forge", 95.0));

    CHECK(!Progression::TryLearnNode(State, Catalog, "node.blade.big_toms_answer")); // no marks
    CHECK(Progression::AwardMarks(State, "marks.c01", 3));
    CHECK(!Progression::AwardMarks(State, "marks.c01", 3));
    CHECK(Progression::Credit(State, Currency::Doubloons, 5000));
    CHECK(!Progression::TryLearnNode(State, Catalog, "node.blade.big_toms_answer")); // money is not teaching
    CHECK(!Progression::CompleteTeachingScene(State, Catalog, "teacher.big_tom", "node.blade.big_toms_answer")); // not met yet
    CHECK(Progression::RecordTeacherMet(State, "teacher.big_tom"));
    CHECK(!Progression::RecordTeacherMet(State, "teacher.invented"));
    CHECK(Progression::CompleteTeachingScene(State, Catalog, "teacher.big_tom", "node.blade.big_toms_answer"));
    CHECK(Progression::TryLearnNode(State, Catalog, "node.blade.big_toms_answer"));
    CHECK(!Progression::TryLearnNode(State, Catalog, "node.blade.big_toms_answer"));
    CHECK_EQ(State.MarksSpent, 1);
    CHECK(!Progression::TryLearnNode(State, Catalog, "node.blade.long_night")); // posture 100 < 150

    std::vector<std::string> Errors;
    CHECK(Progression::Validate(State, Errors));
}

TEST_CASE("Economy: services are atomic, currencies never convert, the ledger resolves once per chapter")
{
    ProgressionState State;
    CharacterProgression Character;
    Character.Inventory = {"item.plank", "item.plank", "item.pitch"};
    ServiceOffer Repair{"service.koa.hull_patch", Currency::Doubloons, 40, {"item.plank", "item.plank", "item.pitch"}, {}, true};
    CHECK(!Progression::PurchaseService(State, Character, Repair)); // no money
    CHECK_EQ(Character.Inventory.size(), std::size_t{3});           // nothing consumed
    CHECK(Progression::Credit(State, Currency::Pounds, 1000));
    CHECK(!Progression::PurchaseService(State, Character, Repair)); // pounds are not doubloons
    CHECK(Progression::Credit(State, Currency::Doubloons, 50));
    CHECK(Progression::PurchaseService(State, Character, Repair));
    CHECK(Character.Inventory.empty());
    CHECK_EQ(Progression::Balance(State, Currency::Doubloons), 10);
    CHECK(!Progression::PurchaseService(State, Character, Repair)); // one-time work

    ServiceOffer Provisions{"service.koa.provisions", Currency::Doubloons, 5, {}, {"item.biscuit", "item.water"}, false};
    CHECK(Progression::PurchaseService(State, Character, Provisions));
    CHECK(Progression::PurchaseService(State, Character, Provisions));
    CHECK(!Progression::PurchaseService(State, Character, Provisions));
    CHECK_EQ(Character.Inventory.size(), std::size_t{4});

    CHECK(!Progression::SetPendingLedger(State, {100, 20, 0, 0})); // upkeep is never positive
    CHECK(Progression::SetPendingLedger(State, {120, -30, -10, 0}));
    CHECK(Progression::ResolveChapterLedger(State, 5));
    CHECK_EQ(Progression::Balance(State, Currency::Doubloons), 80);
    CHECK(!Progression::ResolveChapterLedger(State, 5));
    CHECK(Progression::SetPendingLedger(State, {0, -500, 0, 0}));
    CHECK(!Progression::ResolveChapterLedger(State, 6)); // cannot go into debt
    CHECK(Progression::PayCrewShare(State, 30, 6));
    CHECK(!Progression::PayCrewShare(State, 30, 6));
}

TEST_CASE("Social: a drawn weapon makes a place wary until chapters pass; presence never saves")
{
    CampaignRuntime Campaign;
    CHECK(Campaign.UpdateProgression([](ProgressionState& State, CharacterProgression&, const SkillCatalog&)
    {
        Progression::SetGreeting(State, "social.koa.tavern", GreetingState::Owed);
        return Progression::RecordWeaponDrawnHere(State, "social.koa.tavern", 1);
    }));
    CHECK(Progression::Greeting(Campaign.GetProgression(), "social.koa.tavern") == GreetingState::Wary);
    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, "Main.C02.01.ShatteredCoast"));
    CHECK(Progression::Greeting(Campaign.GetProgression(), "social.koa.tavern") == GreetingState::Owed);

    SocialPresence& Presence = Campaign.Social();
    CHECK(Presence.BeginSitting("seat.koa.bench"));
    CHECK(Presence.BeginListening("talk.koa.smugglers"));
    Presence.OnWeaponDrawn();
    CHECK(!Presence.IsSitting());
    ProgressionState Scratch;
    CHECK(!Presence.CompleteListening(Scratch));
    CHECK(Scratch.OverheardConversations.empty());
}

TEST_CASE("Living world: memories weigh reputation, spread by connection and decay; death is permanent")
{
    LivingWorldState World;
    CHECK(LivingWorld::RegisterNpc(World, "npc.koa", "community.koa"));
    CHECK(LivingWorld::RegisterNpc(World, "npc.koa_daughter", "community.koa"));
    CHECK(LivingWorld::RegisterNpc(World, "npc.fisher", "community.koa"));
    CHECK(LivingWorld::RegisterNpc(World, "npc.rexa_clerk", "community.rexa"));
    CHECK(LivingWorld::Connect(World, "npc.koa", "npc.koa_daughter", NpcConnectionKind::Family));
    CHECK(!LivingWorld::Connect(World, "npc.koa", "npc.koa", NpcConnectionKind::Friend));

    NpcMemory Saved;
    Saved.EventId = "event.koa.saved_from_press_gang";
    Saved.Emotion = NpcMemoryEmotion::Positive;
    Saved.OriginalWeight = 60;
    Saved.EffectiveWeight = 60.0;
    Saved.DecayPerGameDay = 10.0;
    CHECK(LivingWorld::RecordMemory(World, "npc.koa", Saved));
    CHECK_EQ(LivingWorld::Find(World, "npc.koa")->PersonalReputation, 30);
    CHECK_EQ(LivingWorld::PropagateMemory(World, "npc.koa", Saved.EventId), 2); // family edge + community
    CHECK_NEAR(LivingWorld::Find(World, "npc.koa_daughter")->Memories.front().EffectiveWeight, 51.0, 1e-9);
    CHECK_NEAR(LivingWorld::Find(World, "npc.fisher")->Memories.front().EffectiveWeight, 21.0, 1e-9);
    CHECK(LivingWorld::Find(World, "npc.rexa_clerk")->Memories.empty());

    CHECK(LivingWorld::SimulateToGameMinute(World, 1440 * 3));
    CHECK_NEAR(LivingWorld::Find(World, "npc.koa")->Memories.front().EffectiveWeight, 30.0, 1e-9);
    CHECK(!LivingWorld::SimulateToGameMinute(World, 10));
    CHECK(LivingWorld::SimulateToGameMinute(World, 1440 * 10));
    CHECK(LivingWorld::Find(World, "npc.koa")->Memories.empty());
    CHECK_EQ(LivingWorld::Find(World, "npc.koa")->PersonalReputation, 0);

    CHECK(LivingWorld::UpdateScheduleAnchor(World, "npc.fisher", "anchor.koa.jetty"));
    CHECK(LivingWorld::ResolveInteraction(World, "npc.fisher", "talk.fisher.nets"));
    CHECK(!LivingWorld::ResolveInteraction(World, "npc.fisher", "talk.fisher.nets"));
    CHECK(LivingWorld::SetAlive(World, "npc.fisher", false));
    CHECK(!LivingWorld::Find(World, "npc.fisher")->Available);
    CHECK(!LivingWorld::SetAlive(World, "npc.fisher", true));
    CHECK(!LivingWorld::SetAvailable(World, "npc.fisher", true));
    CHECK(!LivingWorld::UpdateScheduleAnchor(World, "npc.fisher", "anchor.koa.tavern"));

    std::vector<std::string> Errors;
    CHECK(LivingWorld::Validate(World, Errors));
    LivingWorldState Corrupt = World;
    Corrupt.Records.at("npc.fisher").Available = true;
    CHECK(!LivingWorld::Validate(Corrupt, Errors));
}

TEST_CASE("Save v11 carries journal, economy and living NPCs; evidence and observations persist; v10 migrates")
{
    CampaignRuntime Campaign;
    OpeningRuntime Opening(Campaign);
    for (const char* Family : {"character.marc", "character.denise", "character.ethan"}) CHECK(Opening.SignalFamilyInteraction(Family));
    const auto& Observations = Campaign.GetJournal().Observations;
    CHECK(std::find(Observations.begin(), Observations.end(), "Journal.HomeWater.FamilyRoutes") != Observations.end());

    CHECK(DarkArisen::Test::AdvanceUntil(Campaign, "Main.C06.02.CrystalPassage"));
    CHECK(Campaign.ActivateMission("Main.C06.02.CrystalPassage"));
    StoryActorSpec Report;
    Report.Kind = StoryActorKind::Evidence;
    Report.Name = "Report";
    Report.Anchor = "Room";
    Report.MissionId = "Main.C06.02.CrystalPassage";
    Report.CheckpointId = "Checkpoint.Test";
    Report.SpawnId = "Spawn.Test";
    Report.Title = "Surveyor's report";
    Report.Body = "The crystal seam runs north.\tIt sings at low tide.";
    CHECK(StoryActions::Interact(Campaign, Report).Accepted);
    CHECK(StoryActions::Interact(Campaign, Report).Accepted);
    CHECK_EQ(Campaign.GetJournal().Evidence.size(), std::size_t{1});
    CHECK(Campaign.GetJournal().Evidence.front().Body == Report.Body);

    CHECK(Campaign.RegisterQuest(Quest("quest.salvage", "entry.salvage.1", "Ines wants the bell.")));
    CHECK(!Campaign.RegisterQuest(Quest("quest.salvage", "entry.salvage.2", "Twice.")));
    CHECK(Campaign.UpdateJournal([](QuestJournalState& State, const QuestCatalog& Catalog)
    {
        return QuestJournal::Activate(State, Catalog, "quest.salvage", QuestActivationTrigger::Conversation, 42, false);
    }));
    CHECK(!Campaign.UpdateJournal([](QuestJournalState& State, const QuestCatalog&)
    {
        State.NextSequence = -5; // invalid results never commit
        return true;
    }));
    CHECK(Campaign.UpdateProgression([](ProgressionState& State, CharacterProgression& Character, const SkillCatalog& Catalog)
    {
        Character.Inventory.push_back("item.pitch");
        return Progression::ApplyPhysiciansDraught(State, "draught.salvio") && Progression::AwardMarks(State, "marks.c01", 2) &&
            Progression::RecordTeacherMet(State, "teacher.big_tom") &&
            Progression::CompleteTeachingScene(State, Catalog, "teacher.big_tom", "node.blade.big_toms_answer") &&
            Progression::TryLearnNode(State, Catalog, "node.blade.big_toms_answer") && Progression::Credit(State, Currency::SilverMarks, 12) &&
            Progression::PurchaseService(State, Character, {"service.koa.caulk", Currency::SilverMarks, 2, {"item.pitch"}, {}, true}) &&
            Progression::RecordWeaponDrawnHere(State, "social.rexa.market", 2) && Progression::RecordOverheard(State, "talk.rexa.dock");
    }));
    CHECK(Campaign.UpdateLivingWorld([](LivingWorldState& World)
    {
        NpcMemory Memory;
        Memory.EventId = "event.rexa.bell";
        Memory.Emotion = NpcMemoryEmotion::Negative;
        Memory.OriginalWeight = 30;
        Memory.EffectiveWeight = 30.0;
        Memory.DecayPerGameDay = 1.5;
        Memory.SourceNpcId = "npc.ines";
        return LivingWorld::RegisterNpc(World, "npc.ines", "community.rexa") && LivingWorld::RegisterNpc(World, "npc.clerk", "community.rexa") &&
            LivingWorld::Connect(World, "npc.ines", "npc.clerk", NpcConnectionKind::Professional) &&
            LivingWorld::RecordMemory(World, "npc.ines", Memory) && LivingWorld::ResolveInteraction(World, "npc.ines", "talk.ines.bell") &&
            LivingWorld::SetAlive(World, "npc.clerk", false);
    }));

    const std::string Encoded = SaveCodec::Encode(Campaign.State());
    CampaignState Decoded;
    std::vector<std::string> Errors;
    CHECK(SaveCodec::Decode(Encoded, Decoded, Errors));
    CHECK(Decoded.Journal == Campaign.State().Journal);
    CHECK(Decoded.Progress == Campaign.State().Progress);
    CHECK(Decoded.LivingWorld == Campaign.State().LivingWorld);
    CHECK(Decoded == Campaign.State());
    CHECK(!LivingWorld::Find(Decoded.LivingWorld, "npc.clerk")->Alive);

    CampaignState Legacy = Campaign.State();
    Legacy.SaveVersion = 10;
    Errors.clear();
    CHECK(MigrateCampaignState(Legacy, Errors));
    CHECK_EQ(Legacy.SaveVersion, 11);
    CHECK(Legacy.Journal == QuestJournalState{});
    CHECK(Legacy.Progress == ProgressionState{});
    CHECK(Legacy.LivingWorld == LivingWorldState{});

    CampaignState Corrupt = Campaign.State();
    Corrupt.Progress.MarksSpent = 50;
    CHECK(!ValidateCampaignState(Corrupt, Errors));
}
