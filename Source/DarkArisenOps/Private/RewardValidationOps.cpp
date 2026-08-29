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
        Errors.Add(FString::Printf(TEXT("missing reward file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read reward file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing reward contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanRewardSourceForForbiddenLoot(const FString& Root, TArray<FString>& Errors)
{
    const FString ScanRoot = FPaths::Combine(Root, TEXT("Source/DarkArisen/ContentScale"));
    const TArray<FString> Forbidden = {
        TEXT("GenerateRandomLoot"),
        TEXT("RandomLootTable"),
        TEXT("RollLoot"),
        TEXT("TreasureFoundCounterWidget"),
        TEXT("ReturnedArtifactCountWidget"),
        TEXT("RarityColourTier")
    };

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);
    for (const FString& File : Files)
    {
        FString Text;
        if (!ReadText(File, Text))
        {
            Errors.Add(FString::Printf(TEXT("cannot read reward source during loot scan: %s"), *File));
            continue;
        }
        for (const FString& Token : Forbidden)
        {
            if (Text.Contains(Token, ESearchCase::CaseSensitive))
            {
                Errors.Add(FString::Printf(TEXT("forbidden loot-system token %s in %s"), *Token, *File));
            }
        }
    }
}
}

int32 ValidateRewardContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.h"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.cpp"),
        TEXT("Source/DarkArisen/ContentScale/TreasureStateComponent.h"),
        TEXT("Source/DarkArisen/ContentScale/TreasureStateComponent.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredRewardCatalogSpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.h"), {
        TEXT("StateTreasureSlotCount = 9"),
        TEXT("ReturnQuestionArtifactCount = 40"),
        TEXT("BuriedHoardCount = 16"),
        TEXT("ArchipelagoBuriedHoardCount = 12"),
        TEXT("HighmooreBuriedHoardCount = 4"),
        TEXT("ApproxCoinPercent = 15"),
        TEXT("ApproxDocumentPercent = 20"),
        TEXT("AllowsRandomLootTables() { return false; }"),
        TEXT("AllowsReturnedCountUI() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.cpp"), {
        TEXT("Expected one reward/outcome binding for each of 40 grounded dungeon profiles"),
        TEXT("The Conquest Archives, 1651"),
        TEXT("The Labor Ledgers"),
        TEXT("The 1846 Patrol Reports"),
        TEXT("The Bribe Ledgers"),
        TEXT("Vega's Eleven Years"),
        TEXT("The Master Ledger"),
        TEXT("Sterling's Correspondence"),
        TEXT("The Four Thousand Contracts"),
        TEXT("Thorne's Dispatches"),
        TEXT("reward.unique.crystal-katana"),
        TEXT("design-gap.return-question-artifact-identities"),
        TEXT("design-gap.buried-hoard-identities")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/TreasureStateComponent.h"), {
        TEXT("ReturnedArtifactNetworkStrengthDelta = 5"),
        TEXT("IsRecoveryReturnThresholdSatisfied"),
        TEXT("The exact hidden return count intentionally has no public/UI getter")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/TreasureStateComponent.cpp"), {
        TEXT("ETreasureDisposition::Returned"),
        TEXT("ETreasureDisposition::Kept"),
        TEXT("ETreasureDisposition::Sold"),
        TEXT("Returned > 0 && Returned > NonReturned")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AuthoredRewardCatalogSpec.cpp"), {
        TEXT("Exactly nine state treasures exist"),
        TEXT("Conquest Archives are present"),
        TEXT("Sterling correspondence is present"),
        TEXT("Returned-count UI remains prohibited"),
        TEXT("Two returned versus one non-returned restores the hidden return bias"),
        TEXT("Crystal Katana remains the named unique reward")}, Errors);

    ScanRewardSourceForForbiddenLoot(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Reward validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored reward/treasure source validation passed."));
    return 0;
}
}
