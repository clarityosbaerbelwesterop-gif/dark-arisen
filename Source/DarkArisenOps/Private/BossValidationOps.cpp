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
        Errors.Add(FString::Printf(TEXT("missing boss-content file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read boss-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing boss contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanBossSourceForForbiddenSpectacle(const FString& Root, TArray<FString>& Errors)
{
    const FString ScanRoot = FPaths::Combine(Root, TEXT("Source/DarkArisen/Bosses"));
    const TArray<FString> Forbidden = {
        TEXT("SlowMotionFinisher"),
        TEXT("TriumphalMusic"),
        TEXT("VictoryStinger"),
        TEXT("AutoExecute"),
        TEXT("ForcedKill"),
        TEXT("BossMinimap"),
        TEXT("GenerateBoss")
    };

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.h"), true, false, false);
    IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*.cpp"), true, false, false);
    for (const FString& File : Files)
    {
        FString Text;
        if (!ReadText(File, Text))
        {
            Errors.Add(FString::Printf(TEXT("cannot read boss source during prohibition scan: %s"), *File));
            continue;
        }
        for (const FString& Token : Forbidden)
        {
            if (Text.Contains(Token, ESearchCase::CaseSensitive))
            {
                Errors.Add(FString::Printf(TEXT("forbidden boss spectacle/generation token %s in %s"), *Token, *File));
            }
        }
    }
}
}

int32 ValidateBossContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    const TArray<FString> RequiredFiles = {
        TEXT("Docs/M7_TIER1_BOSS_REGISTER.md"),
        TEXT("Source/DarkArisen/Bosses/IsabelCruzCharacter.h"),
        TEXT("Source/DarkArisen/Bosses/IsabelCruzCharacter.cpp"),
        TEXT("Source/DarkArisen/Bosses/Tier1CommanderCatalog.h"),
        TEXT("Source/DarkArisen/Bosses/Tier1CommanderCatalog.cpp"),
        TEXT("Source/DarkArisen/Bosses/Tier1CommanderEncounterComponent.h"),
        TEXT("Source/DarkArisen/Bosses/Tier1CommanderEncounterComponent.cpp"),
        TEXT("Source/DarkArisen/Tests/Tier1CommanderSystemsSpec.cpp")
    };
    for (const FString& Relative : RequiredFiles)
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Bosses/Tier1CommanderCatalog.h"), {
        TEXT("RequiredCommanderCount = 9"),
        TEXT("HumanDuel"),
        TEXT("NegotiationPursuit"),
        TEXT("VerticalPursuit"),
        TEXT("NavalEngagement"),
        TEXT("SpareRefusedLeaveOnly")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Bosses/Tier1CommanderCatalog.cpp"), {
        TEXT("boss.herrera"), TEXT("0.70f, 0.35f"),
        TEXT("boss.reyes"), TEXT("0.60f, 0.25f"),
        TEXT("boss.cruz"), TEXT("0.65f, 0.30f"),
        TEXT("boss.de_silva"), TEXT("NegotiationPursuit"),
        TEXT("boss.vega"), TEXT("SpareRefusedLeaveOnly"),
        TEXT("boss.blackwood"), TEXT("VerticalPursuit"),
        TEXT("boss.sterling"), TEXT("NavalEngagement"),
        TEXT("boss.ashcroft"),
        TEXT("boss.thorne"), TEXT("bGarrisonWeakensOverTime = true")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Bosses/Tier1CommanderEncounterComponent.cpp"), {
        TEXT("UpdateHealthFraction"),
        TEXT("AdvanceScriptedStage"),
        TEXT("bSpareRefusedObserved = true"),
        TEXT("ResolveInternal(ETier1CommanderResolution::Avoided"),
        TEXT("Avoidance may resolve before the direct encounter begins")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Bosses/IsabelCruzCharacter.h"), {
        TEXT("ListPhaseHealthFraction = 0.65f"),
        TEXT("EighteenFortySixPhaseHealthFraction = 0.30f"),
        TEXT("TrySpare"),
        TEXT("ResolveAvoided")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/Tier1CommanderSystemsSpec.cpp"), {
        TEXT("Exactly nine Tier-1 commanders exist"),
        TEXT("Vega refuses a clean spare"),
        TEXT("scripted stage cannot skip ahead"),
        TEXT("Sterling alliance path can resolve before combat")}, Errors);

    ScanBossSourceForForbiddenSpectacle(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Tier-1 boss validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Tier-1 boss source validation passed."));
    return 0;
}
}
