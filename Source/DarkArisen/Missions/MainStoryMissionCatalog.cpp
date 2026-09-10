// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/MainStoryMissionCatalog.h"

namespace
{
    FMainStoryMissionDefinition Mission(
        const TCHAR* QuestId,
        const int32 Chapter,
        const int32 Sequence,
        const EQuestActivationTrigger Trigger,
        const TCHAR* RouteId,
        const TCHAR* OutcomeId,
        std::initializer_list<const TCHAR*> PersistenceFlags,
        const EMainStoryControlOwnership Control = EMainStoryControlOwnership::Playable,
        const TCHAR* PresentationSlot = TEXT("None"),
        const TCHAR* AlternateOutcomeId = nullptr)
    {
        FMainStoryMissionDefinition Result;
        Result.Activation.QuestId = FName(QuestId);
        Result.Activation.Trigger = Trigger;
        Result.Activation.bSilentAtActivation = true;
        Result.Chapter = Chapter;
        Result.SequenceInChapter = Sequence;
        Result.RouteId = FName(RouteId);
        Result.RequiredBeatIds.Add(FName(*(FString(QuestId) + TEXT(".Beat.Primary"))));
        Result.AllowedOutcomeIds.Add(FName(OutcomeId));
        if (AlternateOutcomeId && AlternateOutcomeId[0] != TEXT('\0'))
        {
            Result.AllowedOutcomeIds.Add(FName(AlternateOutcomeId));
        }
        for (const TCHAR* Flag : PersistenceFlags)
        {
            if (Flag && Flag[0] != TEXT('\0')) Result.PersistenceFlags.Add(FName(Flag));
        }
        Result.ControlOwnership = Control;
        Result.PresentationSlotId = FName(PresentationSlot);
        return Result;
    }

    bool HasMissionWithFlag(
        const TArray<FMainStoryMissionDefinition>& Missions,
        const FName QuestId,
        const FName Flag)
    {
        for (const FMainStoryMissionDefinition& Candidate : Missions)
        {
            if (Candidate.Activation.QuestId == QuestId && Candidate.PersistenceFlags.Contains(Flag))
            {
                return true;
            }
        }
        return false;
    }

    const FMainStoryMissionDefinition* FindMission(
        const TArray<FMainStoryMissionDefinition>& Missions,
        const FName QuestId)
    {
        for (const FMainStoryMissionDefinition& Candidate : Missions)
        {
            if (Candidate.Activation.QuestId == QuestId) return &Candidate;
        }
        return nullptr;
    }
}

