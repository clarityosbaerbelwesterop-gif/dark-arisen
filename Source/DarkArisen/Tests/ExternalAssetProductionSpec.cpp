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
    TestEqual(TEXT("Exactly fifty-nine source-derived Higgsfield briefs exist"),
        Briefs.Num(), FExternalAssetProductionCatalog::RequiredHiggsfieldBriefCount);

    int32 MotionOrPerformance = 0;
    int32 Cinematic = 0;
    int32 Concepts = 0;

    for (const FExternalAssetProductionBrief& Brief : Briefs)
    {
        TestEqual(TEXT("Current external brief remains requirement-only"),
            Brief.EvidenceState, EExternalAssetEvidenceState::RequirementOnly);
        TestTrue(TEXT("No provider job id is fabricated"), Brief.ProviderJobId.IsEmpty());
        TestTrue(TEXT("No provider media reference is fabricated"), Brief.MediaReference.IsEmpty());
        TestTrue(TEXT("No Unreal asset path is fabricated"), Brief.UnrealAssetPath.IsEmpty());
        TestFalse(TEXT("Provider cost approval is not self-granted"), Brief.bProviderCostApproved);
        TestFalse(TEXT("Shipping rights are not self-granted"), Brief.bShippingRightsCleared);

        switch (Brief.MediaKind)
        {
        case EExternalAssetMediaKind::MotionPrevisVideo:
        case EExternalAssetMediaKind::FacialPerformanceReference:
            ++MotionOrPerformance;
            break;
        case EExternalAssetMediaKind::CinematicPrevisVideo:
            ++Cinematic;
            break;
        case EExternalAssetMediaKind::ConceptReferenceImage:
            ++Concepts;
            break;
        default:
            break;
        }
    }

    TestEqual(TEXT("Twenty-three animation motion/performance briefs derive from the animation catalog"),
        MotionOrPerformance,
        FExternalAssetProductionCatalog::NamedAnimationBriefCount
            + FExternalAssetProductionCatalog::SystemAnimationBriefCount);
    TestEqual(TEXT("Fourteen cinematic-previs briefs derive only from resolved cutscene identities"),
        Cinematic,
        FExternalAssetProductionCatalog::ResolvedPresentationBriefCount);
    TestEqual(TEXT("Twenty-two concept briefs cover twelve Highmoore anchors plus ten grounded props"),
        Concepts,
        FExternalAssetProductionCatalog::HighmooreWorldBriefCount
            + FExternalAssetProductionCatalog::StateTreasureBriefCount
            + FExternalAssetProductionCatalog::UniqueRewardBriefCount);

    TestFalse(TEXT("External providers can never create canon"),
        FExternalAssetProductionCatalog::AllowsProviderToCreateCanon());
    TestFalse(TEXT("Generated media never self-counts as imported UE content"),
        FExternalAssetProductionCatalog::AllowsGeneratedMediaToCountAsImportedAsset());
    TestFalse(TEXT("Generated media never self-counts as runtime-accepted content"),
        FExternalAssetProductionCatalog::AllowsGeneratedMediaToCountAsRuntimeAccepted());
    TestFalse(TEXT("External asset pipeline cannot buy or upgrade providers automatically"),
        FExternalAssetProductionCatalog::AllowsAutomaticProviderPurchaseOrUpgrade());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
