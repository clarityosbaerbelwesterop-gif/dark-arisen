#include "DarkArisenOps.h"

#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative))) Errors.Add(FString::Printf(TEXT("missing alpha-content file: %s"), *Relative));
}

void RequireFragments(const FString& Root, const FString& Relative, std::initializer_list<const TCHAR*> Fragments, TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read alpha-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments) if (!Text.Contains(Fragment, ESearchCase::CaseSensitive)) Errors.Add(FString::Printf(TEXT("%s missing alpha-content contract: %s"), *Relative, Fragment));
}
}

int32 ValidateAlphaContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;
    for (const FString& Relative : {
        TEXT("Source/DarkArisen/Release/AlphaContentReadiness.h"), TEXT("Source/DarkArisen/Release/AlphaContentReadiness.cpp"),
        TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.h"), TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.cpp"),
        TEXT("Source/DarkArisen/Tests/AlphaContentReadinessSpec.cpp")}) RequireFile(Root, Relative, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Release/AlphaContentReadiness.cpp"), {
        TEXT("alpha-content.named-dungeons"), TEXT("41, 40, 0"),
        TEXT("alpha-content.minor-dungeons"), TEXT("20, 0, 0"),
        TEXT("alpha-content.turns"), TEXT("132, 3, 0"),
        TEXT("alpha-content.standing"), TEXT("147, 1, 0"),
        TEXT("alpha-content.tier1-bosses"), TEXT("9, 9, 0"),
        TEXT("alpha-content.state-treasures"),
        TEXT("alpha-content.world-regions"), TEXT("8, 8, 0"),
        TEXT("alpha-content.highmoore-world-anchors"), TEXT("12, 12, 0"),
        TEXT("alpha-content.cutscenes"), TEXT("19, 14, 0"),
        TEXT("alpha-content.protected-playable"), TEXT("22, 22, 0"),
        TEXT("alpha-content.named-animations"), TEXT("11, 11, 0"),
        TEXT("alpha-content.audio-core"), TEXT("12, 12, 0"),
        TEXT("alpha-content.core-crew-voice"), TEXT("5, 5, 0"),
        TEXT("alpha-content.platform-builds"), TEXT("2, 2, 0"),
        TEXT("requires 16 tracked families"),
        TEXT("must not self-certify Alpha content production")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.h"), {
        TEXT("Win64"), TEXT("LinuxX64"), TEXT("bWindowsStartToCreditsPlaythrough"), TEXT("bLinuxStartToCreditsPlaythrough"),
        TEXT("bContentManifestReconciled"), TEXT("bPerformanceFloor60Fps"), TEXT("bCreditsMusicAndLicencesApproved"), TEXT("bFloAlphaApproval")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AlphaContentReadinessSpec.cpp"), {
        TEXT("Sixteen Alpha content families are reconciled"), TEXT("Source cannot self-certify complete Alpha production"),
        TEXT("Three of 132 Turn identities are grounded"), TEXT("All nine state treasure identities are now source grounded"),
        TEXT("Twelve current Highmoore named anchors are source grounded"), TEXT("Auditions are not counted as shipping voice assets")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Alpha content reconciliation validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors) UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Alpha content reconciliation source validation passed (acceptance remains evidence-gated)."));
    return 0;
}
}
