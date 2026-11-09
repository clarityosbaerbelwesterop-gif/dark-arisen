#pragma once

#include "DarkArisen/Core/QuestJournal.h"

#include <span>
#include <string_view>

/**
 * The seventeen side Threads of ContentSource/SideQuests/ThreadCatalog.json ("side quest catalog.md" 2), wired to the
 * markerless quest journal by their activation (`quest activation.md`'s six invisible modes). Policy of the catalog:
 * finite authored Threads only; missing step-level canon is never invented. A Thread whose activation the catalog does
 * not state (it points at its own document instead) is registered but cannot activate until that document is wired.
 */
namespace DarkArisen::Core
{
    struct SideThread
    {
        std::string_view Id;
        std::string_view Name;
        std::string_view Activation;   // the catalog's words
        std::string_view Source;
        bool ActivationStated = true;
        QuestActivationTrigger Trigger = QuestActivationTrigger::Conversation;
        int MinimumChapter = 0;
        std::string_view Region;       // empty: anywhere
        int Meeting = 0;               // the giver's nth meeting (0: any)
        std::string_view Condition;    // a named world condition the engine checks (empty: none)
        bool Silent = false;           // stays without a journal entry even once its steps are authored
    };

    std::span<const SideThread> SideThreadRegister();
    const SideThread* FindSideThread(std::string_view Id);
    /** One quest definition per Thread whose activation the catalog states; each begins silently (no invented note). */
    QuestCatalog SideThreadCatalog();

    struct ThreadContext
    {
        int Chapter = 1;
        std::string_view Region;
        int MeetingsWithGiver = 0;
        bool ConditionMet = false;
    };
    /** Whether the Thread may activate now through its own trigger. */
    bool SideThreadReady(const SideThread& Thread, QuestActivationTrigger Trigger, const ThreadContext& Context);
}
