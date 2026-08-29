#include "DarkArisenOps.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing mission-content file: %s"), *Relative));
    }
}

void RequireFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read mission-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing mission contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanMissionSourceForForbiddenGeneration(const FString& Root, TArray<FString>& Errors)
{
    const TArray<FString> Roots = {
        FPaths::Combine(Root, TEXT("Source/DarkArisen/Missions")),
        FPaths::Combine(Root, TEXT("Source/DarkArisen/ContentScale")),
        FPaths::Combine(Root, TEXT("Source/DarkArisen/Highmoore")),
        FPaths::Combine(Root, TEXT("Source/DarkArisen/ColonialWar"))
    };
    const TArray<FString> Forbidden = {
        TEXT("GenerateRadiantQuest"),
        TEXT("GenerateRandomMission"),
        TEXT("RandomMission"),
        TEXT("FMath::Rand"),
        TEXT("FRandomStream"),
        TEXT("KillTen"),
        TEXT("ClearTheCamp"),
        TEXT("TakeAll")
    };

    for (const FString& ScanRoot : Roots)
    {
        TArray<FString> Files;
        IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
        IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);
        for (const FString& File : Files)
        {
            FString Text;
            if (!ReadText(File, Text))
            {
                Errors.Add(FString::Printf(TEXT("cannot read mission source during generation scan: %s"), *File));
                continue;
            }
            for (const FString& Token : Forbidden)
            {
                if (Text.Contains(Token, ESearchCase::CaseSensitive))
                {
                    Errors.Add(FString::Printf(TEXT("forbidden mission-generation token %s in %s"), *Token, *File));
                }
            }
        }
    }
}
}

