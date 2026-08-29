#include "DarkArisenOps.h"

#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing audio/voice file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read audio/voice file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing audio/voice contract: %s"), *Relative, Fragment));
        }
    }
}
}

int32 ValidateAudioVoiceContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/Audio/AudioProductionCatalog.h"),
        TEXT("Source/DarkArisen/Audio/AudioProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Audio/VoiceProductionCatalog.h"),
        TEXT("Source/DarkArisen/Audio/VoiceProductionCatalog.cpp"),
        TEXT("Docs/Voice/CORE_CREW_PRONUNCIATION.md"),
        TEXT("Docs/Voice/CORE_CREW_AUDITION_EVIDENCE.md"),
        TEXT("Source/DarkArisen/Tests/AudioVoiceProductionSpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Audio/AudioProductionCatalog.h"), {
        TEXT("CoreRequirementCount = 12"),
        TEXT("bReactiveToGameplayEvent = false"),
        TEXT("bAssetAuthored = false")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Audio/AudioProductionCatalog.cpp"), {
        TEXT("audio.global.non-reactive-score"),
        TEXT("audio.dungeons.no-ambient-score"),
        TEXT("audio.highmoore.rain-dance"),
        TEXT("audio.highmoore.belos-silence"),
        TEXT("audio.katana.resonance"),
        TEXT("design-gap.highmoore-arrow-music-conflict"),
        TEXT("continues its authored playback unchanged through the arrow")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Audio/VoiceProductionCatalog.h"), {
        TEXT("CoreCrewRoleCount = 5"),
        TEXT("RejectedSynthetic"),
        TEXT("bDirectedPerformanceReviewRequired = true"),
        TEXT("bCurrentSyntheticProviderRejected = false"),
        TEXT("bCommercialRightsApproved = false")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Audio/VoiceProductionCatalog.cpp"), {
        TEXT("voice.crew.mira"),
        TEXT("voice.crew.big-tom"),
        TEXT("voice.crew.ines"),
        TEXT("voice.crew.father-salvio"),
        TEXT("voice.crew.esteban"),
        TEXT("AuditionDisposition = EVoiceAuditionDisposition::RejectedSynthetic"),
        TEXT("bDirectedPerformanceReviewRequired = true"),
        TEXT("bCurrentSyntheticProviderRejected = true"),
        TEXT("design-gap.tier1-performance-casting")}, Errors);

    RequireFragments(Root, TEXT("Docs/Voice/CORE_CREW_AUDITION_EVIDENCE.md"), {
        TEXT("Disposition:** **REJECTED"),
        TEXT("synthetic / emotionally flat"),
        TEXT("cc8c322eaabf41a69db2c05206e40ec1"),
        TEXT("c8846ec4fce6424a81adaa78c4600cd0"),
        TEXT("b26bf00ae737438986876426040e2437"),
        TEXT("974b632e64ad4b59b1c7dc0789ce50a1"),
        TEXT("7befac8f8007447cbaf80733e752f2f6"),
        TEXT("negative evidence")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AudioVoiceProductionSpec.cpp"), {
        TEXT("No production music reacts to gameplay events"),
        TEXT("Exactly five core crew voice roles are locked"),
        TEXT("Generic synthetic audition remains rejected"),
        TEXT("Directed performance review is mandatory"),
        TEXT("Current synthetic provider remains rejected for core cast")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Audio/voice validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Audio/voice production source validation passed; rejected synthetic core auditions remain fail-closed."));
    return 0;
}
}
