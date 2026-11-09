#pragma once

#include "DarkArisen/Core/CampaignState.h"
#include "DarkArisen/Core/LivingWorld.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/**
 * Conversations after docs/design/npcs/dialogue_system.md: a greeting chosen by relationship, mood and context (2.1),
 * topics and choices shown as full statements with a tone (2.2), branches hidden until reputation, knowledge, items or
 * time allow them (2.3), consequences that change mood, reputation and memory (2.4, 6), a conversation state with its
 * end reason and an open or closed door (2.5), player and NPC control (7.3), interruption (7.4) and goodbyes that follow
 * the relationship (7.5). Promises and lies are remembered as the NPC's memories (6.3, 6.4) and travel through the
 * living world like any other memory.
 *
 * Conversations are authored data (never generated). Their choices may change world facts and the NPC; story facts
 * belong to the campaign runtime, so a conversation can read them but never set them.
 */
namespace DarkArisen::Core
{
    /** 2.2's tone indicator. */
    enum class DialogueTone : std::uint8_t { Calm, Polite, Direct, Curious, Sympathetic, Aggressive, Hostile, Silent };

    /** 7.5. Final is authored (a conversation that closes for good); the others follow the relationship. */
    enum class Goodbye : std::uint8_t { Cold, Standard, Warm, Loyal, Final };

    enum class ConversationEnd : std::uint8_t { None, Completed, PlayerLeft, NpcRefused, Interrupted };

    struct DialogueRequirement
    {
        enum class Kind : std::uint8_t { Fact, NoFact, ReputationAtLeast, ReputationBelow, TrustAtLeast, Remembers, Mood };
        Kind Type = Kind::Fact;
        std::string Key;   // fact id or memory event id
        int Value = 0;     // threshold, or the NpcMood ordinal for Mood
    };

    struct DialogueEffect
    {
        enum class Kind : std::uint8_t { SetWorldFact, Remember, SetMood, Promise, Lie, ResolveInteraction };
        Kind Type = Kind::SetWorldFact;
        std::string Key;
        NpcMemoryEmotion Emotion = NpcMemoryEmotion::Neutral;  // Remember
        int Weight = 1;                                         // Remember
        NpcMood Mood = NpcMood::Content;                        // SetMood
    };

    struct DialogueLine
    {
        std::string Speaker;  // "Jake" or the NPC id
        std::string Text;
    };

    struct DialogueOption
    {
        std::string Id;
        DialogueTone Tone = DialogueTone::Calm;
        std::string Statement;   // what Jake will say, paraphrased (2.2)
        std::string Hint;        // an optional consequence hint
        std::vector<DialogueRequirement> Requires;  // unmet: the option is hidden, not greyed (2.3)
        std::vector<DialogueEffect> Effects;
        std::string Next;        // node id; empty ends the conversation
        bool Final = false;      // ending here closes the door for good (Goodbye::Final)
    };

    struct DialogueNode
    {
        std::string Id;
        std::vector<DialogueLine> Lines;
        std::vector<DialogueOption> Options;
        bool NpcEnds = false;    // the NPC refuses to go on (7.3)
    };

    struct DialogueGreeting
    {
        std::string Node;
        std::vector<DialogueRequirement> Requires;
    };

    struct Conversation
    {
        std::string Id;
        std::string NpcId;
        /** In priority order; the first whose requirements hold opens the conversation (2.1). */
        std::vector<DialogueGreeting> Greetings;
        std::vector<DialogueNode> Nodes;
    };

    /** Authoring checks: ids, links, reachable greeting, no story fact set by a choice. */
    bool ValidateConversation(const Conversation& Talk, std::vector<std::string>& OutErrors);

    /** 7.5: Cold below -20, Warm from 30, Loyal from 70 (reputation runs -100..100; thresholds are tuning values). */
    Goodbye GoodbyeFor(int PersonalReputation);

    struct ConversationRecord
    {
        std::vector<std::string> ChoicesMade;  // option ids in order (2.5)
        std::vector<std::string> NodesVisited;
        ConversationEnd End = ConversationEnd::None;
        Goodbye Farewell = Goodbye::Standard;
        bool DoorOpen = true;
    };

    class ConversationRunner
    {
    public:
        ConversationRunner(const Conversation& Talk, CampaignState& Campaign, LivingWorldState& World, std::int64_t GameMinute);

        /** Picks the greeting; false when none applies or the NPC is dead or unavailable. */
        bool Begin();
        const DialogueNode* GetNode() const { return Node; }
        /** The options whose requirements hold; hidden branches are simply absent. */
        std::vector<const DialogueOption*> AvailableOptions() const;
        bool Choose(std::string_view OptionId);
        /** 7.3: the player can end the conversation at any time. */
        void Leave();
        /** 7.4: combat or another interruption ends the conversation; the door stays open. */
        void Interrupt();

        bool IsActive() const { return Node != nullptr; }
        const ConversationRecord& GetRecord() const { return Record; }

    private:
        bool Holds(const DialogueRequirement& Requirement) const;
        bool HoldsAll(const std::vector<DialogueRequirement>& Requirements) const;
        void Apply(const DialogueEffect& Effect);
        void Enter(std::string_view NodeId);
        void Finish(ConversationEnd End, bool Final);

        const Conversation& Talk;
        CampaignState& Campaign;
        LivingWorldState& World;
        std::int64_t GameMinute = 0;
        const DialogueNode* Node = nullptr;
        ConversationRecord Record;
    };
}
