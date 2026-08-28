// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/RexaM2MissionCatalog.h"

namespace
{
    FLocalDirectionVariant Direction(
        const TCHAR* Speaker,
        const TCHAR* Entry,
        const FText& JakeText,
        const FText& SpokenDirections,
        const bool bDistorted = false)
    {
        FLocalDirectionVariant Result;
        Result.SpeakerKnowledgeId = FName(Speaker);
        Result.EntryId = FName(Entry);
        Result.JakeText = JakeText;
        Result.SpokenDirections = SpokenDirections;
        Result.bDistorted = bDistorted;
        return Result;
    }

    FRexaM2MissionDefinition EmptyHammock()
    {
        FRexaM2MissionDefinition Mission;
        Mission.Activation.QuestId = TEXT("Rexa.Turn.EmptyHammock");
        Mission.Activation.Trigger = EQuestActivationTrigger::Absence;
        Mission.Activation.bSilentAtActivation = true;
        Mission.Tier = EQuestStructuralTier::Turn;
        Mission.AuthoredVariantId = TEXT("Rexa.Turn.01");
        Mission.LocalDirections = {
            Direction(
                TEXT("Raices.NetMender"),
                TEXT("Rexa.EmptyHammock.Direction.NetMender"),
                NSLOCTEXT("RexaM2", "EmptyHammockNetJake", "The net-mender noticed Tomás was gone before dawn."),
                NSLOCTEXT("RexaM2", "EmptyHammockNetDirection", "Take the plank walk behind the smokehouse. His skiff scraped the red mangrove roots, heading upriver.")),
            Direction(
                TEXT("Raices.RiverChild"),
                TEXT("Rexa.EmptyHammock.Direction.RiverChild"),
                NSLOCTEXT("RexaM2", "EmptyHammockChildJake", "A child saw Tomás hide something under the old ferry step."),
                NSLOCTEXT("RexaM2", "EmptyHammockChildDirection", "The ferry step is below the kapok with the split trunk. Wait for the water to fall or you will miss it."))
        };
        Mission.AuthoredOutcomeIds = {
            TEXT("TomasReturned"),
            TEXT("TomasLeftByChoice"),
            TEXT("TomasTakenByPatrol")
        };
        return Mission;
    }

    FRexaM2MissionDefinition ThreeCutsInStone()
    {
        FRexaM2MissionDefinition Mission;
        Mission.Activation.QuestId = TEXT("Rexa.Turn.ThreeCutsInStone");
        Mission.Activation.Trigger = EQuestActivationTrigger::Overheard;
        Mission.Activation.InitialEntryId = TEXT("Rexa.ThreeCuts.Rumour");
        Mission.Activation.InitialJakeText = NSLOCTEXT(
            "RexaM2", "ThreeCutsInitialJake",
            "Two traders said a lantern walks by itself beyond a stone cut three times.");
        Mission.Activation.InitialDirections = NSLOCTEXT(
            "RexaM2", "ThreeCutsInitialDirections",
            "Past the western cassava plots, where the raised path first sinks into black water.");
        Mission.Activation.bInitialEntryDistorted = true;
        Mission.Tier = EQuestStructuralTier::Turn;
        Mission.AuthoredVariantId = TEXT("Rexa.Turn.02");
        Mission.LocalDirections = {
            Direction(
                TEXT("Raices.CassavaGrower"),
                TEXT("Rexa.ThreeCuts.Direction.Grower"),
                NSLOCTEXT("RexaM2", "ThreeCutsGrowerJake", "The grower says the three cuts mark an old flood measure, not a grave."),
                NSLOCTEXT("RexaM2", "ThreeCutsGrowerDirection", "Keep the drainage ditch on your left until it meets the flooded ceiba. The stone leans toward the water.")),
            Direction(
                TEXT("Raices.Fisher"),
                TEXT("Rexa.ThreeCuts.Direction.Fisher"),
                NSLOCTEXT("RexaM2", "ThreeCutsFisherJake", "The fisher has seen the light. Someone carries it low inside the reeds."),
                NSLOCTEXT("RexaM2", "ThreeCutsFisherDirection", "Approach from the narrow channel after dusk. From the footpath they will hear you before you see them."))
        };
        Mission.AuthoredOutcomeIds = {
            TEXT("FamilySheltered"),
            TEXT("FamilyMovedOn"),
            TEXT("PatrolInformed")
        };
        return Mission;
    }

