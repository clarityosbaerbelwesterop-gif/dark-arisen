#include "DarkArisen/Core/SideThreads.h"

#include <array>

namespace DarkArisen::Core
{
    namespace
    {
        using T = QuestActivationTrigger;
        constexpr std::array<SideThread, 17> Threads = {{
            {"thread.01.ines_esperanza", "Ines and Esperanza", "conversation, Chapter 5+, third meeting", "npcs/named_crew_deep_dives.md", true, T::Conversation, 5, "", 3, "", false},
            {"thread.02.miras_coast", "Mira's Coast", "overheard, in Fjordlund", "npcs/named_crew_deep_dives.md", true, T::Overheard, 0, "QuietCoast", 0, "", false},
            {"thread.03.big_toms_service", "Big Tom's Service", "absence: someone recognises him and leaves", "npcs/named_crew_deep_dives.md", true, T::Absence, 0, "", 0, "", true},
            {"thread.04.father_salvios_parish", "Father Salvio's Parish", "conversation after Jake has spared someone", "npcs/named_crew_deep_dives.md", true, T::Conversation, 0, "", 0, "JakeHasSpared", false},
            {"thread.05.estebans_last_chart", "Esteban's Last Chart", "object: chart in his quarters", "npcs/named_crew_deep_dives.md", true, T::Object, 0, "", 0, "", false},
            {"thread.06.assassin_network", "The Assassin Network", "authored network activation", "colonial_war/assassin_network_questline.md", true, T::WorldState, 0, "", 0, "", false},
            {"thread.07.liberation_connections", "Liberation Connections", "five connections as one thread", "colonial_war/indigenous_liberation_alliance.md", true, T::WorldState, 0, "", 0, "", false},
            {"thread.08.kesslers_investigation", "Kessler's Investigation", "terminates at Castillo Dorado bribe ledgers", "npcs/imperial_contacts.md", false, T::Conversation, 0, "", 0, "", false},
            {"thread.09.ledger_trilogy", "The Ledger Trilogy", "four locations across a century of families", "regional dungeon catalog.md", false, T::Object, 0, "", 0, "", false},
            {"thread.10.schreibers_documentation", "Schreiber's Documentation", "recurring quest giver", "npcs/recurring_quest_givers.md", true, T::Conversation, 0, "", 0, "", false},
            {"thread.11.captain_vasquez", "Captain Vasquez", "recurring quest giver", "npcs/recurring_quest_givers.md", true, T::Conversation, 0, "", 0, "", false},
            {"thread.12.old_fortresses", "The Old Fortresses", "four reclamations, Alliance-gated", "colonial_war/castle_catalog.md", true, T::WorldState, 0, "", 0, "AllianceStanding", false},
            {"thread.13.princess", "The Princess", "fully specified Highmoore thread", "princess_quest_overview.md", false, T::Conversation, 0, "Highmoore", 0, "", false},
            {"thread.14.light_elves", "The Light Elves", "fully specified Highmoore thread", "wizard_questline.md", false, T::Conversation, 0, "Highmoore", 0, "", false},
            {"thread.15.reconstruction", "Reconstruction", "longest thread; no balance reward invented", "highmoore_reconstruction.md", false, T::WorldState, 0, "Highmoore", 0, "", false},
            {"thread.16.named_dead", "The Named Dead", "accumulation with no ending", "npcs/the_named_dead.md", true, T::WorldState, 0, "", 0, "", true},
            {"thread.17.ethan", "Ethan", "only Thread touching the main-story spine", "characters/ethan_harlow.md", false, T::Conversation, 0, "", 0, "", false},
        }};
    }

    std::span<const SideThread> SideThreadRegister() { return Threads; }

    const SideThread* FindSideThread(const std::string_view Id)
    {
        for (const SideThread& Thread : Threads)
            if (Thread.Id == Id) return &Thread;
        return nullptr;
    }

    QuestCatalog SideThreadCatalog()
    {
        QuestCatalog Catalog;
        for (const SideThread& Thread : Threads)
        {
            if (!Thread.ActivationStated) continue;
            // The catalog gives no words for Jake's first note, and the notebook is never written for him: a Thread
            // begins silently and its first entry comes with the first step its own document authors
            // (QuestJournal::RevealSilentQuest). Big Tom's and the Named Dead stay silent by design.
            QuestDefinition Definition;
            Definition.QuestId = std::string(Thread.Id);
            Definition.Trigger = Thread.Trigger;
            Definition.SilentAtActivation = true;
            Catalog.Register(Definition);
        }
        return Catalog;
    }

    bool SideThreadReady(const SideThread& Thread, const QuestActivationTrigger Trigger, const ThreadContext& Context)
    {
        if (!Thread.ActivationStated || Trigger != Thread.Trigger) return false;
        if (Context.Chapter < Thread.MinimumChapter) return false;
        if (!Thread.Region.empty() && Context.Region != Thread.Region) return false;
        if (Thread.Meeting > 0 && Context.MeetingsWithGiver < Thread.Meeting) return false;
        if (!Thread.Condition.empty() && !Context.ConditionMet) return false;
        return true;
    }
}