int32 ValidateMissionContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    const TArray<FString> RequiredFiles = {
        TEXT("Source/DarkArisen/Missions/AuthoredQuestCatalog.h"),
        TEXT("Source/DarkArisen/Missions/AuthoredQuestCatalog.cpp"),
        TEXT("Source/DarkArisen/Missions/MissionScaleRequirements.h"),
        TEXT("Source/DarkArisen/Missions/MissionScaleRequirements.cpp"),
        TEXT("Source/DarkArisen/Missions/QuestCoverageRegister.h"),
        TEXT("Source/DarkArisen/Missions/QuestCoverageRegister.cpp"),
        TEXT("Source/DarkArisen/Missions/CrewPersonalThreadsComponent.h"),
        TEXT("Source/DarkArisen/Missions/CrewPersonalThreadsComponent.cpp"),
        TEXT("Source/DarkArisen/Missions/AssassinNetworkThreadComponent.h"),
        TEXT("Source/DarkArisen/Missions/AssassinNetworkThreadComponent.cpp"),
        TEXT("Source/DarkArisen/Missions/ArchipelagoThreadsComponent.h"),
        TEXT("Source/DarkArisen/Missions/ArchipelagoThreadsComponent.cpp"),
        TEXT("Source/DarkArisen/Missions/MemoryThreadsSubsystem.h"),
        TEXT("Source/DarkArisen/Missions/MemoryThreadsSubsystem.cpp"),
        TEXT("Source/DarkArisen/Missions/ThreadRuntimeCoverage.h"),
        TEXT("Source/DarkArisen/Missions/ThreadRuntimeCoverage.cpp"),
        TEXT("Source/DarkArisen/ColonialWar/LiberationAllianceSubsystem.h"),
        TEXT("Source/DarkArisen/ColonialWar/LiberationAllianceSubsystem.cpp"),
        TEXT("Source/DarkArisen/Highmoore/LightElvesThreadComponent.h"),
        TEXT("Source/DarkArisen/Highmoore/LightElvesThreadComponent.cpp"),
        TEXT("Source/DarkArisen/Highmoore/HighmooreReconstructionComponent.h"),
        TEXT("Source/DarkArisen/Highmoore/HighmooreReconstructionComponent.cpp"),
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.h"),
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredQuestCatalogSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/MissionScaleRequirementsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/QuestCoverageRegisterSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/CrewPersonalThreadsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/AssassinNetworkThreadSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/LiberationAllianceSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/ArchipelagoThreadsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/HighmooreThreadContractsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/MemoryThreadsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/ThreadRuntimeCoverageSpec.cpp"),
        TEXT("Docs/QUEST_CONTENT_GAPS.md"),
        TEXT("Docs/THREAD_RUNTIME_COVERAGE.md")
    };
    for (const FString& Relative : RequiredFiles)
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/AuthoredQuestCatalog.h"), {
        TEXT("RequiredThreads = 17"),
        TEXT("RequiredTurns = 132"),
        TEXT("RequiredStandingVariants = 147"),
        TEXT("IndividuallyAuthoredTurnIdentitiesInSource = 3"),
        TEXT("IndividuallyAuthoredStandingIdentitiesInSource = 1")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/AuthoredQuestCatalog.cpp"), {
        TEXT("thread.crew.ines-esperanza"),
        TEXT("thread.crew.miras-coast"),
        TEXT("thread.crew.big-toms-service"),
        TEXT("thread.crew.father-salvios-parish"),
        TEXT("thread.crew.estebans-last-chart"),
        TEXT("thread.archipelago.assassin-network"),
        TEXT("thread.archipelago.liberation-connections"),
        TEXT("thread.archipelago.kesslers-investigation"),
        TEXT("thread.archipelago.ledger-trilogy"),
        TEXT("thread.archipelago.schreiber-documentation"),
        TEXT("thread.archipelago.captain-vasquez"),
        TEXT("thread.archipelago.old-fortresses"),
        TEXT("thread.highmoore.princess"),
        TEXT("thread.highmoore.light-elves"),
        TEXT("thread.highmoore.reconstruction"),
        TEXT("thread.cross.named-dead"),
        TEXT("thread.cross.ethan"),
        TEXT("Rexa.Standing.Salvage.SanTelmoBell"),
        TEXT("design-gap.turn-identities"),
        TEXT("design-gap.standing-identities")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/MissionScaleRequirements.cpp"), {
        TEXT("region.rexa-moran"), TEXT("22, 3"),
        TEXT("region.fjordlund"), TEXT("18, 0"),
        TEXT("region.ashenmoor"), TEXT("16, 0"),
        TEXT("region.quiet-coast"), TEXT("14, 0"),
        TEXT("region.pale-isle-silvera"), TEXT("15, 0"),
        TEXT("region.at-sea"), TEXT("19, 0"),
        TEXT("region.highmoore"), TEXT("24, 0"),
        TEXT("region.06"), TEXT("4, 0"),
        TEXT("EM7StandingMissionType::Escort, 21"),
        TEXT("EM7StandingMissionType::ConvoyRaid, 18"),
        TEXT("EM7StandingMissionType::Recovery, 24"),
        TEXT("EM7StandingMissionType::Champion, 12"),
        TEXT("EM7StandingMissionType::Transport, 16"),
        TEXT("EM7StandingMissionType::PrivateerCommission, 14"),
        TEXT("EM7StandingMissionType::RoadWork, 15"),
        TEXT("EM7StandingMissionType::Hunt, 13"),
        TEXT("EM7StandingMissionType::Salvage, 14, 1"),
        TEXT("design-gap.standing-region-allocation"),
        TEXT("design-gap.pay-curve-middle")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/QuestCoverageRegister.h"), {
        TEXT("MissingTurns = RequiredTurns - AuthoredTurns"),
        TEXT("MissingStanding = RequiredStanding - AuthoredStanding"),
        TEXT("TotalMissingQuestIdentities = MissingTurns + MissingStanding"),
        TEXT("ReservedUnauthored")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/QuestCoverageRegister.cpp"), {
        TEXT("turn-gap.%s"),
        TEXT("standing-gap.%s"),
        TEXT("Rexa.Turn.EmptyHammock"),
        TEXT("Rexa.Turn.ThreeCutsInStone"),
        TEXT("Rexa.Turn.SaltLedger"),
        TEXT("Rexa.Standing.Salvage.SanTelmoBell"),
        TEXT("Reserved quest slot %s must not carry an invented authored identity")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/CrewPersonalThreadsComponent.cpp"), {
        TEXT("CurrentChapter < 5"),
        TEXT("MeetingCount < 3"),
        TEXT("bFjordlundControlAlreadyResolved"),
        TEXT("bRecogniserSawTomAndLeft"),
        TEXT("AwaitingAuthoredDetail"),
        TEXT("CompletionMarksPerCrewThread"),
        TEXT("AwardMarks")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/CrewPersonalThreadsComponent.h"), {
        TEXT("AllowsGeneratedCrewQuestStages() const { return false; }"),
        TEXT("AllowsSystemicRomance() const { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/AssassinNetworkThreadComponent.cpp"), {
        TEXT("thread.archipelago.assassin-network"),
        TEXT("HasReachedStageAtLeastOnce"),
        TEXT("RecordVidalConfessionTaken"),
        TEXT("ResolveCofradiaByWithdrawal"),
        TEXT("ResolveRegulatorsBySettlement"),
        TEXT("bUnactionedRegulatorContractRecovered")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ColonialWar/LiberationAllianceSubsystem.cpp"), {
        TEXT("thread.archipelago.liberation-connections"),
        TEXT("alliance.connection.cultural-recovery"),
        TEXT("alliance.connection.cultural-mountain"),
        TEXT("alliance.connection.mountain-patient"),
        TEXT("alliance.connection.patient-deed"),
        TEXT("alliance.connection.recovery-deed"),
        TEXT("FullVisionGap"),
        TEXT("case 5: return 1.60f")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/ArchipelagoThreadsComponent.cpp"), {
        TEXT("thread.archipelago.kesslers-investigation"),
        TEXT("thread.archipelago.ledger-trilogy"),
        TEXT("thread.archipelago.schreiber-documentation"),
        TEXT("thread.archipelago.captain-vasquez"),
        TEXT("thread.archipelago.old-fortresses"),
        TEXT("bCastilloDoradoFell"),
        TEXT("bDeSilvaDied"),
        TEXT("RecoveredLedgers.Num() == 4"),
        TEXT("BoatsDocumentedAndLeft"),
        TEXT("AwaitingAuthoredDetail"),
        TEXT("ReclaimedOldFortresses.Num() == 4")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/ArchipelagoThreadsComponent.h"), {
        TEXT("AllowsGeneratedThreadStages() const { return false; }"),
        TEXT("AllowsQuestExpiryTimerUI() const { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/LightElvesThreadComponent.cpp"), {
        TEXT("thread.highmoore.light-elves"),
        TEXT("HasElevenDaySpanElapsed"),
        TEXT("LordRefusals.Num() == 4"),
        TEXT("TakingAccounts.Num() != 4"),
        TEXT("ResolvedMoorSites.Num() == 3"),
        TEXT("HasReachedElevenDays()"),
        TEXT("EEllisLetterResponse::None")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/LightElvesThreadComponent.h"), {
        TEXT("ElevenDaysInWorldMinutes"),
        TEXT("AllowsFirstStoppedShaftCutscene() const { return false; }"),
        TEXT("AllowsBossEndingCutscene() const { return false; }"),
        TEXT("AllowsWestwardQuestReminder() const { return false; }"),
        TEXT("AllowsFastTravelForCampaign() const { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/HighmooreReconstructionComponent.cpp"), {
        TEXT("thread.highmoore.reconstruction"),
        TEXT("PermanentlyUnavailableAfterStair"),
        TEXT("TurnedWest"),
        TEXT("ConnectArchipelagoSupplyLine"),
        TEXT("BuildFordBridge"),
        TEXT("bCadwallMasonsSecured")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/HighmooreReconstructionComponent.h"), {
        TEXT("AllowsRedemptionMeter() const { return false; }"),
        TEXT("AllowsForgivenessReward() const { return false; }"),
        TEXT("MakesJakeLordOfHighmoore() const { return false; }"),
        TEXT("RewardsGalleryGap() const { return false; }"),
        TEXT("RewardsRabbitHutches() const { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/MemoryThreadsSubsystem.cpp"), {
        TEXT("thread.cross.named-dead"),
        TEXT("thread.cross.ethan"),
        TEXT("ResolveBjornStatus"),
        TEXT("RecordDreamEthanVoiceHeard"),
        TEXT("CurrentChapter < 9")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/MemoryThreadsSubsystem.h"), {
        TEXT("NamedDeadThreadHasEnding() const { return false; }"),
        TEXT("EthanHasClosureMechanic() const { return false; }"),
        TEXT("EthanHasSecretFinalLetter() const { return false; }"),
        TEXT("AllowsClearEthanFlashbackFace() const { return false; }"),
        TEXT("AwardsMemorialGameplayReward() const { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/ThreadRuntimeCoverage.h"), {
        TEXT("RequiredThreadCount = 17"),
        TEXT("NativeContractWithAuthorshipGaps")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Missions/ThreadRuntimeCoverage.cpp"), {
        TEXT("design-gap.thread.salvio-parish-stage-chain"),
        TEXT("design-gap.thread.esteban-last-chart-destination"),
        TEXT("design-gap.thread.alliance-five-full-vision-connections"),
        TEXT("design-gap.thread.vasquez-six-hour-stage-chain"),
        TEXT("design-gap.thread.reconstruction-turned-west-diplomatic-content")}, Errors);

    RequireFragments(Root, TEXT("Docs/THREAD_RUNTIME_COVERAGE.md"), {
        TEXT("Every identity now has an explicit native C++ owner"),
        TEXT("Seven stable gaps remain across five Threads"),
        TEXT("275 individual Turn/Standing identities therefore remain unwritten by design")}, Errors);

    RequireFragments(Root, TEXT("Docs/QUEST_CONTENT_GAPS.md"), {
        TEXT("Missing Turn identities — 129"),
        TEXT("Missing Standing identities — 146"),
        TEXT("275 = 129 Turns + 146 Standing variants"),
        TEXT("Forbidden:")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.h"), {
        TEXT("TArray<FStandingMissionVariantDefinition> AuthoredVariants"),
        TEXT("TSet<FName> CompletedVariantIds"),
        TEXT("FindNextAvailableVariant"),
        TEXT("IsPoolExhausted")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.cpp"), {
        TEXT("SameRegionTypePayMultipliers.Num() != 6"),
        TEXT("CompletedVariantIds.Contains"),
        TEXT("Standing gain never degrades")}, Errors);

    ScanMissionSourceForForbiddenGeneration(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Authored mission validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored mission source validation passed."));
    return 0;
}
}
