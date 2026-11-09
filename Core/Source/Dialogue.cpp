#include "DarkArisen/Core/Dialogue.h"

#include "DarkArisen/Core/Facts.h"

#include <set>

namespace DarkArisen::Core
{
    namespace
    {
        const DialogueNode* FindNode(const Conversation& Talk, const std::string_view Id)
        {
            for (const DialogueNode& Node : Talk.Nodes)
                if (Node.Id == Id) return &Node;
            return nullptr;
        }

        NpcMemory MemoryOf(const std::string& EventId, const NpcMemoryEmotion Emotion, const int Weight, const std::int64_t Minute, const bool Permanent)
        {
            NpcMemory Memory;
            Memory.EventId = EventId;
            Memory.Kind = NpcMemoryKind::DirectEncounter;
            Memory.Emotion = Emotion;
            Memory.OriginalWeight = Weight;
            Memory.EffectiveWeight = static_cast<double>(Weight);
            Memory.OccurredAtGameMinute = Minute;
            Memory.Permanent = Permanent;
            return Memory;
        }
    }

    bool ValidateConversation(const Conversation& Talk, std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        if (Talk.Id.empty() || Talk.NpcId.empty()) OutErrors.push_back("conversation needs an id and an NPC");
        if (Talk.Greetings.empty()) OutErrors.push_back(Talk.Id + ": no greeting");
        std::set<std::string> Ids;
        for (const DialogueNode& Node : Talk.Nodes)
            if (Node.Id.empty() || !Ids.insert(Node.Id).second) OutErrors.push_back(Talk.Id + ": empty or duplicate node id '" + Node.Id + "'");
        for (const DialogueGreeting& Greeting : Talk.Greetings)
            if (!FindNode(Talk, Greeting.Node)) OutErrors.push_back(Talk.Id + ": greeting opens missing node '" + Greeting.Node + "'");
        if (!Talk.Greetings.empty() && !Talk.Greetings.back().Requires.empty())
            OutErrors.push_back(Talk.Id + ": the last greeting must have no requirements, so a conversation can always open");
        for (const DialogueNode& Node : Talk.Nodes)
        {
            std::set<std::string> OptionIds;
            for (const DialogueOption& Option : Node.Options)
            {
                if (Option.Id.empty() || !OptionIds.insert(Option.Id).second) OutErrors.push_back(Talk.Id + "/" + Node.Id + ": empty or duplicate option id");
                if (Option.Statement.empty()) OutErrors.push_back(Talk.Id + "/" + Option.Id + ": a choice shows what Jake will say");
                if (!Option.Next.empty() && !FindNode(Talk, Option.Next)) OutErrors.push_back(Talk.Id + "/" + Option.Id + ": leads to missing node '" + Option.Next + "'");
                for (const DialogueEffect& Effect : Option.Effects)
                    if (Effect.Type == DialogueEffect::Kind::SetWorldFact && (Effect.Key.empty() || Facts::IsStoryFact(Effect.Key)))
                        OutErrors.push_back(Talk.Id + "/" + Option.Id + ": a conversation cannot set the story fact '" + Effect.Key + "'");
            }
        }
        return OutErrors.size() == Before;
    }

    Goodbye GoodbyeFor(const int PersonalReputation)
    {
        if (PersonalReputation < -20) return Goodbye::Cold;
        if (PersonalReputation >= 70) return Goodbye::Loyal;
        if (PersonalReputation >= 30) return Goodbye::Warm;
        return Goodbye::Standard;
    }

    ConversationRunner::ConversationRunner(const Conversation& InTalk, CampaignState& InCampaign, LivingWorldState& InWorld, const std::int64_t InGameMinute)
        : Talk(InTalk), Campaign(InCampaign), World(InWorld), GameMinute(InGameMinute)
    {
    }

    bool ConversationRunner::Holds(const DialogueRequirement& Requirement) const
    {
        const NpcRecord* Npc = LivingWorld::Find(World, Talk.NpcId);
        const int Reputation = Npc ? Npc->PersonalReputation : 0;
        switch (Requirement.Type)
        {
        case DialogueRequirement::Kind::Fact: return Campaign.HasFact(Requirement.Key);
        case DialogueRequirement::Kind::NoFact: return !Campaign.HasFact(Requirement.Key);
        case DialogueRequirement::Kind::ReputationAtLeast: return Reputation >= Requirement.Value;
        case DialogueRequirement::Kind::ReputationBelow: return Reputation < Requirement.Value;
        case DialogueRequirement::Kind::TrustAtLeast: return Npc && Npc->SpecificTrust >= Requirement.Value;
        case DialogueRequirement::Kind::Mood: return Npc && static_cast<int>(Npc->Mood) == Requirement.Value;
        case DialogueRequirement::Kind::Remembers:
            if (!Npc) return false;
            for (const NpcMemory& Memory : Npc->Memories)
                if (Memory.EventId == Requirement.Key) return true;
            return false;
        }
        return false;
    }

