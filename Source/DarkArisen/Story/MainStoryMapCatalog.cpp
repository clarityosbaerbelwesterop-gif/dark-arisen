#include "Story/MainStoryMapCatalog.h"
FName FMainStoryMapCatalog::ResolveMap(const FName Id)
{
    static const TMap<FName,FName> Maps={
        {TEXT("Main.C01.01.HomeWater"),TEXT("L_HarlowOpening")},
        {TEXT("Main.C01.02.BlackSails"),TEXT("L_HarlowOpening")},
        {TEXT("Main.C01.03.TheTaking"),TEXT("L_HarlowOpening")},
        {TEXT("Main.C01.04.Undertow"),TEXT("L_DriftwoodBeach")},
        {TEXT("Main.C02.01.ShatteredCoast"),TEXT("L_DriftwoodBeach")},
        {TEXT("Main.C02.02.AShipToTake"),TEXT("L_GalleonCove")},
        {TEXT("Main.C02.03.FirstWake"),TEXT("L_OpenSea_FirstWake")},
        {TEXT("Main.C03.01.RexaHarbor"),TEXT("L_RexaHarbor")},
        {TEXT("Main.C03.02.SafeRoutes"),TEXT("L_RexaSafeRoutes")},
        {TEXT("Main.C03.03.TheFirstHolder"),TEXT("L_CrownCitadelApproach")},
        {TEXT("Main.C04.01.SaltAndIron"),TEXT("L_SaltAndIron_Blockade")},
        {TEXT("Main.C04.02.BrokenCompact"),TEXT("L_BrokenCompact")},
        {TEXT("Main.C04.03.HerrerasFall"),TEXT("L_HerrerasFall")},
        {TEXT("Main.C05.01.WarCurrent"),TEXT("L_WarCurrent")},
        {TEXT("Main.C05.02.HoldersWake"),TEXT("L_HoldersWake")},
        {TEXT("Main.C05.03.NoSafeHarbor"),TEXT("L_NoSafeHarbor")},
        {TEXT("Main.C06.01.HighmooreRoad"),TEXT("L_HighmooreRoad")},
        {TEXT("Main.C06.02.CrystalPassage"),TEXT("L_CrystalPassage")},
        {TEXT("Main.C06.03.TheNorthernOath"),TEXT("L_TheNorthernOath")},
        {TEXT("Main.C07.01.FalseBearings"),TEXT("L_FalseBearings")},
        {TEXT("Main.C07.02.EthansMarks"),TEXT("L_EthansMarks")},
        {TEXT("Main.C07.03.ThroughTheNet"),TEXT("L_ThroughTheNet")},
        {TEXT("Main.C08.01.ThePrisonCourse"),TEXT("L_ThePrisonCourse")},
        {TEXT("Main.C08.02.BrothersAlive"),TEXT("L_BrothersAlive")},
        {TEXT("Main.C08.03.HomewardBearing"),TEXT("L_HomewardBearing")},
        {TEXT("Main.C09.01.EthansGrove"),TEXT("L_EthansGrove")},
        {TEXT("Main.C09.02.MemoryOfABrother"),TEXT("L_MemoryOfABrother")},
        {TEXT("Main.C09.03.DreamFight"),TEXT("L_DreamFight")},
        {TEXT("Main.C09.04.WakingCourse"),TEXT("L_WakingCourse")},
        {TEXT("Main.C10.01.Armada"),TEXT("L_Armada")},
        {TEXT("Main.C10.02.BreakTheChain"),TEXT("L_BreakTheChain")},
        {TEXT("Main.C10.03.BlackDeck"),TEXT("L_BlackDeck")},
        {TEXT("Main.C10.04.DravenVoss"),TEXT("L_DravenVoss")},
        {TEXT("Main.C10.05.TheWakeAfter"),TEXT("L_TheWakeAfter")}
    };
    if(const FName* Map=Maps.Find(Id))return *Map;
    return NAME_None;
}
