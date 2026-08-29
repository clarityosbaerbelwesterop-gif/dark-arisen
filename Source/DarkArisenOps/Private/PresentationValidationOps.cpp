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
        Errors.Add(FString::Printf(TEXT("missing presentation file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read presentation file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing presentation contract: %s"), *Relative, Fragment));
        }
    }
}

void ScanForForbiddenPresentationAuthority(const FString& Root, TArray<FString>& Errors)
{
    const TArray<FString> Roots = {
        FPaths::Combine(Root, TEXT("Source/DarkArisen/Presentation")),
        FPaths::Combine(Root, TEXT("Source/DarkArisen/Animation"))
    };
    const TArray<FString> Forbidden = {
        TEXT("DynamicMusicIntensity"),
        TEXT("AdaptiveMusic"),
        TEXT("EventDrivenStinger"),
        TEXT("SlowMotionArrow"),
        TEXT("CutsceneBossDeath"),
        TEXT("CutsceneIbarra"),
        TEXT("LedgeMagnet"),
        TEXT("CancelIntoDodge"),
        TEXT("CancelIntoBlock")
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
                Errors.Add(FString::Printf(TEXT("cannot read presentation source during prohibition scan: %s"), *File));
                continue;
            }
            for (const FString& Token : Forbidden)
            {
                if (Text.Contains(Token, ESearchCase::CaseSensitive))
                {
                    Errors.Add(FString::Printf(TEXT("forbidden presentation token %s in %s"), *Token, *File));
                }
            }
        }
    }
}
}

int32 ValidatePresentationContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/Presentation/PresentationProductionCatalog.h"),
        TEXT("Source/DarkArisen/Presentation/PresentationProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Animation/AnimationProductionCatalog.h"),
        TEXT("Source/DarkArisen/Animation/AnimationProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Animation/DarkArisenAnimInstance.h"),
        TEXT("Source/DarkArisen/Tests/PresentationProductionCatalogSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/AnimationProductionCatalogSpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Presentation/PresentationProductionCatalog.h"), {
        TEXT("RequiredCutsceneCount = 19"),
        TEXT("ResolvedCutsceneIdentityCount = 14"),
        TEXT("UnresolvedEndgameCutsceneCount = 5"),
        TEXT("RequiredProtectedPlayableCount = 22"),
        TEXT("RequiredInsertCount = 5"),
        TEXT("RequiredSlowPushCount = 6")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Presentation/PresentationProductionCatalog.cpp"), {
        TEXT("cutscene.opening.the-brother"),
        TEXT("cutscene.highmoore.arrow"),
        TEXT("bCameraMustRemainStatic = true"),
        TEXT("bMusicMustContinueUnchanged = true"),
        TEXT("cutscene.endgame.slot-%d"),
        TEXT("playable.boss-death.hold"),
        TEXT("playable.ibarra.garden"),
        TEXT("playable.liberation.aftermath"),
        TEXT("design-gap.p1-summary-count")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Animation/AnimationProductionCatalog.h"), {
        TEXT("NamedBibleRequirementCount = 11"),
        TEXT("RequiredHorseGaitTransitionCount = 6"),
        TEXT("MinimumAttackTellFrames = 8"),
        TEXT("LockedDeflectionWindowFrames = 6")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Animation/AnimationProductionCatalog.cpp"), {
        TEXT("anim.named.bad-catch"),
        TEXT("anim.named.guardian-settling"),
        TEXT("anim.named.vega-stands-again"),
        TEXT("anim.named.bailiffs-leaving"),
        TEXT("anim.named.mount-three-attempts"),
        TEXT("anim.system.wounded-four-layers"),
        TEXT("Katana suppresses wounded weapon-set swap"),
        TEXT("design-gap.animation-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Animation/DarkArisenAnimInstance.h"), {
        TEXT("EPostureVisualState PostureVisualState"),
        TEXT("EWoundLayer WoundLayer"),
        TEXT("bool bUseWoundedAnimationSet"),
        TEXT("bool bKatanaEquipped")}, Errors);

    ScanForForbiddenPresentationAuthority(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Presentation/animation validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Presentation/animation source validation passed."));
    return 0;
}
}
