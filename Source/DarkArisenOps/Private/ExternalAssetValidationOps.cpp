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
        TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"),
        TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md"),
        TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.h"), {
        TEXT("RequirementOnly"),
        TEXT("PrevisGenerated"),
        TEXT("CandidateAssetGenerated"),
        TEXT("ImportedForReview"),
        TEXT("RuntimeAccepted"),
        TEXT("RequiredHiggsfieldBriefCount"),
        TEXT("AllowsProviderToCreateCanon() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsImportedAsset() { return false; }"),
        TEXT("AllowsGeneratedMediaToCountAsRuntimeAccepted() { return false; }"),
        TEXT("AllowsAutomaticProviderPurchaseOrUpgrade() { return false; }")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Production/ExternalAssetProductionCatalog.cpp"), {
        TEXT("FAnimationProductionCatalog::BuildNamedBibleRequirements"),
        TEXT("FAnimationProductionCatalog::BuildSystemRequirements"),
        TEXT("FPresentationProductionCatalog::BuildCutscenes"),
        TEXT("FHighmooreWorldProductionCatalog::BuildNamedAnchors"),
        TEXT("FAuthoredRewardCatalog::BuildStateTreasureSlots"),
        TEXT("FAuthoredRewardCatalog::BuildNamedUniqueRewards"),
        TEXT("EvidenceState = EExternalAssetEvidenceState::RequirementOnly"),
        TEXT("design-gap.external-assets.unreal-import"),
        TEXT("design-gap.external-assets.runtime-acceptance"),
        TEXT("design-gap.external-assets.shipping-rights")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/ExternalAssetProductionSpec.cpp"), {
        TEXT("Exactly fifty-nine source-derived Higgsfield briefs exist"),
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
        TEXT("do not buy/upgrade automatically")}, Errors);

    RequireFragments(Root, TEXT("Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md"), {
        TEXT("Seedance"),
        TEXT("Meshy"),
        TEXT("AnimMontage"),
        TEXT("AnimNotify"),
        TEXT("Sequencer"),
        TEXT("provider-plan blocked"),
        TEXT("zero credits consumed")}, Errors);

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
