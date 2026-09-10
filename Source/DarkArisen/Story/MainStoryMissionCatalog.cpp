#include "Story/MainStoryMissionCatalog.h"

namespace
{
FMainStoryMissionDefinition Mission(const TCHAR* Id, int32 Chapter, const TCHAR* Previous = nullptr)
{
    FMainStoryMissionDefinition Result;
    Result.MissionId = FName(Id); Result.Chapter = Chapter;
    if (Previous) Result.Prerequisites.Add(FName(Previous));
    return Result;
}
}

const TArray<FMainStoryMissionDefinition>& FMainStoryMissionCatalog::Get()
{
    static const TArray<FMainStoryMissionDefinition> Missions = {
        Mission(TEXT("Main.C01.01.HomeWater"),1),
        Mission(TEXT("Main.C01.02.BlackSails"),1,TEXT("Main.C01.01.HomeWater")),
        Mission(TEXT("Main.C01.03.TheTaking"),1,TEXT("Main.C01.02.BlackSails")),
        Mission(TEXT("Main.C01.04.Undertow"),1,TEXT("Main.C01.03.TheTaking")),
        Mission(TEXT("Main.C02.01.ShatteredCoast"),2,TEXT("Main.C01.04.Undertow")),
        Mission(TEXT("Main.C02.02.AShipToTake"),2,TEXT("Main.C02.01.ShatteredCoast")),
        Mission(TEXT("Main.C02.03.FirstWake"),2,TEXT("Main.C02.02.AShipToTake")),
        Mission(TEXT("Main.C03.01.RexaHarbor"),3,TEXT("Main.C02.03.FirstWake")),
        Mission(TEXT("Main.C03.02.SafeRoutes"),3,TEXT("Main.C03.01.RexaHarbor")),
        Mission(TEXT("Main.C03.03.TheFirstHolder"),3,TEXT("Main.C03.02.SafeRoutes")),
        Mission(TEXT("Main.C04.01.SaltAndIron"),4,TEXT("Main.C03.03.TheFirstHolder")),
        Mission(TEXT("Main.C04.02.BrokenCompact"),4,TEXT("Main.C04.01.SaltAndIron")),
        Mission(TEXT("Main.C04.03.HerrerasFall"),4,TEXT("Main.C04.02.BrokenCompact")),
        Mission(TEXT("Main.C05.01.WarCurrent"),5,TEXT("Main.C04.03.HerrerasFall")),
        Mission(TEXT("Main.C05.02.HoldersWake"),5,TEXT("Main.C05.01.WarCurrent")),
        Mission(TEXT("Main.C05.03.NoSafeHarbor"),5,TEXT("Main.C05.02.HoldersWake")),
        Mission(TEXT("Main.C06.01.HighmooreRoad"),6,TEXT("Main.C05.03.NoSafeHarbor")),
        Mission(TEXT("Main.C06.02.CrystalPassage"),6,TEXT("Main.C06.01.HighmooreRoad")),
        Mission(TEXT("Main.C06.03.TheNorthernOath"),6,TEXT("Main.C06.02.CrystalPassage")),
        Mission(TEXT("Main.C07.01.FalseBearings"),7,TEXT("Main.C06.03.TheNorthernOath")),
        Mission(TEXT("Main.C07.02.EthansMarks"),7,TEXT("Main.C07.01.FalseBearings")),
        Mission(TEXT("Main.C07.03.ThroughTheNet"),7,TEXT("Main.C07.02.EthansMarks")),
        Mission(TEXT("Main.C08.01.ThePrisonCourse"),8,TEXT("Main.C07.03.ThroughTheNet")),
        Mission(TEXT("Main.C08.02.BrothersAlive"),8,TEXT("Main.C08.01.ThePrisonCourse")),
        Mission(TEXT("Main.C08.03.HomewardBearing"),8,TEXT("Main.C08.02.BrothersAlive")),
        Mission(TEXT("Main.C09.01.EthansGrove"),9,TEXT("Main.C08.03.HomewardBearing")),
        Mission(TEXT("Main.C09.02.MemoryOfABrother"),9,TEXT("Main.C09.01.EthansGrove")),
        Mission(TEXT("Main.C09.03.DreamFight"),9,TEXT("Main.C09.02.MemoryOfABrother")),
        Mission(TEXT("Main.C09.04.WakingCourse"),9,TEXT("Main.C09.03.DreamFight")),
        Mission(TEXT("Main.C10.01.Armada"),10,TEXT("Main.C09.04.WakingCourse")),
        Mission(TEXT("Main.C10.02.BreakTheChain"),10,TEXT("Main.C10.01.Armada")),
        Mission(TEXT("Main.C10.03.BlackDeck"),10,TEXT("Main.C10.02.BreakTheChain")),
        Mission(TEXT("Main.C10.04.DravenVoss"),10,TEXT("Main.C10.03.BlackDeck")),
        Mission(TEXT("Main.C10.05.TheWakeAfter"),10,TEXT("Main.C10.04.DravenVoss"))};
    return Missions;
}

int32 FMainStoryMissionCatalog::FindIndex(FName Id) { return Get().IndexOfByPredicate([Id](const auto& M){return M.MissionId==Id;}); }
bool FMainStoryMissionCatalog::Validate(TArray<FString>& Errors)
{
    Errors.Reset(); const auto& M=Get(); const int32 Expected[]={4,3,3,3,3,3,3,3,4,5};
    if(M.Num()!=MissionCount) Errors.Add(TEXT("Main story must contain exactly 34 missions."));
    TSet<FName> Seen;
    for(int32 I=0;I<M.Num();++I){const auto& E=M[I]; if(E.MissionId.IsNone()||Seen.Contains(E.MissionId)) Errors.Add(TEXT("Mission IDs must be unique and non-empty.")); Seen.Add(E.MissionId); if(E.Chapter<1||E.Chapter>10) Errors.Add(TEXT("Invalid chapter.")); for(FName P:E.Prerequisites) if(!Seen.Contains(P)) Errors.Add(FString::Printf(TEXT("%s has an orphan/forward prerequisite."),*E.MissionId.ToString())); const FString Id=E.MissionId.ToString().ToLower(); if(Id.Contains(TEXT("ethanboss"))||Id.Contains(TEXT("ethan.boss"))||Id.Contains(TEXT("ethanbetrayal"))) Errors.Add(TEXT("Legacy Ethan boss path is forbidden."));}
    for(int32 C=1;C<=10;++C) if(M.FilterByPredicate([C](const auto& E){return E.Chapter==C;}).Num()!=Expected[C-1]) Errors.Add(TEXT("Chapter distribution mismatch."));
    return Errors.IsEmpty();
}
