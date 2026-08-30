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
        Errors.Add(FString::Printf(TEXT("missing external-asset production file: %s"), *Relative));
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
        Errors.Add(FString::Printf(TEXT("cannot read external-asset production file: %s"), *Relative));
        return;
    }

    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing external-asset contract: %s"), *Relative, Fragment));
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
        TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.h"),
        TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.cpp"),
        TEXT("Source/DarkArisen/Production/BossVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/BossVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.h"),
        TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.cpp"),
        TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.h"),
        TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/CharacterVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/BossVisualAuthoritySpec.cpp"),
        TEXT("Source/DarkArisen/Tests/Tier1BossVisualReadinessSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/ShipVisualProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/FaunaVisualProductionSpec.cpp"),
        TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md"),
        TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"),
        TEXT("Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md"),
        TEXT("Docs/NON_AI_SLOP_STANDARD.md")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.h"), {
        TEXT("RequirementOnly"),
        TEXT("PrevisGenerated"),
        TEXT("CandidateAssetGenerated"),
        TEXT("ImportedForReview"),
        TEXT("RuntimeAccepted"),
        TEXT("GroundedDungeonBriefCount = 40"),
        TEXT("WorldRegionBriefCount = 8"),
        TEXT("ProviderReadyCharacterBriefCount = 6"),
        TEXT("ProviderReadyBossVisualBriefCount = 19"),
        TEXT("ProviderReadyShipVisualBriefCount = 5"),
        TEXT("ProviderReadyFaunaVisualBriefCount = 19"),
        TEXT("UnresolvedPresentationIdentityCount = 5"),
        TEXT("UnauthoredMinorDungeonIdentityCount = 20"),
        TEXT("DeliberateTurnStandingIdentityGapCount = 275"),
        TEXT("RequiredHiggsfieldBriefCount"),
        TEXT("AllowsProviderToCreateCanon() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsImportedAsset() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsRuntimeAccepted() { return false; }"),
        TEXT("AllowsAutomaticProviderPurchaseOrUpgrade() { return false; }"),
        TEXT("AllowsGenericFillerPrompt() { return false; }"),
        TEXT("AllowsProviderToResolveDesignGap() { return false; }"),
        TEXT("AllowsUngroundedDecorativeCompletion() { return false; }"),
        TEXT("AllowsUnreviewedProviderDefaultToBecomeIdentity() { return false; }"),
        TEXT("RequiresSourceSpecificity() { return true; }"),
        TEXT("RequiresFunctionalLogicReview() { return true; }"),
        TEXT("RequiresRestraintAndRepetitionReview() { return true; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"), {
        TEXT("FAnimationProductionCatalog::BuildNamedBibleRequirements"),
        TEXT("FAnimationProductionCatalog::BuildSystemRequirements"),
        TEXT("FPresentationProductionCatalog::BuildCutscenes"),
        TEXT("FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles"),
        TEXT("FAuthoredDungeonCatalog::TryGetKnownSite"),
        TEXT("FAuthoredWorldRegionRegistry::BuildAll"),
        TEXT("FHighmooreWorldProductionCatalog::BuildNamedAnchors"),
        TEXT("FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs"),
        TEXT("FBossVisualProductionCatalog::BuildDeepDiveBossBriefs"),
        TEXT("FShipVisualProductionCatalog::BuildLaLiberacionBriefs"),
        TEXT("FFaunaVisualProductionCatalog::BuildAllBriefs"),
        TEXT("external.higgsfield.character"),
        TEXT("external.higgsfield.boss-visual"),
        TEXT("external.higgsfield.ship"),
        TEXT("external.higgsfield.fauna"),
        TEXT("fauna.legendary.final-wolf"),
        TEXT("fauna.highmoore.grouse"),
        TEXT("fauna.highmoore.hare"),
        TEXT("fauna.highmoore.fox"),
        TEXT("fauna.highmoore.fell-wolf"),
        TEXT("FAuthoredRewardCatalog::BuildStateTreasureSlots"),
        TEXT("FAuthoredRewardCatalog::BuildNamedUniqueRewards"),
        TEXT("EvidenceState = EExternalAssetEvidenceState::RequirementOnly"),
        TEXT("turn-gap."),
        TEXT("standing-gap."),
        TEXT("minor-slot"),
        TEXT("design-gap.external-assets.boss-final-act-authority"),
        TEXT("design-gap.external-assets.fauna-variable-identity"),
        TEXT("design-gap.external-assets.provider-3d-path"),
        TEXT("design-gap.external-assets.unreal-import"),
        TEXT("design-gap.external-assets.runtime-acceptance"),
        TEXT("design-gap.external-assets.shipping-rights")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.h"), {
        TEXT("MajorCharacterBriefCount = 9"),
        TEXT("ProviderReadyCharacterCount = 6"),
        TEXT("ExplicitlyBlockedCharacterCount = 3")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.cpp"), {
        TEXT("character.jake-harlow"),
        TEXT("character.elowen-arion"),
        TEXT("character.ethan-harlow"),
        TEXT("character.draven-voss"),
        TEXT("design-gap.character-visual.phase11-ethan-draven-rewrite")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/BossVisualAuthorityPolicy.cpp"), {
        TEXT("boss-visual.ethan-harlow"),
        TEXT("boss-visual.draven-voss"),
        TEXT("CurrentStoryConflict"),
        TEXT("ProviderEligibleVisualIdentityCount != 19")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/BossVisualProductionCatalog.h"), {
        TEXT("DeepDiveBossBriefCount = 21"),
        TEXT("ProviderReadyBossBriefCount = 19"),
        TEXT("AuthorityBlockedBossBriefCount = 2")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.h"), {
        TEXT("RequiredTier1BossCount = 9"),
        TEXT("ProviderFullLookReadyCount = 0"),
        TEXT("ProviderBlockedFullLookCount = RequiredTier1BossCount")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/Tier1BossVisualReadinessCatalog.cpp"), {
        TEXT("boss.herrera"),
        TEXT("boss.reyes"),
        TEXT("boss.cruz"),
        TEXT("boss.de_silva"),
        TEXT("boss.vega"),
        TEXT("boss.blackwood"),
        TEXT("boss.sterling"),
        TEXT("boss.ashcroft"),
        TEXT("boss.thorne"),
        TEXT("BuildDesignGaps().Num() != ProviderBlockedFullLookCount")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.h"), {
        TEXT("RequiredBriefCount = 6"),
        TEXT("ProviderReadyBriefCount = 5"),
        TEXT("ProviderBlockedBriefCount = 1")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ShipVisualProductionCatalog.cpp"), {
        TEXT("ship-visual.la-liberacion.exterior"),
        TEXT("ship-visual.la-liberacion.weather-deck"),
        TEXT("ship-visual.la-liberacion.captains-cabin"),
        TEXT("DESIGN-GAP"),
        TEXT("forbids quest markers")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.h"), {
        TEXT("LegendaryCreatureBriefCount = 19"),
        TEXT("HighmooreMvpBriefCount = 5"),
        TEXT("ProviderReadyLegendaryCount = 18"),
        TEXT("ProviderReadyHighmooreMvpCount = 1"),
        TEXT("ProviderReadyBriefCount")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/FaunaVisualProductionCatalog.cpp"), {
        TEXT("fauna.legendary.patriarch"),
        TEXT("fauna.legendary.final-wolf"),
        TEXT("fauna.legendary.deep-one"),
        TEXT("fauna.highmoore.red-deer"),
        TEXT("PROVIDER-BLOCKED"),
        TEXT("Do not render a complete creature"),
        TEXT("design-gap.fauna.phase4-scale-not-authored-assets")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/CharacterVisualProductionSpec.cpp"), {
        TEXT("Six characters are current-authority ready for bounded reference generation"),
        TEXT("Exactly three major characters remain explicitly blocked")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/Tier1BossVisualReadinessSpec.cpp"), {
        TEXT("Exactly the Nine Who Hold are tracked"),
        TEXT("No current Tier-1 full look is fabricated"),
        TEXT("Nine explicit current-boss visual design gaps remain")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/FaunaVisualProductionSpec.cpp"), {
        TEXT("Nineteen individually grounded legendary fauna identities are indexed"),
        TEXT("Five Highmoore MVP fauna identities are indexed"),
        TEXT("Final Wolf cannot be frozen into one provider-generated appearance"),
        TEXT("Deep One full body stays deliberately undefined")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"), {
        TEXT("Forty grounded named dungeons have source-backed visual briefs"),
        TEXT("Six current-authority major characters have bounded visual-reference briefs"),
        TEXT("Nineteen non-conflicted deep-dive boss visuals have bounded visual-reference briefs"),
        TEXT("Five source-ready La Liberacion deck/interior visuals have bounded reference briefs"),
        TEXT("Nineteen individually grounded fauna visuals have bounded reference briefs"),
        TEXT("Player-history-dependent Final Wolf stays provider-blocked"),
        TEXT("No provider job id is fabricated"),
        TEXT("No Unreal asset path is fabricated"),
        TEXT("External asset pipeline cannot buy or upgrade providers automatically"),
        TEXT("Generic filler prompts are forbidden"),
        TEXT("Providers cannot resolve design gaps"),
        TEXT("Ungrounded decorative completion is forbidden"),
        TEXT("Unreviewed provider defaults cannot become identity"),
        TEXT("Every provider brief requires source specificity"),
        TEXT("Every important candidate requires functional-logic review"),
        TEXT("Every important candidate requires restraint and repetition review")}, Errors);

    RequireFragments(Root, TEXT("Docs/NON_AI_SLOP_STANDARD.md"), {
        TEXT("## 2. The specificity test"),
        TEXT("## 3. Functional world rule"),
        TEXT("## 4. Restraint and negative space"),
        TEXT("## 5. Material honesty"),
        TEXT("## 6. Silhouette before detail"),
        TEXT("## 9. Level and dungeon anti-slop rule"),
        TEXT("## 10. Animation anti-slop rule"),
        TEXT("## 11. Cinematic anti-slop rule"),
        TEXT("## 12. Audio anti-slop rule"),
        TEXT("## 13. External-AI prompt rule"),
        TEXT("## 14. Review passes"),
        TEXT("## 15. Automatic rejection list"),
        TEXT("Provider output is rejected if it introduces canon"),
        TEXT("More adjectives are not specificity")}, Errors);

    RequireFragments(Root, TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md"), {
        TEXT("RequirementOnly"),
        TEXT("PrevisGenerated"),
        TEXT("CandidateAssetGenerated"),
        TEXT("ImportedForReview"),
        TEXT("RuntimeAccepted"),
        TEXT("Requires basic plan or higher"),
        TEXT("zero credits were consumed"),
        TEXT("129 missing Turn identities"),
        TEXT("146 missing Standing identities")}, Errors);

    RequireFragments(Root, TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"), {
        TEXT("Seedance"),
        TEXT("AnimMontage"),
        TEXT("AnimNotify"),
        TEXT("Sequencer"),
        TEXT("forty currently grounded named dungeon"),
        TEXT("twenty unauthored minor-dungeon identities"),
        TEXT("Requires basic plan or higher"),
        TEXT("zero credits consumed"),
        TEXT("3D-model search did not return a usable connected 3D generation model/action")}, Errors);

    RequireFragments(Root, TEXT("Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md"), {
        TEXT("156"),
        TEXT("40 named dungeons"),
        TEXT("6 current-authority character references"),
        TEXT("Current Tier-1 bosses — Nine Who Hold"),
        TEXT("0 full-look ready / 9 tracked gaps"),
        TEXT("0/9 full looks are provider-ready"),
        TEXT("19 non-conflicted deep-dive boss/creature references"),
        TEXT("5 La Liberación deck/interior references"),
        TEXT("19 fauna references"),
        TEXT("129 unauthored Turn identities"),
        TEXT("146 unauthored Standing variants"),
        TEXT("Turn/Standing identity gap remains **275**")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("External asset production validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("External asset production source validation passed (generation/import/runtime evidence remains separate)."));
    return 0;
}
}