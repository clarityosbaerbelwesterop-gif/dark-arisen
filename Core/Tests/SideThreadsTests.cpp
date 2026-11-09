#include "TestHarness.h"

#include "DarkArisen/Core/SideThreads.h"

using namespace DarkArisen::Core;

TEST_CASE("Threads: the catalog's seventeen, the eleven with a stated activation registered as quests")
{
    CHECK(SideThreadRegister().size() == 17);
    int Stated = 0;
    for (const SideThread& Thread : SideThreadRegister()) Stated += Thread.ActivationStated ? 1 : 0;
    CHECK(Stated == 11);
    const QuestCatalog Catalog = SideThreadCatalog();
    CHECK(static_cast<int>(Catalog.Definitions().size()) == Stated);
    CHECK(Catalog.Find("thread.13.princess") == nullptr);
    CHECK(Catalog.Find("thread.03.big_toms_service") != nullptr && Catalog.Find("thread.03.big_toms_service")->SilentAtActivation);
}

TEST_CASE("Threads: Ines needs Chapter 5 and the third meeting; Mira's coast is overheard in Fjordlund")
{
    const SideThread* Ines = FindSideThread("thread.01.ines_esperanza");
    CHECK(Ines != nullptr);
    if (!Ines) return;
    ThreadContext Context;
    Context.Chapter = 5;
    Context.MeetingsWithGiver = 2;
    CHECK(!SideThreadReady(*Ines, QuestActivationTrigger::Conversation, Context));
    Context.MeetingsWithGiver = 3;
    CHECK(SideThreadReady(*Ines, QuestActivationTrigger::Conversation, Context));
    Context.Chapter = 4;
    CHECK(!SideThreadReady(*Ines, QuestActivationTrigger::Conversation, Context));

    const SideThread* Mira = FindSideThread("thread.02.miras_coast");
    ThreadContext Coast;
    Coast.Region = "Rexa";
    CHECK(!SideThreadReady(*Mira, QuestActivationTrigger::Overheard, Coast));
    Coast.Region = "QuietCoast";
    CHECK(SideThreadReady(*Mira, QuestActivationTrigger::Overheard, Coast));
    CHECK(!SideThreadReady(*Mira, QuestActivationTrigger::Conversation, Coast));
}

TEST_CASE("Threads: Salvio's parish waits until Jake has spared someone; unstated activations never fire")
{
    const SideThread* Salvio = FindSideThread("thread.04.father_salvios_parish");
    ThreadContext Context;
    CHECK(!SideThreadReady(*Salvio, QuestActivationTrigger::Conversation, Context));
    Context.ConditionMet = true;
    CHECK(SideThreadReady(*Salvio, QuestActivationTrigger::Conversation, Context));
    const SideThread* Ethan = FindSideThread("thread.17.ethan");
    CHECK(!SideThreadReady(*Ethan, QuestActivationTrigger::Conversation, Context));
}

TEST_CASE("Threads: a Thread activates through the journal, a silent one without an entry")
{
    const QuestCatalog Catalog = SideThreadCatalog();
    QuestJournalState State;
    QuestJournal::AdoptCatalog(State, Catalog);
    CHECK(QuestJournal::Activate(State, Catalog, "thread.03.big_toms_service", QuestActivationTrigger::Absence, 600, false));
    CHECK(!QuestJournal::HasEntryForQuest(State, "thread.03.big_toms_service"));
    CHECK(!QuestJournal::Activate(State, Catalog, "thread.13.princess", QuestActivationTrigger::Conversation, 600, false));
}
