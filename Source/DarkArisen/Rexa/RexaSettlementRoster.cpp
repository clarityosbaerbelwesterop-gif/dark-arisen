// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementRoster.h"

#include <initializer_list>

namespace
{
    FRexaResidentDefinition Resident(
        const TCHAR* Id,
        const TCHAR* Name,
        const ERexaResidentHeritage Heritage,
        const ERexaResidentAgeBand AgeBand,
        const TCHAR* Occupation,
        const FVector& HomeOffset,
        const TCHAR* Dawn,
        const TCHAR* Midday,
        const TCHAR* Evening,
        const TCHAR* Night,
        std::initializer_list<const TCHAR*> Knowledge = {},
        const TCHAR* ChildSafetyAnchor = nullptr)
    {
        FRexaResidentDefinition Result;
        Result.StableResidentId = FName(Id);
        Result.DisplayName = FText::FromString(Name);
        Result.Heritage = Heritage;
        Result.AgeBand = AgeBand;
        Result.OccupationId = FName(Occupation);
        Result.HomeOffsetCentimetres = HomeOffset;
        Result.DawnAnchorId = FName(Dawn);
        Result.MiddayAnchorId = FName(Midday);
        Result.EveningAnchorId = FName(Evening);
        Result.NightAnchorId = FName(Night);
        Result.bProtectedChild = AgeBand == ERexaResidentAgeBand::Child;
        if (ChildSafetyAnchor) Result.ChildSafetyAnchorId = FName(ChildSafetyAnchor);
        for (const TCHAR* KnowledgeId : Knowledge) Result.KnowledgeIds.Add(FName(KnowledgeId));
        return Result;
    }
}

FName FRexaResidentDefinition::GetPurposeAnchorAtGameMinute(const int64 GameMinute) const
{
    if (GameMinute < 0) return NAME_None;
    const int64 MinuteOfDay = GameMinute % 1440;
    if (MinuteOfDay >= 300 && MinuteOfDay < 660) return DawnAnchorId;
    if (MinuteOfDay >= 660 && MinuteOfDay < 900) return MiddayAnchorId;
    if (MinuteOfDay >= 900 && MinuteOfDay < 1320) return EveningAnchorId;
    return NightAnchorId;
}

