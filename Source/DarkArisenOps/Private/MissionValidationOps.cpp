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
        FPaths::Combine(Root, TEXT("Source/DarkArisen/ContentScale"))
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
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.h"),
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredQuestCatalogSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/MissionScaleRequirementsSpec.cpp")
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
        TEXT("thread.archipelago.assassin-network"),
        TEXT("thread.highmoore.princess"),
        TEXT("thread.cross.named-dead"),
        TEXT("thread.cross.ethan"),
        TEXT("region.rexa-moran"), TEXT("22"),
        TEXT("region.highmoore"), TEXT("24"),
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
