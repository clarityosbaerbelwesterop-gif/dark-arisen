#include "DarkArisenOps.h"

#include "Misc/Paths.h"

#include <initializer_list>

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing pre-runner production file: %s"), *Relative));
    }
}

bool ReadRequiredText(const FString& Root, const FString& Relative, FString& OutText, TArray<FString>& Errors)
{
    if (!ReadText(FPaths::Combine(Root, Relative), OutText))
    {
        Errors.Add(FString::Printf(TEXT("cannot read pre-runner production file: %s"), *Relative));
        return false;
    }
    return true;
}

void RequireFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    if (!ReadRequiredText(Root, Relative, Text, Errors))
    {
        return;
    }

    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing production contract: %s"), *Relative, Fragment));
        }
    }
}

void ForbidFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    if (!ReadRequiredText(Root, Relative, Text, Errors))
    {
        return;
    }

    for (const TCHAR* Fragment : Fragments)
    {
        if (Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s contains forbidden production route: %s"), *Relative, Fragment));
        }
    }
}
}

int32 ValidateExternalAssetProductionCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/CharacterVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.h"),
        TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.cpp"),
        TEXT("Source/DarkArisen/Production/BossVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/BossVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/BossVisualAuthoritySpec.cpp"),
        TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.h"),
        TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/Tier1BossVisualReadinessSpec.cpp"),
        TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/ShipVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/FaunaVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Production/FloraVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/FloraVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/FloraVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Production/ThreeDAssetReadinessCatalog.h"),
        TEXT("Source/DarkArisen/Production/ThreeDAssetReadinessCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/ThreeDAssetReadinessSpec.cpp"),
        TEXT("Source/DarkArisen/Production/PreMainQuestReadinessCatalog.h"),
        TEXT("Source/DarkArisen/Production/PreMainQuestReadinessCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/PreMainQuestReadinessSpec.cpp"),
        TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md"),
        TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"),
        TEXT("Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md"),
        TEXT("Docs/THREED_ASSET_READINESS_MATRIX.md"),
        TEXT("Docs/PRE_MAIN_QUEST_READINESS.md"),
        TEXT("Docs/MAIN_QUEST_AUTHORITY_GAP.md"),
        TEXT("Docs/NON_AI_SLOP_STANDARD.md")})
    {
        RequireFile(Root, Relative, Errors);
    }

    // Higgsfield is intentionally motion/presentation-only.
    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.h"), {
        TEXT("NamedAnimationBriefCount = 11"),
        TEXT("SystemAnimationBriefCount = 12"),
        TEXT("ResolvedPresentationBriefCount = 14"),
        TEXT("RequiredHiggsfieldBriefCount"),
        TEXT("AllowsHiggsfieldStaticVisualProduction() { return false; }"),
        TEXT("AllowsHiggsfieldCharacterOrWorldLookProduction() { return false; }"),
        TEXT("AllowsHiggsfieldThreeDProductionWithoutVerifiedAction() { return false; }"),
        TEXT("DeliberateTurnStandingIdentityGapCount = 275")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"), {
        TEXT("FAnimationProductionCatalog::BuildNamedBibleRequirements"),
        TEXT("FAnimationProductionCatalog::BuildSystemRequirements"),
        TEXT("FPresentationProductionCatalog::BuildCutscenes"),
        TEXT("external.higgsfield.animation"),
        TEXT("external.higgsfield.presentation"),
        TEXT("motion/presentation-only"),
        TEXT("design-gap.external-assets.final-act-presentation"),
        TEXT("Requires basic plan or higher")}, Errors);

    ForbidFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"), {
        TEXT("external.higgsfield.character"),
        TEXT("external.higgsfield.boss-visual"),
        TEXT("external.higgsfield.dungeon"),
        TEXT("external.higgsfield.region"),
        TEXT("external.higgsfield.world"),
        TEXT("external.higgsfield.ship"),
        TEXT("external.higgsfield.fauna"),
        TEXT("external.higgsfield.flora"),
        TEXT("external.higgsfield.prop")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"), {
        TEXT("Exactly the finite source-derived Higgsfield previs set exists"),
        TEXT("Higgsfield is exactly 23 animation/performance + 14 resolved cinematic briefs"),
        TEXT("No character-look brief is sent to Higgsfield"),
        TEXT("No dungeon look-development brief is sent to Higgsfield"),
        TEXT("No prop-look brief is sent to Higgsfield"),
        TEXT("Higgsfield cannot choose character/world looks")}, Errors);

    // Current visual/readiness authorities remain source packages, not provider queues.
    RequireFragments(Root, TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.h"), {
        TEXT("MajorCharacterBriefCount = 9"),
        TEXT("ProviderReadyCharacterCount = 6"),
        TEXT("ExplicitlyBlockedCharacterCount = 3")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.h"), {
        TEXT("DeepDiveVisualIdentityCount = 21"),
        TEXT("CurrentStoryConflictCount = 2"),
        TEXT("ProviderEligibleVisualIdentityCount = 19")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.cpp"), {
        TEXT("boss-visual.ethan-harlow"),
        TEXT("boss-visual.draven-voss"),
        TEXT("CurrentStoryConflict")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.h"), {
        TEXT("RequiredTier1BossCount = 9"),
        TEXT("ProviderFullLookReadyCount = 0"),
        TEXT("ProviderBlockedFullLookCount = RequiredTier1BossCount")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.h"), {
        TEXT("RequiredBriefCount = 6"),
        TEXT("ProviderReadyBriefCount = 5"),
        TEXT("ProviderBlockedBriefCount = 1")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.h"), {
        TEXT("RequiredBriefCount = LegendaryCreatureBriefCount + HighmooreMvpBriefCount"),
        TEXT("ProviderReadyBriefCount")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/FloraVisualProductionCatalog.h"), {
        TEXT("RequiredBriefCount = 9"),
        TEXT("ProviderReadyBriefCount = 8"),
        TEXT("ProviderBlockedBriefCount = 1")}, Errors);

    // 3D is a separate source-readiness lane.
    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ThreeDAssetReadinessCatalog.h"), {
        TEXT("CurrentTier1BossRecordCount = 9"),
        TEXT("RequiredRecordCount"),
        TEXT("CandidateGeometryReadyCount = 105"),
        TEXT("ReferenceOnlyCount = 14"),
        TEXT("BlockedCount = 21"),
        TEXT("HasVerifiedConnectedProvider3DAction() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Docs/THREED_ASSET_READINESS_MATRIX.md"), {
        TEXT("**Total** | **140** | **105** | **14** | **21**"),
        TEXT("Current Tier-1 bosses — The Nine Who Hold"),
        TEXT("Higgsfield is not the owner of the 140 records above")}, Errors);

    // Finite closure before main-story work.
    RequireFragments(Root, TEXT("Source/DarkArisen/Production/PreMainQuestReadinessCatalog.h"), {
        TEXT("RequiredFamilyCount = 17"),
        TEXT("DungeonIdentityGaps = 21"),
        TEXT("TurnIdentityGaps = 129"),
        TEXT("StandingIdentityGaps = 146"),
        TEXT("DeliberateMissionIdentityGapCount"),
        TEXT("ThreeDRecordCount = 140"),
        TEXT("ThreeDGeometryReady = 105"),
        TEXT("ThreeDReferenceOnly = 14"),
        TEXT("ThreeDBlocked = 21"),
        TEXT("HiggsfieldPrevisBriefCount = AnimationRequirementCount + ResolvedCutsceneCount"),
        TEXT("AllowsMainQuestFabrication() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/PreMainQuestReadinessCatalog.cpp"), {
        TEXT("pre-main.dungeons"),
        TEXT("pre-main.turns"),
        TEXT("pre-main.standing"),
        TEXT("pre-main.tier1-looks"),
        TEXT("pre-main.3d-readiness"),
        TEXT("pre-main.higgsfield-previs"),
        TEXT("main-quest-gap.structure-is-scaffold"),
        TEXT("main-quest-gap.ethan-draven-authority-conflict"),
        TEXT("main-quest-gap.crimson-armada-final-act-source"),
        TEXT("main-quest-gap.final-act-cutscenes")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/PreMainQuestReadinessSpec.cpp"), {
        TEXT("129 Turns remain unauthored rather than generated"),
        TEXT("146 Standing variants remain unauthored rather than generated"),
        TEXT("3D source-readiness register has 140 records"),
        TEXT("Higgsfield is restricted to thirty-seven motion/cinematic briefs"),
        TEXT("Four explicit main-story authority blockers define the stop boundary")}, Errors);

    RequireFragments(Root, TEXT("Docs/PRE_MAIN_QUEST_READINESS.md"), {
        TEXT("Turn + Standing mission identity gap remains exactly **275**"),
        TEXT("**Total**"),
        TEXT("Higgsfield has been reduced to the work it is actually wanted for"),
        TEXT("105 candidate-geometry-ready"),
        TEXT("5 final-act cutscene identities")}, Errors);

    RequireFragments(Root, TEXT("Docs/MAIN_QUEST_AUTHORITY_GAP.md"), {
        TEXT("current structure is a scaffold"),
        TEXT("Ethan / Draven authority conflict"),
        TEXT("missing Crimson Armada final-act authority"),
        TEXT("five final-act cutscenes are unresolved"),
        TEXT("Do not reconstruct it from legacy boss files")}, Errors);

    RequireFragments(Root, TEXT("Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md"), {
        TEXT("exactly **37** current Higgsfield briefs"),
        TEXT("Static character/world/dungeon/boss/ship/fauna/flora/prop looks"),
        TEXT("Deliberately outside Higgsfield provider catalog"),
        TEXT("zero credits consumed")}, Errors);

    RequireFragments(Root, TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"), {
        TEXT("Exactly **37** source-derived Higgsfield briefs"),
        TEXT("not the project's general look-development or 3D-production owner"),
        TEXT("AnimMontage"),
        TEXT("AnimNotify"),
        TEXT("Sequencer"),
        TEXT("Requires basic plan or higher"),
        TEXT("zero credits were consumed")}, Errors);

    RequireFragments(Root, TEXT("Docs/NON_AI_SLOP_STANDARD.md"), {
        TEXT("## 10. Animation anti-slop rule"),
        TEXT("## 11. Cinematic anti-slop rule"),
        TEXT("Provider output is rejected if it introduces canon"),
        TEXT("More adjectives are not specificity")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Pre-runner production validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display,
        TEXT("Pre-runner production source validation passed (Higgsfield=motion/cinematic only; runtime evidence remains separate)."));
    return 0;
}
}
