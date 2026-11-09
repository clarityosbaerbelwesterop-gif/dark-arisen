#include "TestHarness.h"

#include "DarkArisen/Core/Dialogue.h"

using namespace DarkArisen::Core;

namespace
{
    using Req = DialogueRequirement;
    using Eff = DialogueEffect;

    /** A test conversation in the shape of dialogue_system.md 6.1's shopkeeper (fixture data, not game content). */
    Conversation Shopkeeper()
    {
        Conversation Talk;
        Talk.Id = "Test.Shopkeeper";
        Talk.NpcId = "npc.test.shopkeeper";
        Talk.Greetings = {
            {"Friend", {{Req::Kind::ReputationAtLeast, "", 30}}},
            {"Stranger", {}},
        };
        DialogueNode Stranger;
        Stranger.Id = "Stranger";
        Stranger.Lines = {{"npc.test.shopkeeper", "Welcome to my shop, Captain."}};
        DialogueOption Help;
        Help.Id = "OfferHelp";
        Help.Tone = DialogueTone::Polite;
        Help.Statement = "\"I could deal with those bandits.\"";
        Help.Effects = {{Eff::Kind::Promise, "Bandits"}, {Eff::Kind::SetWorldFact, "World.Test.BanditsPromised"}};
        Help.Next = "Thanks";
        DialogueOption Daughter;
        Daughter.Id = "AskDaughter";
        Daughter.Tone = DialogueTone::Direct;
        Daughter.Statement = "\"Where is your daughter?\"";
        Daughter.Requires = {{Req::Kind::Fact, "World.Test.DaughterMissing"}};
        Daughter.Next = "Refuses";
        DialogueOption Threaten;
        Threaten.Id = "Threaten";
        Threaten.Tone = DialogueTone::Aggressive;
        Threaten.Statement = "\"You'd better tell me.\"";
        Threaten.Effects = {{Eff::Kind::Remember, "Test.Threatened", NpcMemoryEmotion::Negative, 80}, {Eff::Kind::SetMood, "", NpcMemoryEmotion::Neutral, 1, NpcMood::Angry}};
        Threaten.Next = "";
        Stranger.Options = {Help, Daughter, Threaten};
        DialogueNode Thanks;
        Thanks.Id = "Thanks";
        Thanks.Lines = {{"npc.test.shopkeeper", "I'll hold you to that."}};
        DialogueOption Bye;
        Bye.Id = "Bye";
        Bye.Statement = "\"Good day.\"";
        Thanks.Options = {Bye};
        DialogueNode Refuses;
        Refuses.Id = "Refuses";
        Refuses.Lines = {{"npc.test.shopkeeper", "That's all I have to say."}};
        Refuses.NpcEnds = true;
        DialogueNode Friend;
        Friend.Id = "Friend";
        Friend.Lines = {{"npc.test.shopkeeper", "Captain Harlow! Good to see you."}};
        DialogueOption Farewell;
        Farewell.Id = "Farewell";
        Farewell.Statement = "\"Goodbye, old friend.\"";
        Farewell.Final = true;
        Friend.Options = {Farewell};
        Talk.Nodes = {Stranger, Thanks, Refuses, Friend};
        return Talk;
    }

    struct Fixture
    {
        CampaignState Campaign;
        LivingWorldState World;
        Fixture() { LivingWorld::RegisterNpc(World, "npc.test.shopkeeper", "community.test"); }
    };
}

TEST_CASE("Dialogue: a valid conversation passes the authoring checks; a story fact in a choice fails them")
{
    std::vector<std::string> Errors;
    CHECK(ValidateConversation(Shopkeeper(), Errors));
    CHECK(Errors.empty());
    Conversation Bad = Shopkeeper();
    Bad.Nodes[0].Options[0].Effects.push_back({Eff::Kind::SetWorldFact, "Story.EthanRecovered"});
    Bad.Nodes[0].Options[0].Next = "Nowhere";
    CHECK(!ValidateConversation(Bad, Errors));
    CHECK(Errors.size() == 2);
}

TEST_CASE("Dialogue: the greeting follows the relationship (2.1, 6.5)")
{
    Fixture F;
    const Conversation Talk = Shopkeeper();
    ConversationRunner First(Talk, F.Campaign, F.World, 0);
    CHECK(First.Begin());
    CHECK(First.GetNode()->Id == "Stranger");
    F.World.Records["npc.test.shopkeeper"].PersonalReputation = 40;
    ConversationRunner Later(Talk, F.Campaign, F.World, 100);
    CHECK(Later.Begin());
    CHECK(Later.GetNode()->Id == "Friend");
}