    bool ConversationRunner::HoldsAll(const std::vector<DialogueRequirement>& Requirements) const
    {
        for (const DialogueRequirement& Requirement : Requirements)
            if (!Holds(Requirement)) return false;
        return true;
    }

    bool ConversationRunner::Begin()
    {
        const NpcRecord* Npc = LivingWorld::Find(World, Talk.NpcId);
        if (!Npc || !Npc->Alive || !Npc->Available || Node) return false;
        for (const DialogueGreeting& Greeting : Talk.Greetings)
            if (HoldsAll(Greeting.Requires))
            {
                Record = {};
                Enter(Greeting.Node);
                return Node != nullptr;
            }
        return false;
    }

    std::vector<const DialogueOption*> ConversationRunner::AvailableOptions() const
    {
        std::vector<const DialogueOption*> Out;
        if (!Node) return Out;
        for (const DialogueOption& Option : Node->Options)
            if (HoldsAll(Option.Requires)) Out.push_back(&Option);
        return Out;
    }

    bool ConversationRunner::Choose(const std::string_view OptionId)
    {
        for (const DialogueOption* Option : AvailableOptions())
        {
            if (Option->Id != OptionId) continue;
            Record.ChoicesMade.push_back(Option->Id);
            for (const DialogueEffect& Effect : Option->Effects) Apply(Effect);
            if (Option->Next.empty()) Finish(ConversationEnd::Completed, Option->Final);
            else Enter(Option->Next);
            return true;
        }
        return false;
    }

    void ConversationRunner::Leave()
    {
        if (Node) Finish(ConversationEnd::PlayerLeft, false);
    }

    void ConversationRunner::Interrupt()
    {
        if (Node) Finish(ConversationEnd::Interrupted, false);
    }

    void ConversationRunner::Apply(const DialogueEffect& Effect)
    {
        switch (Effect.Type)
        {
        case DialogueEffect::Kind::SetWorldFact:
            if (!Effect.Key.empty() && !Facts::IsStoryFact(Effect.Key)) Campaign.WorldFacts.insert(Effect.Key);
            break;
        case DialogueEffect::Kind::Remember:
            LivingWorld::RecordMemory(World, Talk.NpcId, MemoryOf(Effect.Key, Effect.Emotion, Effect.Weight, GameMinute, false));
            break;
        case DialogueEffect::Kind::SetMood:
            LivingWorld::SetMood(World, Talk.NpcId, Effect.Mood);
            break;
        case DialogueEffect::Kind::Promise:
            // 6.4: the NPC remembers the promise for good; keeping or breaking it is a later memory of its own
            LivingWorld::RecordMemory(World, Talk.NpcId, MemoryOf("Promise." + Effect.Key, NpcMemoryEmotion::Positive, 2, GameMinute, true));
            break;
        case DialogueEffect::Kind::Lie:
            // 6.3: a lie is a memory the NPC does not yet know to be one; the truth arrives as a negative memory later
            LivingWorld::RecordMemory(World, Talk.NpcId, MemoryOf("Lie." + Effect.Key, NpcMemoryEmotion::Neutral, 1, GameMinute, true));
            break;
        case DialogueEffect::Kind::ResolveInteraction:
            LivingWorld::ResolveInteraction(World, Talk.NpcId, Effect.Key);
            break;
        }
    }

    void ConversationRunner::Enter(const std::string_view NodeId)
    {
        Node = FindNode(Talk, NodeId);
        if (!Node) return;
        Record.NodesVisited.push_back(Node->Id);
        if (Node->NpcEnds) Finish(ConversationEnd::NpcRefused, false);
    }

    void ConversationRunner::Finish(const ConversationEnd End, const bool Final)
    {
        Node = nullptr;
        Record.End = End;
        const NpcRecord* Npc = LivingWorld::Find(World, Talk.NpcId);
        Record.Farewell = Final ? Goodbye::Final : GoodbyeFor(Npc ? Npc->PersonalReputation : 0);
        Record.DoorOpen = !Final && End != ConversationEnd::NpcRefused;
    }
}