    FRexaM2MissionDefinition SaltLedger()
    {
        FRexaM2MissionDefinition Mission;
        Mission.Activation.QuestId = TEXT("Rexa.Turn.SaltLedger");
        Mission.Activation.Trigger = EQuestActivationTrigger::Object;
        Mission.Activation.InitialEntryId = TEXT("Rexa.SaltLedger.Found");
        Mission.Activation.InitialJakeText = NSLOCTEXT(
            "RexaM2", "SaltLedgerInitialJake",
            "A dock ledger records wages paid in salt, then carries the same sums forward as debt.");
        Mission.Activation.InitialDirections = NSLOCTEXT(
            "RexaM2", "SaltLedgerInitialDirections",
            "The names are from Las Raíces. Someone here will know which hands still live.");
        Mission.Tier = EQuestStructuralTier::Turn;
        Mission.AuthoredVariantId = TEXT("Rexa.Turn.03");
        Mission.LocalDirections = {
            Direction(
                TEXT("Raices.Elder"),
                TEXT("Rexa.SaltLedger.Direction.Elder"),
                NSLOCTEXT("RexaM2", "SaltLedgerElderJake", "Yara knew three names without opening the book."),
                NSLOCTEXT("RexaM2", "SaltLedgerElderDirection", "The first family works the dye yard. The second keeps boats beyond the lower dock. Ask about the third only inside my house.")),
            Direction(
                TEXT("Raices.DockWorker"),
                TEXT("Rexa.SaltLedger.Direction.Dock"),
                NSLOCTEXT("RexaM2", "SaltLedgerDockJake", "The dock workers say the company clerk still copies accounts at San Rafael."),
                NSLOCTEXT("RexaM2", "SaltLedgerDockDirection", "Enter below the eastern gun stair. His room faces the cistern because it is the only cool office."))
        };
        Mission.AuthoredOutcomeIds = {
            TEXT("FamiliesKeepLedger"),
            TEXT("LedgerCopiedForCruzList"),
            TEXT("ClerkKeepsLedger")
        };
        return Mission;
    }

    FRexaM2MissionDefinition SanTelmoBell()
    {
        FRexaM2MissionDefinition Mission;
        Mission.Activation.QuestId = TEXT("Rexa.Standing.Salvage.SanTelmoBell");
        Mission.Activation.Trigger = EQuestActivationTrigger::Conversation;
        Mission.Activation.bRequiresSpokenAgreement = true;
        Mission.Activation.InitialEntryId = TEXT("Rexa.SanTelmoBell.Agreement");
        Mission.Activation.InitialJakeText = NSLOCTEXT(
            "RexaM2", "SanTelmoInitialJake",
            "I agreed to dive the San Telmo wreck for its bell. Lucía wants the name cast under its rim, not the bronze.");
        Mission.Activation.InitialDirections = NSLOCTEXT(
            "RexaM2", "SanTelmoInitialDirections",
            "Follow the mangrove channel south from Las Raíces. The wreck lies beyond the white snag where river water first tastes of salt.");
        Mission.Tier = EQuestStructuralTier::Standing;
        Mission.AuthoredVariantId = TEXT("Rexa.Standing.Salvage.01");
        Mission.StandingType = EStandingMissionType::Salvage;
        Mission.LocalDirections = {
            Direction(
                TEXT("Raices.WreckDiver"),
                TEXT("Rexa.SanTelmo.Direction.Diver"),
                NSLOCTEXT("RexaM2", "SanTelmoDiverJake", "Lucía says the bell is below the stern, pinned under a spar."),
                NSLOCTEXT("RexaM2", "SanTelmoDiverDirection", "Dive on the slack water after the river turns. Go under from the seaward side; the deck boards lift with the current.")),
            Direction(
                TEXT("Raices.Fisher"),
                TEXT("Rexa.SanTelmo.Direction.Fisher"),
                NSLOCTEXT("RexaM2", "SanTelmoFisherJake", "The fishers avoid the wreck when the current carries green leaves."),
                NSLOCTEXT("RexaM2", "SanTelmoFisherDirection", "If leaves pass the lower dock, wait. The channel will pull you through the broken hull."),
                true)
        };
        Mission.AuthoredOutcomeIds = {
            TEXT("NameReturnedToLucia"),
            TEXT("BellLeftWithWreck"),
            TEXT("BellGivenToFamily")
        };
        return Mission;
    }
}

