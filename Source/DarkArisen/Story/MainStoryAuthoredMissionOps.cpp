#include "Story/MainStorySubsystem.h"

bool UMainStorySubsystem::SetQuestOutcome(const FName OutcomeKey,const FName OutcomeValue)
{
    if(!State||OutcomeKey.IsNone()||OutcomeValue.IsNone())return false;
    if(const FName* Existing=State->QuestOutcomes.Find(OutcomeKey))return *Existing==OutcomeValue;
    State->QuestOutcomes.Add(OutcomeKey,OutcomeValue);
    return true;
}

bool UMainStorySubsystem::HasBossDefeated(const FName BossId) const
{
    return State && !BossId.IsNone() && State->DefeatedBosses.Contains(BossId);
}

bool UMainStorySubsystem::CompleteAuthoredMission(const FName Id)
{
    if(!State||GetMissionState(Id)!=EMainMissionState::Active)return false;
    if(!CompleteMission(Id))return false;

    const auto Story=[this](const TCHAR* Fact){State->StoryFacts.Add(FName(Fact));};
    const auto World=[this](const TCHAR* Fact){State->WorldFacts.Add(FName(Fact));};

    if(Id==TEXT("Main.C03.01.RexaHarbor"))Story(TEXT("Story.EthanAliveConfirmed"));
    else if(Id==TEXT("Main.C03.02.SafeRoutes"))Story(TEXT("Story.EthanRouteMarksFound"));
    else if(Id==TEXT("Main.C03.03.TheFirstHolder")){Story(TEXT("Story.FirstHolderCrossed"));World(TEXT("Chapter.03.Complete"));}
    else if(Id==TEXT("Main.C04.01.SaltAndIron"))World(TEXT("War.VisibleToJake"));
    else if(Id==TEXT("Main.C04.02.BrokenCompact"))World(TEXT("War.Playable"));
    else if(Id==TEXT("Main.C04.03.HerrerasFall")){Story(TEXT("Story.HarlowChartRecovered"));World(TEXT("Chapter.04.Complete"));}
    else if(Id==TEXT("Main.C05.01.WarCurrent"))Story(TEXT("Story.NavigatorAliasKnown"));
    else if(Id==TEXT("Main.C05.02.HoldersWake"))World(TEXT("Assassins.Unlocked"));
    else if(Id==TEXT("Main.C05.03.NoSafeHarbor")){Story(TEXT("Story.DeepRouteKnown"));World(TEXT("Chapter.05.Complete"));}
    else if(Id==TEXT("Main.C06.01.HighmooreRoad"))World(TEXT("Holding.FirstOwned"));
    else if(Id==TEXT("Main.C06.02.CrystalPassage"))World(TEXT("Holding.IntelligenceReady"));
    else if(Id==TEXT("Main.C06.03.TheNorthernOath")){Story(TEXT("Story.EthanResisting"));World(TEXT("Chapter.06.Complete"));}
    else if(Id==TEXT("Main.C07.01.FalseBearings"))Story(TEXT("Story.RetaliationPaid"));
    else if(Id==TEXT("Main.C07.02.EthansMarks"))Story(TEXT("Story.EthanUsedAsNavigator"));
    else if(Id==TEXT("Main.C07.03.ThroughTheNet")){Story(TEXT("Story.RescueBeforeRevenge"));World(TEXT("Chapter.07.Complete"));}
    else if(Id==TEXT("Main.C08.01.ThePrisonCourse"))World(TEXT("Armada.Revealed"));
    else if(Id==TEXT("Main.C08.02.BrothersAlive"))
    {
        State->StoryFacts.Remove(TEXT("Story.EthanRecovered"));
        Story(TEXT("Story.EthanSignalUnderstood"));
    }
    else if(Id==TEXT("Main.C08.03.HomewardBearing"))
    {
        Story(TEXT("Story.EthanRecovered"));Story(TEXT("Story.EthanNoLongerCaptive"));World(TEXT("Chapter.08.Complete"));
    }
    else if(Id==TEXT("Main.C09.01.EthansGrove"))Story(TEXT("Story.DravenMotiveKnown"));
    else if(Id==TEXT("Main.C09.02.MemoryOfABrother"))Story(TEXT("Story.GroveVisited"));
    else if(Id==TEXT("Main.C09.03.DreamFight"))Story(TEXT("Story.DreamResolved"));
    else if(Id==TEXT("Main.C09.04.WakingCourse")){World(TEXT("Combat.RacheUnlocked"));Story(TEXT("Story.FinalRouteKnown"));World(TEXT("Chapter.09.Complete"));}
    else if(Id==TEXT("Main.C10.01.Armada"))World(TEXT("Finale.EntryStateLocked"));
    else if(Id==TEXT("Main.C10.02.BreakTheChain"))World(TEXT("Armada.LogisticsBroken"));
    else if(Id==TEXT("Main.C10.03.BlackDeck"))World(TEXT("Finale.DravenReached"));
    else if(Id==TEXT("Main.C10.04.DravenVoss"))Story(TEXT("Story.DravenDefeated"));
    else if(Id==TEXT("Main.C10.05.TheWakeAfter"))World(TEXT("Chapter.10.Complete"));

    const bool bChapterBoundary=
        Id==TEXT("Main.C03.03.TheFirstHolder")||Id==TEXT("Main.C04.03.HerrerasFall")||
        Id==TEXT("Main.C05.03.NoSafeHarbor")||Id==TEXT("Main.C06.03.TheNorthernOath")||
        Id==TEXT("Main.C07.03.ThroughTheNet")||Id==TEXT("Main.C08.03.HomewardBearing")||
        Id==TEXT("Main.C09.04.WakingCourse")||Id==TEXT("Main.C10.05.TheWakeAfter");
    if(bChapterBoundary) Save(TEXT("DarkArisenAlpha"),0);
    return true;
}
