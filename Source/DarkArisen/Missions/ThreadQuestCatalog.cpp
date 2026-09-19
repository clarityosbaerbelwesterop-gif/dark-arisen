#include "Missions/ThreadQuestCatalog.h"

namespace
{
FQuestActivationDefinition Thread(const TCHAR* Id,EQuestActivationTrigger Trigger)
{
    FQuestActivationDefinition D;
    D.QuestId=FName(Id);
    D.Trigger=Trigger;
    D.bSilentAtActivation=true;
    return D;
}
}

TArray<FQuestActivationDefinition> UThreadQuestCatalog::GetThreadDefinitions()
{
    return {
        Thread(TEXT("thread.01.ines_esperanza"),EQuestActivationTrigger::Conversation),
        Thread(TEXT("thread.02.miras_coast"),EQuestActivationTrigger::Overheard),
        Thread(TEXT("thread.03.big_toms_service"),EQuestActivationTrigger::Absence),
        Thread(TEXT("thread.04.father_salvios_parish"),EQuestActivationTrigger::Conversation),
        Thread(TEXT("thread.05.estebans_last_chart"),EQuestActivationTrigger::Object),
        Thread(TEXT("thread.06.assassin_network"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.07.liberation_connections"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.08.kesslers_investigation"),EQuestActivationTrigger::Conversation),
        Thread(TEXT("thread.09.ledger_trilogy"),EQuestActivationTrigger::Object),
        Thread(TEXT("thread.10.schreibers_documentation"),EQuestActivationTrigger::Conversation),
        Thread(TEXT("thread.11.captain_vasquez"),EQuestActivationTrigger::Conversation),
        Thread(TEXT("thread.12.old_fortresses"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.13.princess"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.14.light_elves"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.15.reconstruction"),EQuestActivationTrigger::WorldState),
        Thread(TEXT("thread.16.named_dead"),EQuestActivationTrigger::Object),
        Thread(TEXT("thread.17.ethan"),EQuestActivationTrigger::WorldState)
    };
}

bool UThreadQuestCatalog::RegisterThreads(UQuestJournalComponent* Journal)
{
    if(!Journal) return false;
    const TArray<FQuestActivationDefinition> Definitions=GetThreadDefinitions();
    if(Definitions.Num()!=17) return false;
    TSet<FName> Ids;
    for(const FQuestActivationDefinition& D:Definitions)
    {
        if(D.QuestId.IsNone() || Ids.Contains(D.QuestId) || !D.bSilentAtActivation) return false;
        Ids.Add(D.QuestId);
        FQuestRuntimeState Existing;
        if(Journal->TryGetQuestState(D.QuestId,Existing)) continue;
        if(!Journal->RegisterQuestDefinition(D)) return false;
    }
    return true;
}
