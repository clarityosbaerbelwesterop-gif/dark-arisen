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
        TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/CharacterVisualProductionSpec.cpp"),
        TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md"),
        TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"),
        TEXT("Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md")})
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
        TEXT("ProviderReadyCharacterBriefCount = 8"),
        TEXT("UnresolvedPresentationIdentityCount = 5"),
        TEXT("UnauthoredMinorDungeonIdentityCount = 20"),
        TEXT("DeliberateTurnStandingIdentityGapCount = 275"),
        TEXT("RequiredHiggsfieldBriefCount"),
        TEXT("AllowsProviderToCreateCanon() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsImportedAsset() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsRuntimeAccepted() { return false; }"),
        TEXT("AllowsAutomaticProviderPurchaseOrUpgrade() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"), {
        TEXT("FAnimationProductionCatalog::BuildNamedBibleRequirements"),
        TEXT("FAnimationProductionCatalog::BuildSystemRequirements"),
        TEXT("FPresentationProductionCatalog::BuildCutscenes"),
        TEXT("FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles"),
        TEXT("FAuthoredDungeonCatalog::TryGetKnownSite"),
        TEXT("FAuthoredWorldRegionRegistry::BuildAll"),
        TEXT("FHighmooreWorldProductionCatalog::BuildNamedAnchors"),
        TEXT("FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs"),
        TEXT("external.higgsfield.character"),
        TEXT("FAuthoredRewardCatalog::BuildStateTreasureSlots"),
        TEXT("FAuthoredRewardCatalog::BuildNamedUniqueRewards"),
        TEXT("EvidenceState = EExternalAssetEvidenceState::RequirementOnly"),
        TEXT("turn-gap."),
        TEXT("standing-gap."),
        TEXT("minor-slot"),
        TEXT("design-gap.external-assets.provider-3d-path"),
        TEXT("design-gap.external-assets.unreal-import"),
        TEXT("design-gap.external-assets.runtime-acceptance"),
        TEXT("design-gap.external-assets.shipping-rights")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.h"), {
        TEXT("MajorCharacterBriefCount = 9"),
        TEXT("ProviderReadyCharacterCount = 8"),
        TEXT("ExplicitlyBlockedCharacterCount = 1"),
        TEXT("bProviderReferenceReady"),
        TEXT("bApprovedReferenceExists")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/CharacterVisualProductionCatalog.cpp"), {
        TEXT("character.jake-harlow"),
        TEXT("character.mira"),
        TEXT("character.big-tom"),
        TEXT("character.ines"),
        TEXT("character.father-salvio"),
        TEXT("character.esteban"),
        TEXT("character.elowen-arion"),
        TEXT("character.ethan-harlow"),
        TEXT("character.draven-voss"),
        TEXT("design-gap.character-visual.elowen-physical-sheet")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/CharacterVisualProductionSpec.cpp"), {
        TEXT("Nine major-character source briefs exist"),
        TEXT("Eight characters are source-complete enough for bounded reference generation"),
        TEXT("Elowen remains blocked rather than receiving an invented canonical appearance")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"), {
        TEXT("Forty grounded named dungeons have source-backed visual briefs"),
        TEXT("Eight world regions have source-backed visual briefs"),
        TEXT("Eight source-ready major characters have bounded visual-reference briefs"),
        TEXT("Elowen remains provider-blocked until physical visual authority exists"),
        TEXT("No unauthored Turn slot is sent to the provider"),
        TEXT("No unauthored Standing slot is sent to the provider"),
        TEXT("No unauthored minor-dungeon slot is sent to the provider"),
        TEXT("No provider job id is fabricated"),
        TEXT("No Unreal asset path is fabricated"),
        TEXT("Provider cost approval is not self-granted"),
        TEXT("Shipping rights are not self-granted"),
        TEXT("Generated media never self-counts as imported UE content"),
        TEXT("External asset pipeline cannot buy or upgrade providers automatically")}, Errors);

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
        TEXT("115"),
        TEXT("40 grounded named dungeons"),
        TEXT("8 world regions"),
        TEXT("12 Highmoore named anchors"),
        TEXT("8 source-ready major-character reference briefs"),
        TEXT("129 unauthored Turn identities"),
        TEXT("146 unauthored Standing variants"),
        TEXT("Total deliberate Turn/Standing identity gap remains **275**")}, Errors);

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