TEST_CASE("Dialogue: hidden branches appear only when Jake knows enough (2.3)")
{
    Fixture F;
    const Conversation Talk = Shopkeeper();
    ConversationRunner Runner(Talk, F.Campaign, F.World, 0);
    CHECK(Runner.Begin());
    CHECK(Runner.AvailableOptions().size() == 2);
    CHECK(!Runner.Choose("AskDaughter"));
    F.Campaign.WorldFacts.insert("World.Test.DaughterMissing");
    CHECK(Runner.AvailableOptions().size() == 3);
    CHECK(Runner.Choose("AskDaughter"));
    // the NPC refuses to go on: the conversation ends and the door closes
    CHECK(!Runner.IsActive());
    CHECK(Runner.GetRecord().End == ConversationEnd::NpcRefused);
    CHECK(!Runner.GetRecord().DoorOpen);
}

TEST_CASE("Dialogue: a promise is remembered for good and sets its world fact (6.4)")
{
    Fixture F;
    const Conversation Talk = Shopkeeper();
    ConversationRunner Runner(Talk, F.Campaign, F.World, 10);
    CHECK(Runner.Begin());
    CHECK(Runner.Choose("OfferHelp"));
    CHECK(Runner.GetNode()->Id == "Thanks");
    CHECK(Runner.Choose("Bye"));
    CHECK(Runner.GetRecord().End == ConversationEnd::Completed);
    CHECK(F.Campaign.HasFact("World.Test.BanditsPromised"));
    const NpcRecord* Npc = LivingWorld::Find(F.World, "npc.test.shopkeeper");
    bool Remembered = false;
    for (const NpcMemory& Memory : Npc->Memories) Remembered = Remembered || (Memory.EventId == "Promise.Bandits" && Memory.Permanent);
    CHECK(Remembered);
    CHECK(Runner.GetRecord().ChoicesMade.size() == 2);
}

TEST_CASE("Dialogue: a threat sours the NPC and the goodbye turns cold (2.4, 7.5)")
{
    Fixture F;
    const Conversation Talk = Shopkeeper();
    ConversationRunner Runner(Talk, F.Campaign, F.World, 0);
    CHECK(Runner.Begin());
    CHECK(Runner.Choose("Threaten"));
    const NpcRecord* Npc = LivingWorld::Find(F.World, "npc.test.shopkeeper");
    CHECK(Npc->Mood == NpcMood::Angry);
    CHECK(Npc->PersonalReputation < -20);
    CHECK(Runner.GetRecord().Farewell == Goodbye::Cold);
}

TEST_CASE("Dialogue: the player may leave, combat interrupts, and a final goodbye closes the door (7.3-7.5)")
{
    Fixture F;
    const Conversation Talk = Shopkeeper();
    ConversationRunner Leaving(Talk, F.Campaign, F.World, 0);
    CHECK(Leaving.Begin());
    Leaving.Leave();
    CHECK(Leaving.GetRecord().End == ConversationEnd::PlayerLeft && Leaving.GetRecord().DoorOpen);
    ConversationRunner Fight(Talk, F.Campaign, F.World, 0);
    CHECK(Fight.Begin());
    Fight.Interrupt();
    CHECK(Fight.GetRecord().End == ConversationEnd::Interrupted && Fight.GetRecord().DoorOpen);
    F.World.Records["npc.test.shopkeeper"].PersonalReputation = 80;
    ConversationRunner Last(Talk, F.Campaign, F.World, 0);
    CHECK(Last.Begin());
    CHECK(Last.Choose("Farewell"));
    CHECK(Last.GetRecord().Farewell == Goodbye::Final && !Last.GetRecord().DoorOpen);
    CHECK(GoodbyeFor(80) == Goodbye::Loyal);
    CHECK(GoodbyeFor(40) == Goodbye::Warm);
    CHECK(GoodbyeFor(0) == Goodbye::Standard);
}

TEST_CASE("Dialogue: the dead do not talk")
{
    Fixture F;
    LivingWorld::SetAlive(F.World, "npc.test.shopkeeper", false);
    const Conversation Talk = Shopkeeper();
    ConversationRunner Runner(Talk, F.Campaign, F.World, 0);
    CHECK(!Runner.Begin());
}