TArray<FMainStoryMissionDefinition> UMainStoryMissionCatalog::GetAuthoredMissions()
{
    TArray<FMainStoryMissionDefinition> Missions;
    Missions.Reserve(34);

    // Chapter 1 — The Taking
    Missions.Add(Mission(TEXT("Main.C01.01.HomeWater"), 1, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C01.FamilyShip"), TEXT("FamilyWatchBegun"),
        {TEXT("Story.FamilyIntact"), TEXT("Story.EthanPresent")}));
    Missions.Add(Mission(TEXT("Main.C01.02.BlackSails"), 1, 2, EQuestActivationTrigger::Proximity,
        TEXT("Route.C01.Boarding"), TEXT("DravenReachedHarlows"),
        {TEXT("Story.DravenIdentified")}, EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C01.03.TheTaking"), 1, 3, EQuestActivationTrigger::WorldState,
        TEXT("Route.C01.Stern"), TEXT("JakeOverboard"),
        {TEXT("Story.MarcDead"), TEXT("Story.DeniseDead"), TEXT("Story.EthanAbducted"), TEXT("Story.JakeOverboard")},
        EMainStoryControlOwnership::BriefAuthoredTakeover, TEXT("Cutscene.01")));
    Missions.Add(Mission(TEXT("Main.C01.04.Undertow"), 1, 4, EQuestActivationTrigger::WorldState,
        TEXT("Route.C01.SeaToShore"), TEXT("JakeReachedShelter"),
        {TEXT("Story.JakeSurvived"), TEXT("Story.EthanAliveLead"), TEXT("Chapter.01.Complete")}));

    // Chapter 2 — La Liberación
    Missions.Add(Mission(TEXT("Main.C02.01.NoCoinNoFlag"), 2, 1, EQuestActivationTrigger::Conversation,
        TEXT("Route.C02.ShoreToDocks"), TEXT("DravenPortLead"),
        {TEXT("Crew.MiraKnown"), TEXT("Crew.BigTomKnown"), TEXT("Crew.EstebanKnown")}));
    Missions.Add(Mission(TEXT("Main.C02.02.AShipToTake"), 2, 2, EQuestActivationTrigger::Object,
        TEXT("Route.C02.ImpoundToHarbor"), TEXT("LaLiberacionClearedHarbor"),
        {TEXT("Ship.LaLiberacionOwned"), TEXT("Crew.CoreThreeJoined")}, EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C02.03.FirstWake"), 2, 3, EQuestActivationTrigger::WorldState,
        TEXT("Route.C02.FirstSeaPassage"), TEXT("FirstRegionalPortReached"),
        {TEXT("Travel.FreeSailingSeeded"), TEXT("Chapter.02.Complete")}));

    // Chapter 3 — The Archipelago
    Missions.Add(Mission(TEXT("Main.C03.01.ANameInInk"), 3, 1, EQuestActivationTrigger::Object,
        TEXT("Route.C03.PortWarehouse"), TEXT("EthanTransferDated"), {TEXT("Story.EthanAliveConfirmed")}));
    Missions.Add(Mission(TEXT("Main.C03.02.RedWake"), 3, 2, EQuestActivationTrigger::WorldState,
        TEXT("Route.C03.TenderPursuit"), TEXT("EthanShorthandRecovered"), {TEXT("Story.EthanRouteMarksFound")}));
    Missions.Add(Mission(TEXT("Main.C03.03.TheFirstHolder"), 3, 3, EQuestActivationTrigger::Proximity,
        TEXT("Route.C03.FirstHolder"), TEXT("RegionalNetworkOpened"),
        {TEXT("Story.FirstHolderCrossed"), TEXT("Chapter.03.Complete")}, EMainStoryControlOwnership::Hybrid));

    // Chapter 4 — The War
    Missions.Add(Mission(TEXT("Main.C04.01.ClosedHarbor"), 4, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C04.BlockadedHarbor"), TEXT("ContestedShoreReached"), {TEXT("War.VisibleToJake")}));
    Missions.Add(Mission(TEXT("Main.C04.02.ThreeVerbs"), 4, 2, EQuestActivationTrigger::Conversation,
        TEXT("Route.C04.ContestedSettlement"), TEXT("WarRouteChanged"), {TEXT("War.Playable")}));
    Missions.Add(Mission(TEXT("Main.C04.03.GlassCompass"), 4, 3, EQuestActivationTrigger::Object,
        TEXT("Route.C04.ArchiveToShip"), TEXT("CourierLaneRebuilt"),
        {TEXT("Story.HarlowChartRecovered"), TEXT("Chapter.04.Complete")}));

    // Chapter 5 — The Letter
    Missions.Add(Mission(TEXT("Main.C05.01.TheNavigator"), 5, 1, EQuestActivationTrigger::Object,
        TEXT("Route.C05.LetterDelivery"), TEXT("NavigatorAliasAuthenticated"), {TEXT("Story.NavigatorAliasKnown")},
        EMainStoryControlOwnership::BriefAuthoredTakeover, TEXT("Cutscene.Letter")));
    Missions.Add(Mission(TEXT("Main.C05.02.HuntersKnow"), 5, 2, EQuestActivationTrigger::Absence,
        TEXT("Route.C05.AssassinBackRoute"), TEXT("AssassinNetworkExposed"), {TEXT("Assassins.Unlocked")}));
    Missions.Add(Mission(TEXT("Main.C05.03.FourWaysToReadASea"), 5, 3, EQuestActivationTrigger::Conversation,
        TEXT("Route.C05.SageCircuit"), TEXT("DeepWaterCorridorDecoded"),
        {TEXT("Story.DeepRouteKnown"), TEXT("Chapter.05.Complete")}));

    // Chapter 6 — The Holding
    Missions.Add(Mission(TEXT("Main.C06.01.FirstHolding"), 6, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C06.HoldingApproach"), TEXT("FirstHoldingFunctional"), {TEXT("Holding.FirstOwned")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C06.02.SomethingThatCanBurn"), 6, 2, EQuestActivationTrigger::WorldState,
        TEXT("Route.C06.HoldingInterior"), TEXT("IntelligenceRoomReady"), {TEXT("Holding.IntelligenceReady")}));
    Missions.Add(Mission(TEXT("Main.C06.03.EthansHand"), 6, 3, EQuestActivationTrigger::Object,
        TEXT("Route.C06.ChartTable"), TEXT("ResistancePatternProven"),
        {TEXT("Story.EthanResisting"), TEXT("Chapter.06.Complete")},
        EMainStoryControlOwnership::BriefAuthoredTakeover, TEXT("Cutscene.EthansHand")));

    // Chapter 7 — The Cost
    Missions.Add(Mission(TEXT("Main.C07.01.TheCost"), 7, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C07.Retaliation"), TEXT("RetaliationResolved"), {TEXT("Story.RetaliationPaid")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C07.02.NoCleanLedger"), 7, 2, EQuestActivationTrigger::Object,
        TEXT("Route.C07.ConvoyToPrisonLedger"), TEXT("EthanPurposeConfirmed"), {TEXT("Story.EthanUsedAsNavigator")}));
    Missions.Add(Mission(TEXT("Main.C07.03.ThePriceOfFollowing"), 7, 3, EQuestActivationTrigger::Conversation,
        TEXT("Route.C07.GreatCabin"), TEXT("RescuePlanCommitted"),
        {TEXT("Story.RescueBeforeRevenge"), TEXT("Chapter.07.Complete")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.CabinCost")));

    // Chapter 8 — The Turn
    Missions.Add(Mission(TEXT("Main.C08.01.TheArmada"), 8, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C08.ArmadaReveal"), TEXT("PrisonShipLocated"), {TEXT("Armada.Revealed")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C08.02.ThePrisonCourse"), 8, 2, EQuestActivationTrigger::Object,
        TEXT("Route.C08.FalseBearings"), TEXT("PrisonShipIntercepted"), {TEXT("Story.EthanSignalUnderstood")}));
    Missions.Add(Mission(TEXT("Main.C08.03.Brothers"), 8, 3, EQuestActivationTrigger::Proximity,
        TEXT("Route.C08.PrisonShipEscape"), TEXT("EthanBoardedLaLiberacion"),
        {TEXT("Story.EthanRecovered"), TEXT("Story.EthanNoLongerCaptive"), TEXT("Chapter.08.Complete")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.Brothers")));

    // Chapter 9 — Rache
    Missions.Add(Mission(TEXT("Main.C09.01.WhatDravenWanted"), 9, 1, EQuestActivationTrigger::Conversation,
        TEXT("Route.C09.GreatCabin"), TEXT("DravenMotiveEstablished"), {TEXT("Story.DravenMotiveKnown")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C09.02.EthansGrove"), 9, 2, EQuestActivationTrigger::Proximity,
        TEXT("Route.C09.EthansGrove"), TEXT("RememberedClearingReached"), {TEXT("Story.GroveVisited")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.04")));
    Missions.Add(Mission(TEXT("Main.C09.03.TheDreamFight"), 9, 3, EQuestActivationTrigger::WorldState,
        TEXT("Route.C09.DreamFight"), TEXT("DreamIntegrated"), {TEXT("Story.DreamResolved")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.05")));
    Missions.Add(Mission(TEXT("Main.C09.04.WhatRageIsFor"), 9, 4, EQuestActivationTrigger::WorldState,
        TEXT("Route.C09.GroveToShip"), TEXT("FinalRouteLocked"),
        {TEXT("Combat.RacheUnlocked"), TEXT("Story.FinalRouteKnown"), TEXT("Chapter.09.Complete")}));

    // Chapter 10 — The Armada
    Missions.Add(Mission(TEXT("Main.C10.01.FiveWaysIn"), 10, 1, EQuestActivationTrigger::WorldState,
        TEXT("Route.C10.ArmadaPerimeter"), TEXT("ArmadaPerimeterBreached"), {TEXT("Finale.EntryStateLocked")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C10.02.BreakTheChain"), 10, 2, EQuestActivationTrigger::WorldState,
        TEXT("Route.C10.OuterFleet"), TEXT("FleetCohesionBroken"), {TEXT("Armada.LogisticsBroken")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C10.03.TheBlackDeck"), 10, 3, EQuestActivationTrigger::Proximity,
        TEXT("Route.C10.FlagshipBoarding"), TEXT("DravenReached"), {TEXT("Finale.DravenReached")},
        EMainStoryControlOwnership::Hybrid));
    Missions.Add(Mission(TEXT("Main.C10.04.DravenVoss"), 10, 4, EQuestActivationTrigger::WorldState,
        TEXT("Route.C10.CommandDeck"), TEXT("DravenKilled"), {TEXT("Story.DravenDefeated")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.FinalDuel"), TEXT("DravenCaptured")));
    Missions.Add(Mission(TEXT("Main.C10.05.TheWakeAfter"), 10, 5, EQuestActivationTrigger::WorldState,
        TEXT("Route.C10.AftermathToOpenSea"), TEXT("Credits"),
        {TEXT("Story.MainComplete"), TEXT("Chapter.10.Complete")},
        EMainStoryControlOwnership::Hybrid, TEXT("Cutscene.CreditsLeadIn")));

    return Missions;
}

bool UMainStoryMissionCatalog::RegisterAuthoredMissions(UQuestJournalComponent* Journal)
{
    const TArray<FMainStoryMissionDefinition> Missions = GetAuthoredMissions();
    if (!Journal || !IsCatalogValid(Missions)) return false;
    for (const FMainStoryMissionDefinition& MissionDefinition : Missions)
    {
        if (!Journal->RegisterQuestDefinition(MissionDefinition.Activation)) return false;
    }
    return true;
}

bool UMainStoryMissionCatalog::TryGetMission(
    const FName QuestId,
    FMainStoryMissionDefinition& OutMission)
{
    if (QuestId.IsNone()) return false;
    const TArray<FMainStoryMissionDefinition> Missions = GetAuthoredMissions();
    if (const FMainStoryMissionDefinition* MissionDefinition = FindMission(Missions, QuestId))
    {
        OutMission = *MissionDefinition;
        return true;
    }
    return false;
}

bool UMainStoryMissionCatalog::IsCatalogValid(
    const TArray<FMainStoryMissionDefinition>& Missions)
{
    static constexpr int32 ExpectedPerChapter[10] = {4, 3, 3, 3, 3, 3, 3, 3, 4, 5};
    if (Missions.Num() != 34) return false;

    TSet<FName> QuestIds;
    int32 SeenPerChapter[10] = {};
    int32 LastChapter = 0;
    int32 LastSequence = 0;

    for (const FMainStoryMissionDefinition& MissionDefinition : Missions)
    {
        const FQuestActivationDefinition& Activation = MissionDefinition.Activation;
        if (Activation.QuestId.IsNone() || Activation.Trigger == EQuestActivationTrigger::Count ||
            !Activation.bSilentAtActivation || MissionDefinition.Chapter < 1 || MissionDefinition.Chapter > 10 ||
            MissionDefinition.SequenceInChapter < 1 || MissionDefinition.RouteId.IsNone() ||
            MissionDefinition.RequiredBeatIds.IsEmpty() || MissionDefinition.AllowedOutcomeIds.IsEmpty() ||
            MissionDefinition.PersistenceFlags.IsEmpty() || QuestIds.Contains(Activation.QuestId))
        {
            return false;
        }

        const FString QuestString = Activation.QuestId.ToString();
        if (QuestString.Contains(TEXT("EthanBoss"), ESearchCase::IgnoreCase) ||
            QuestString.Contains(TEXT("Ethan.Boss"), ESearchCase::IgnoreCase) ||
            QuestString.Contains(TEXT("Betrayal"), ESearchCase::IgnoreCase))
        {
            return false;
        }

        if (MissionDefinition.Chapter < LastChapter ||
            (MissionDefinition.Chapter == LastChapter && MissionDefinition.SequenceInChapter != LastSequence + 1) ||
            (MissionDefinition.Chapter > LastChapter && MissionDefinition.SequenceInChapter != 1))
        {
            return false;
        }
        if (MissionDefinition.Chapter > LastChapter)
        {
            LastChapter = MissionDefinition.Chapter;
            LastSequence = 0;
        }
        LastSequence = MissionDefinition.SequenceInChapter;
        ++SeenPerChapter[MissionDefinition.Chapter - 1];
        QuestIds.Add(Activation.QuestId);
    }

    for (int32 Index = 0; Index < 10; ++Index)
    {
        if (SeenPerChapter[Index] != ExpectedPerChapter[Index]) return false;
    }

    const bool bTakingLocked = HasMissionWithFlag(
        Missions, TEXT("Main.C01.03.TheTaking"), TEXT("Story.EthanAbducted"));
    const bool bParentsLocked = HasMissionWithFlag(
        Missions, TEXT("Main.C01.03.TheTaking"), TEXT("Story.MarcDead")) &&
        HasMissionWithFlag(Missions, TEXT("Main.C01.03.TheTaking"), TEXT("Story.DeniseDead"));
    const bool bRescueLocked = HasMissionWithFlag(
        Missions, TEXT("Main.C08.03.Brothers"), TEXT("Story.EthanRecovered"));
    const bool bEndingLocked = HasMissionWithFlag(
        Missions, TEXT("Main.C10.05.TheWakeAfter"), TEXT("Story.MainComplete"));

    const FMainStoryMissionDefinition* DravenDuel = FindMission(Missions, TEXT("Main.C10.04.DravenVoss"));
    const bool bDravenOutcomesLocked = DravenDuel &&
        DravenDuel->AllowedOutcomeIds.Contains(TEXT("DravenKilled")) &&
        DravenDuel->AllowedOutcomeIds.Contains(TEXT("DravenCaptured"));

    return bTakingLocked && bParentsLocked && bRescueLocked && bEndingLocked && bDravenOutcomesLocked;
}