TArray<FRexaResidentDefinition> URexaSettlementRoster::GetAuthoredResidents()
{
    using Heritage = ERexaResidentHeritage;
    using Age = ERexaResidentAgeBand;
    TArray<FRexaResidentDefinition> Residents;
    Residents.Reserve(RequiredResidentCount);

    // Indigenous Rexan — sixteen residents (40%).
    Residents.Add(Resident(TEXT("Raices.Resident.01"), TEXT("Yara Maku"), Heritage::IndigenousRexan, Age::Elder, TEXT("Elder"), FVector(-900, -500, 0), TEXT("Raices.Anchor.Well"), TEXT("Raices.Anchor.ElderShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeWest"), {TEXT("Raices.Elder")}));
    Residents.Add(Resident(TEXT("Raices.Resident.02"), TEXT("Raimundo Iri"), Heritage::IndigenousRexan, Age::Adult, TEXT("NetMender"), FVector(-700, -520, 0), TEXT("Raices.Anchor.NetYard"), TEXT("Raices.Anchor.NetYardShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.HomeWest"), {TEXT("Raices.NetMender")}));
    Residents.Add(Resident(TEXT("Raices.Resident.03"), TEXT("Amaya Neri"), Heritage::IndigenousRexan, Age::Adult, TEXT("WaterKeeper"), FVector(-500, -540, 0), TEXT("Raices.Anchor.Well"), TEXT("Raices.Anchor.CisternShade"), TEXT("Raices.Anchor.Well"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.04"), TEXT("Kanu Sola"), Heritage::IndigenousRexan, Age::Adult, TEXT("Fisher"), FVector(-300, -560, 0), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.BoatShade"), TEXT("Raices.Anchor.FishTable"), TEXT("Raices.Anchor.HomeWest"), {TEXT("Raices.Fisher")}));
    Residents.Add(Resident(TEXT("Raices.Resident.05"), TEXT("Lucía Vara"), Heritage::IndigenousRexan, Age::Adult, TEXT("WreckDiver"), FVector(-100, -580, 0), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.BoatShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeWest"), {TEXT("Raices.WreckDiver")}));
    Residents.Add(Resident(TEXT("Raices.Resident.06"), TEXT("Ema Tarek"), Heritage::IndigenousRexan, Age::Adult, TEXT("Weaver"), FVector(100, -580, 0), TEXT("Raices.Anchor.DyeYard"), TEXT("Raices.Anchor.WeavingShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.07"), TEXT("Nilo Ara"), Heritage::IndigenousRexan, Age::Adult, TEXT("CanoeMaker"), FVector(300, -560, 0), TEXT("Raices.Anchor.BoatYard"), TEXT("Raices.Anchor.BoatShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.08"), TEXT("Rosa Yal"), Heritage::IndigenousRexan, Age::Adult, TEXT("Herbalist"), FVector(500, -540, 0), TEXT("Raices.Anchor.HerbGarden"), TEXT("Raices.Anchor.ClinicShade"), TEXT("Raices.Anchor.Clinic"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.09"), TEXT("Ivo Kema"), Heritage::IndigenousRexan, Age::Adult, TEXT("CassavaGrower"), FVector(700, -520, 0), TEXT("Raices.Anchor.CassavaPlots"), TEXT("Raices.Anchor.FieldShade"), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.HomeSouth"), {TEXT("Raices.CassavaGrower")}));
    Residents.Add(Resident(TEXT("Raices.Resident.10"), TEXT("Mara Suni"), Heritage::IndigenousRexan, Age::Adult, TEXT("Teacher"), FVector(900, -500, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.11"), TEXT("Lio Bara"), Heritage::IndigenousRexan, Age::Adult, TEXT("Potter"), FVector(-900, -200, 0), TEXT("Raices.Anchor.Kiln"), TEXT("Raices.Anchor.KilnShade"), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.12"), TEXT("Noa Iri"), Heritage::IndigenousRexan, Age::Child, TEXT("RiverChild"), FVector(-700, -200, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.FerryStep"), TEXT("Raices.Anchor.HomeWest"), {TEXT("Raices.RiverChild")}, TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.13"), TEXT("Ana Maku"), Heritage::IndigenousRexan, Age::Child, TEXT("Learner"), FVector(-500, -220, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.CommonYard"), TEXT("Raices.Anchor.HomeWest"), {}, TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.14"), TEXT("Sami Neri"), Heritage::IndigenousRexan, Age::Child, TEXT("Learner"), FVector(-300, -240, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.Well"), TEXT("Raices.Anchor.HomeWest"), {}, TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.15"), TEXT("Bela Tarek"), Heritage::IndigenousRexan, Age::Adolescent, TEXT("WeaverApprentice"), FVector(-100, -260, 0), TEXT("Raices.Anchor.DyeYard"), TEXT("Raices.Anchor.WeavingShade"), TEXT("Raices.Anchor.CommonYard"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.16"), TEXT("Teo Vara"), Heritage::IndigenousRexan, Age::Elder, TEXT("StoryKeeper"), FVector(100, -260, 0), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.ElderShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeSouth")));

    // Mixed Rexan — twelve residents (30%).
    Residents.Add(Resident(TEXT("Raices.Resident.17"), TEXT("Inés Calder"), Heritage::MixedRexan, Age::Adult, TEXT("Baker"), FVector(300, -240, 0), TEXT("Raices.Anchor.Bakery"), TEXT("Raices.Anchor.BakeryShade"), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.18"), TEXT("Mateo Yara"), Heritage::MixedRexan, Age::Adult, TEXT("Boatwright"), FVector(500, -220, 0), TEXT("Raices.Anchor.BoatYard"), TEXT("Raices.Anchor.BoatShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.19"), TEXT("Pilar Sol"), Heritage::MixedRexan, Age::Adult, TEXT("Dyer"), FVector(700, -200, 0), TEXT("Raices.Anchor.DyeYard"), TEXT("Raices.Anchor.WeavingShade"), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.HomeEast")));
    Residents.Add(Resident(TEXT("Raices.Resident.20"), TEXT("Gabriel Maku"), Heritage::MixedRexan, Age::Adult, TEXT("SmokehouseKeeper"), FVector(900, -180, 0), TEXT("Raices.Anchor.Smokehouse"), TEXT("Raices.Anchor.SmokehouseShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeEast")));
    Residents.Add(Resident(TEXT("Raices.Resident.21"), TEXT("Elena Varo"), Heritage::MixedRexan, Age::Adult, TEXT("Midwife"), FVector(-900, 100, 0), TEXT("Raices.Anchor.Clinic"), TEXT("Raices.Anchor.ClinicShade"), TEXT("Raices.Anchor.Well"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.22"), TEXT("Nicolás Ara"), Heritage::MixedRexan, Age::Adult, TEXT("Carpenter"), FVector(-700, 100, 0), TEXT("Raices.Anchor.WorkYard"), TEXT("Raices.Anchor.WorkShade"), TEXT("Raices.Anchor.CommonYard"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.23"), TEXT("Sofía Ríos"), Heritage::MixedRexan, Age::Adult, TEXT("Cook"), FVector(-500, 80, 0), TEXT("Raices.Anchor.Kitchen"), TEXT("Raices.Anchor.KitchenShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.24"), TEXT("Daniel Neri"), Heritage::MixedRexan, Age::Adult, TEXT("ReedCutter"), FVector(-300, 60, 0), TEXT("Raices.Anchor.ReedPath"), TEXT("Raices.Anchor.FieldShade"), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.25"), TEXT("Carmen Sola"), Heritage::MixedRexan, Age::Elder, TEXT("FishSeller"), FVector(-100, 40, 0), TEXT("Raices.Anchor.FishTable"), TEXT("Raices.Anchor.MarketShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.26"), TEXT("Alma Vega"), Heritage::MixedRexan, Age::Child, TEXT("Learner"), FVector(100, 40, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.CommonYard"), TEXT("Raices.Anchor.HomeSouth"), {}, TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.27"), TEXT("Luis Calder"), Heritage::MixedRexan, Age::Child, TEXT("Learner"), FVector(300, 60, 0), TEXT("Raices.Anchor.School"), TEXT("Raices.Anchor.SchoolShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.HomeSouth"), {}, TEXT("Raices.Anchor.HomeSouth")));
    Residents.Add(Resident(TEXT("Raices.Resident.28"), TEXT("Eva Marín"), Heritage::MixedRexan, Age::Adolescent, TEXT("BakerApprentice"), FVector(500, 80, 0), TEXT("Raices.Anchor.Bakery"), TEXT("Raices.Anchor.BakeryShade"), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.HomeEast")));

    // Imperial colonists — eight residents (20%).
    Residents.Add(Resident(TEXT("Raices.Resident.29"), TEXT("Beatriz Ortega"), Heritage::ImperialColonist, Age::Adult, TEXT("Clerk"), FVector(700, 100, 0), TEXT("Raices.Anchor.CountingRoom"), TEXT("Raices.Anchor.CountingRoom"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeEast")));
    Residents.Add(Resident(TEXT("Raices.Resident.30"), TEXT("Hernán Ruiz"), Heritage::ImperialColonist, Age::Adult, TEXT("Trader"), FVector(900, 120, 0), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.MarketShade"), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.HomeEast")));
    Residents.Add(Resident(TEXT("Raices.Resident.31"), TEXT("Clara Valdés"), Heritage::ImperialColonist, Age::Adult, TEXT("Physician"), FVector(-900, 400, 0), TEXT("Raices.Anchor.Clinic"), TEXT("Raices.Anchor.ClinicShade"), TEXT("Raices.Anchor.Clinic"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.32"), TEXT("Esteban Mora"), Heritage::ImperialColonist, Age::Adult, TEXT("Cooper"), FVector(-700, 400, 0), TEXT("Raices.Anchor.WorkYard"), TEXT("Raices.Anchor.WorkShade"), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.33"), TEXT("Jacinto León"), Heritage::ImperialColonist, Age::Adult, TEXT("Cook"), FVector(-500, 420, 0), TEXT("Raices.Anchor.Kitchen"), TEXT("Raices.Anchor.KitchenShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.34"), TEXT("Teresa Blanco"), Heritage::ImperialColonist, Age::Adult, TEXT("Tailor"), FVector(-300, 440, 0), TEXT("Raices.Anchor.WeavingRoom"), TEXT("Raices.Anchor.WeavingShade"), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.HomeWest")));
    Residents.Add(Resident(TEXT("Raices.Resident.35"), TEXT("Martín Costa"), Heritage::ImperialColonist, Age::Adult, TEXT("DockWorker"), FVector(-100, 460, 0), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.DockShade"), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.HomeSouth"), {TEXT("Raices.DockWorker")}));
    Residents.Add(Resident(TEXT("Raices.Resident.36"), TEXT("Isabel Sáez"), Heritage::ImperialColonist, Age::Elder, TEXT("Bookkeeper"), FVector(100, 460, 0), TEXT("Raices.Anchor.CountingRoom"), TEXT("Raices.Anchor.CountingRoom"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.HomeSouth")));

    // Sailors and traders — four residents (10%).
    Residents.Add(Resident(TEXT("Raices.Resident.37"), TEXT("Asha Dlamini"), Heritage::SailorTrader, Age::Adult, TEXT("SailMender"), FVector(300, 440, 0), TEXT("Raices.Anchor.SailYard"), TEXT("Raices.Anchor.SailShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.GuestHouse")));
    Residents.Add(Resident(TEXT("Raices.Resident.38"), TEXT("Olek Petrov"), Heritage::SailorTrader, Age::Adult, TEXT("BoatHand"), FVector(500, 420, 0), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.DockShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.GuestHouse")));
    Residents.Add(Resident(TEXT("Raices.Resident.39"), TEXT("Kwame Mensah"), Heritage::SailorTrader, Age::Adult, TEXT("CargoHandler"), FVector(700, 400, 0), TEXT("Raices.Anchor.Storehouse"), TEXT("Raices.Anchor.DockShade"), TEXT("Raices.Anchor.LowerDock"), TEXT("Raices.Anchor.GuestHouse")));
    Residents.Add(Resident(TEXT("Raices.Resident.40"), TEXT("Maeve Quinn"), Heritage::SailorTrader, Age::Adult, TEXT("ChartSeller"), FVector(900, 380, 0), TEXT("Raices.Anchor.Market"), TEXT("Raices.Anchor.MarketShade"), TEXT("Raices.Anchor.CommonTable"), TEXT("Raices.Anchor.GuestHouse")));

    return Residents;
}

bool URexaSettlementRoster::IsRosterValid(const TArray<FRexaResidentDefinition>& Residents)
{
    if (Residents.Num() != RequiredResidentCount) return false;
    int32 HeritageCounts[4] = {0, 0, 0, 0};
    int32 ChildCount = 0;
    TSet<FName> StableIds;
    TSet<FString> DisplayNames;
    TSet<FName> KnowledgeIds;
    for (const FRexaResidentDefinition& ResidentDefinition : Residents)
    {
        const int32 HeritageIndex = static_cast<int32>(ResidentDefinition.Heritage);
        if (HeritageIndex < 0 || HeritageIndex >= UE_ARRAY_COUNT(HeritageCounts) ||
            ResidentDefinition.StableResidentId.IsNone() ||
            ResidentDefinition.DisplayName.IsEmpty() || ResidentDefinition.OccupationId.IsNone() ||
            ResidentDefinition.HomeOffsetCentimetres.ContainsNaN() ||
            ResidentDefinition.DawnAnchorId.IsNone() ||
            ResidentDefinition.MiddayAnchorId.IsNone() ||
            ResidentDefinition.EveningAnchorId.IsNone() ||
            ResidentDefinition.NightAnchorId.IsNone() ||
            StableIds.Contains(ResidentDefinition.StableResidentId) ||
            DisplayNames.Contains(ResidentDefinition.DisplayName.ToString())) return false;
        StableIds.Add(ResidentDefinition.StableResidentId);
        DisplayNames.Add(ResidentDefinition.DisplayName.ToString());
        ++HeritageCounts[HeritageIndex];

        const bool bIsChild = ResidentDefinition.AgeBand == ERexaResidentAgeBand::Child;
        if (ResidentDefinition.bProtectedChild != bIsChild ||
            (bIsChild && ResidentDefinition.ChildSafetyAnchorId.IsNone()) ||
            (!bIsChild && !ResidentDefinition.ChildSafetyAnchorId.IsNone())) return false;
        if (bIsChild) ++ChildCount;
        for (const FName KnowledgeId : ResidentDefinition.KnowledgeIds)
        {
            if (KnowledgeId.IsNone() || KnowledgeIds.Contains(KnowledgeId)) return false;
            KnowledgeIds.Add(KnowledgeId);
        }
    }
    static const TSet<FName> RequiredKnowledgeIds = {
        TEXT("Raices.Elder"),
        TEXT("Raices.NetMender"),
        TEXT("Raices.RiverChild"),
        TEXT("Raices.CassavaGrower"),
        TEXT("Raices.Fisher"),
        TEXT("Raices.DockWorker"),
        TEXT("Raices.WreckDiver")
    };
    for (const FName RequiredKnowledgeId : RequiredKnowledgeIds)
    {
        if (!KnowledgeIds.Contains(RequiredKnowledgeId)) return false;
    }
    return HeritageCounts[0] == RequiredIndigenousCount &&
        HeritageCounts[1] == RequiredMixedCount &&
        HeritageCounts[2] == RequiredImperialCount &&
        HeritageCounts[3] == RequiredSailorTraderCount &&
        ChildCount == 5;
}