TArray<FRexaM2MissionDefinition> URexaM2MissionCatalog::GetAuthoredMissions()
{
    return {EmptyHammock(), ThreeCutsInStone(), SaltLedger(), SanTelmoBell()};
}

bool URexaM2MissionCatalog::RegisterAuthoredMissions(UQuestJournalComponent* Journal)
{
    const TArray<FRexaM2MissionDefinition> Missions = GetAuthoredMissions();
    if (!Journal || !IsCatalogValid(Missions)) return false;
    for (const FRexaM2MissionDefinition& Mission : Missions)
    {
        if (!Journal->RegisterQuestDefinition(Mission.Activation)) return false;
    }
    return true;
}

bool URexaM2MissionCatalog::AppendLocalDirection(
    UQuestJournalComponent* Journal,
    const FName QuestId,
    const FName SpeakerKnowledgeId,
    const int64 GameMinute)
{
    if (!Journal || QuestId.IsNone() || SpeakerKnowledgeId.IsNone()) return false;
    for (const FRexaM2MissionDefinition& Mission : GetAuthoredMissions())
    {
        if (Mission.Activation.QuestId != QuestId) continue;
        for (const FLocalDirectionVariant& DirectionVariant : Mission.LocalDirections)
        {
            if (DirectionVariant.SpeakerKnowledgeId != SpeakerKnowledgeId) continue;
            if (!Journal->HasJournalEntryForQuest(QuestId))
            {
                return Journal->RevealSilentQuest(
                    QuestId,
                    DirectionVariant.EntryId,
                    DirectionVariant.JakeText,
                    DirectionVariant.SpokenDirections,
                    GameMinute,
                    DirectionVariant.bDistorted);
            }
            return Journal->AppendJournalNote(
                QuestId,
                DirectionVariant.EntryId,
                DirectionVariant.JakeText,
                DirectionVariant.SpokenDirections,
                GameMinute,
                DirectionVariant.bDistorted);
        }
        return false;
    }
    return false;
}

bool URexaM2MissionCatalog::IsCatalogValid(
    const TArray<FRexaM2MissionDefinition>& Missions)
{
    if (Missions.Num() != 4) return false;
    TSet<FName> QuestIds;
    TSet<FName> VariantIds;
    int32 TurnCount = 0;
    int32 StandingCount = 0;
    for (const FRexaM2MissionDefinition& Mission : Missions)
    {
        const FQuestActivationDefinition& Activation = Mission.Activation;
        if (Activation.QuestId.IsNone() || Mission.AuthoredVariantId.IsNone() ||
            QuestIds.Contains(Activation.QuestId) ||
            VariantIds.Contains(Mission.AuthoredVariantId) ||
            Mission.LocalDirections.Num() < 2 || Mission.AuthoredOutcomeIds.Num() < 2)
            return false;
        QuestIds.Add(Activation.QuestId);
        VariantIds.Add(Mission.AuthoredVariantId);

        TSet<FName> DirectionSpeakers;
        TSet<FName> DirectionEntries;
        for (const FLocalDirectionVariant& DirectionVariant : Mission.LocalDirections)
        {
            if (DirectionVariant.SpeakerKnowledgeId.IsNone() ||
                DirectionVariant.EntryId.IsNone() || DirectionVariant.JakeText.IsEmpty() ||
                DirectionVariant.SpokenDirections.IsEmpty() ||
                DirectionSpeakers.Contains(DirectionVariant.SpeakerKnowledgeId) ||
                DirectionEntries.Contains(DirectionVariant.EntryId)) return false;
            DirectionSpeakers.Add(DirectionVariant.SpeakerKnowledgeId);
            DirectionEntries.Add(DirectionVariant.EntryId);
        }
        TSet<FName> Outcomes;
        for (const FName OutcomeId : Mission.AuthoredOutcomeIds)
        {
            if (OutcomeId.IsNone() || Outcomes.Contains(OutcomeId)) return false;
            Outcomes.Add(OutcomeId);
        }

        if (Mission.Tier == EQuestStructuralTier::Turn)
        {
            if (Mission.StandingType != EStandingMissionType::None ||
                Activation.bRequiresSpokenAgreement) return false;
            ++TurnCount;
        }
        else
        {
            if (Mission.StandingType == EStandingMissionType::None ||
                !Activation.bRequiresSpokenAgreement ||
                Activation.Trigger != EQuestActivationTrigger::Conversation) return false;
            ++StandingCount;
        }
    }
    return TurnCount == 3 && StandingCount == 1;
}
