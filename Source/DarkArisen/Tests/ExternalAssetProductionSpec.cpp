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
    TestEqual(TEXT("Exactly the finite source-derived Higgsfield brief set exists"),
        Briefs.Num(), FExternalAssetProductionCatalog::RequiredHiggsfieldBriefCount);

    int32 MotionOrPerformance = 0;
    int32 Cinematic = 0;
    int32 Concepts = 0;
    int32 DungeonLooks = 0;
    int32 RegionLooks = 0;
    int32 HighmooreAnchors = 0;
    int32 Props = 0;

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
        TestFalse(TEXT("No unauthored Turn slot is sent to the provider"), SourceId.StartsWith(TEXT("turn-gap.")));
        TestFalse(TEXT("No unauthored Standing slot is sent to the provider"), SourceId.StartsWith(TEXT("standing-gap.")));
        TestFalse(TEXT("No unauthored minor-dungeon slot is sent to the provider"), SourceId.Contains(TEXT("minor-slot")));

        if (Id.StartsWith(TEXT("external.higgsfield.dungeon."))) ++DungeonLooks;
        else if (Id.StartsWith(TEXT("external.higgsfield.region."))) ++RegionLooks;
        else if (Id.StartsWith(TEXT("external.higgsfield.world."))) ++HighmooreAnchors;
        else if (Id.StartsWith(TEXT("external.higgsfield.prop."))) ++Props;

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
    TestEqual(TEXT("Forty grounded named dungeons have source-backed visual briefs"),
        DungeonLooks, FExternalAssetProductionCatalog::GroundedDungeonBriefCount);
    TestEqual(TEXT("Eight world regions have source-backed visual briefs"),
        RegionLooks, FExternalAssetProductionCatalog::WorldRegionBriefCount);
    TestEqual(TEXT("Twelve Highmoore anchors remain individually briefed"),
        HighmooreAnchors, FExternalAssetProductionCatalog::HighmooreWorldBriefCount);
    TestEqual(TEXT("Nine State Treasures plus the grounded unique reward remain individually briefed"),
        Props,
        FExternalAssetProductionCatalog::StateTreasureBriefCount
            + FExternalAssetProductionCatalog::UniqueRewardBriefCount);
    TestEqual(TEXT("Concept-reference coverage includes dungeons, regions, Highmoore anchors and grounded props"),
        Concepts,
        FExternalAssetProductionCatalog::GroundedDungeonBriefCount
            + FExternalAssetProductionCatalog::WorldRegionBriefCount
            + FExternalAssetProductionCatalog::HighmooreWorldBriefCount
            + FExternalAssetProductionCatalog::StateTreasureBriefCount
            + FExternalAssetProductionCatalog::UniqueRewardBriefCount);

    TestEqual(TEXT("Five unresolved final-act cutscenes remain outside provider production"),
        FExternalAssetProductionCatalog::UnresolvedPresentationIdentityCount, 5);
    TestEqual(TEXT("Twenty minor-dungeon identities remain outside provider production"),
        FExternalAssetProductionCatalog::UnauthoredMinorDungeonIdentityCount, 20);
    TestEqual(TEXT("Two hundred seventy-five Turn/Standing identities remain outside provider production"),
        FExternalAssetProductionCatalog::DeliberateTurnStandingIdentityGapCount, 275);

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
