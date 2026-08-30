// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/ExternalAssetProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenExternalAssetProductionSpec,
    "DarkArisen.Production.ExternalAssetEvidenceBoundary",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenExternalAssetProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("External asset production catalog validates structurally"),
        FExternalAssetProductionCatalog::Validate(Errors));
    TestEqual(TEXT("External asset catalog has no structural validation errors"), Errors.Num(), 0);

    const TArray<FExternalAssetProductionBrief> Briefs =
        FExternalAssetProductionCatalog::BuildHiggsfieldBriefs();
    TestEqual(TEXT("Exactly the finite source-derived Higgsfield previs set exists"),
        Briefs.Num(), FExternalAssetProductionCatalog::RequiredHiggsfieldBriefCount);
    TestEqual(TEXT("Higgsfield is exactly 23 animation/performance + 14 resolved cinematic briefs"),
        Briefs.Num(), 37);

    int32 MotionOrPerformance = 0;
    int32 Cinematic = 0;

    for (const FExternalAssetProductionBrief& Brief : Briefs)
    {
        TestEqual(TEXT("Current external brief remains requirement-only"),
            Brief.EvidenceState, EExternalAssetEvidenceState::RequirementOnly);
        TestTrue(TEXT("No provider job id is fabricated"), Brief.ProviderJobId.IsEmpty());
        TestTrue(TEXT("No provider media reference is fabricated"), Brief.MediaReference.IsEmpty());
        TestTrue(TEXT("No Unreal asset path is fabricated"), Brief.UnrealAssetPath.IsEmpty());
        TestFalse(TEXT("Provider cost approval is not self-granted"), Brief.bProviderCostApproved);
        TestFalse(TEXT("Shipping rights are not self-granted"), Brief.bShippingRightsCleared);

        const FString Id = Brief.StableId.ToString();
        const FString SourceId = Brief.SourceRequirementId.ToString();
        TestFalse(TEXT("No unauthored Turn slot is sent to Higgsfield"), SourceId.StartsWith(TEXT("turn-gap.")));
        TestFalse(TEXT("No unauthored Standing slot is sent to Higgsfield"), SourceId.StartsWith(TEXT("standing-gap.")));
        TestFalse(TEXT("No unauthored minor-dungeon slot is sent to Higgsfield"), SourceId.Contains(TEXT("minor-slot")));

        TestFalse(TEXT("No dungeon look-development brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.dungeon.")));
        TestFalse(TEXT("No region look-development brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.region.")));
        TestFalse(TEXT("No world-anchor look-development brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.world.")));
        TestFalse(TEXT("No character-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.character.")));
        TestFalse(TEXT("No boss-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.boss-visual.")));
        TestFalse(TEXT("No ship-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.ship.")));
        TestFalse(TEXT("No fauna-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.fauna.")));
        TestFalse(TEXT("No flora-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.flora.")));
        TestFalse(TEXT("No prop-look brief is sent to Higgsfield"), Id.StartsWith(TEXT("external.higgsfield.prop.")));

        switch (Brief.MediaKind)
        {
        case EExternalAssetMediaKind::MotionPrevisVideo:
        case EExternalAssetMediaKind::FacialPerformanceReference:
            ++MotionOrPerformance;
            break;
        case EExternalAssetMediaKind::CinematicPrevisVideo:
            ++Cinematic;
            break;
        default:
            AddError(FString::Printf(TEXT("Disallowed Higgsfield media kind for %s"), *Id));
            break;
        }
    }

    TestEqual(TEXT("Twenty-three animation motion/performance briefs derive from the animation catalog"),
        MotionOrPerformance,
        FExternalAssetProductionCatalog::NamedAnimationBriefCount
            + FExternalAssetProductionCatalog::SystemAnimationBriefCount);
    TestEqual(TEXT("Fourteen cinematic-previs briefs derive only from resolved cutscene identities"),
        Cinematic, FExternalAssetProductionCatalog::ResolvedPresentationBriefCount);

    TestEqual(TEXT("Five unresolved final-act cutscenes remain outside Higgsfield production"),
        FExternalAssetProductionCatalog::UnresolvedPresentationIdentityCount, 5);
    TestEqual(TEXT("Twenty minor-dungeon identities remain deliberately unauthored"),
        FExternalAssetProductionCatalog::UnauthoredMinorDungeonIdentityCount, 20);
    TestEqual(TEXT("Two hundred seventy-five Turn/Standing identities remain deliberately unauthored"),
        FExternalAssetProductionCatalog::DeliberateTurnStandingIdentityGapCount, 275);

    TestFalse(TEXT("External providers can never create canon"),
        FExternalAssetProductionCatalog::AllowsProviderToCreateCanon());
    TestFalse(TEXT("Generated media never self-counts as imported UE content"),
        FExternalAssetProductionCatalog::AllowsGeneratedMediaToCountAsImportedAsset());
    TestFalse(TEXT("Generated media never self-counts as runtime-accepted content"),
        FExternalAssetProductionCatalog::AllowsGeneratedMediaToCountAsRuntimeAccepted());
    TestFalse(TEXT("External asset pipeline cannot buy or upgrade providers automatically"),
        FExternalAssetProductionCatalog::AllowsAutomaticProviderPurchaseOrUpgrade());
    TestFalse(TEXT("Higgsfield cannot own static visual production"),
        FExternalAssetProductionCatalog::AllowsHiggsfieldStaticVisualProduction());
    TestFalse(TEXT("Higgsfield cannot choose character/world looks"),
        FExternalAssetProductionCatalog::AllowsHiggsfieldCharacterOrWorldLookProduction());
    TestFalse(TEXT("Higgsfield cannot claim 3D production without a verified action"),
        FExternalAssetProductionCatalog::AllowsHiggsfieldThreeDProductionWithoutVerifiedAction());

    TestFalse(TEXT("Generic filler prompts are forbidden"),
        FExternalAssetProductionCatalog::AllowsGenericFillerPrompt());
    TestFalse(TEXT("Providers cannot resolve design gaps"),
        FExternalAssetProductionCatalog::AllowsProviderToResolveDesignGap());
    TestFalse(TEXT("Ungrounded decorative completion is forbidden"),
        FExternalAssetProductionCatalog::AllowsUngroundedDecorativeCompletion());
    TestFalse(TEXT("Unreviewed provider defaults cannot become identity"),
        FExternalAssetProductionCatalog::AllowsUnreviewedProviderDefaultToBecomeIdentity());
    TestTrue(TEXT("Every provider brief requires source specificity"),
        FExternalAssetProductionCatalog::RequiresSourceSpecificity());
    TestTrue(TEXT("Every important candidate requires functional-logic review"),
        FExternalAssetProductionCatalog::RequiresFunctionalLogicReview());
    TestTrue(TEXT("Every important candidate requires restraint and repetition review"),
        FExternalAssetProductionCatalog::RequiresRestraintAndRepetitionReview());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
