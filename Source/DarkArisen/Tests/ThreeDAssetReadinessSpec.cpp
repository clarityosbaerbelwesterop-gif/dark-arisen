// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/ThreeDAssetReadinessCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenThreeDAssetReadinessSpec,
    "DarkArisen.Production.ThreeDAssetReadiness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenThreeDAssetReadinessSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("3D source-readiness catalog validates structurally"),
        FThreeDAssetReadinessCatalog::Validate(Errors));
    TestEqual(TEXT("3D source-readiness catalog has no structural validation errors"), Errors.Num(), 0);

    const TArray<FThreeDAssetReadinessRecord> Records = FThreeDAssetReadinessCatalog::BuildRecords();
    TestEqual(TEXT("Cross-family 3D register has the finite source-derived record count"),
        Records.Num(), FThreeDAssetReadinessCatalog::RequiredRecordCount);

    int32 CandidateReady = 0;
    int32 ReferenceOnly = 0;
    int32 Blocked = 0;
    for (const FThreeDAssetReadinessRecord& Entry : Records)
    {
        switch (Entry.SourceReadiness)
        {
        case EThreeDSourceReadiness::CandidateGeometryReady: ++CandidateReady; break;
        case EThreeDSourceReadiness::ReferenceOnly: ++ReferenceOnly; break;
        case EThreeDSourceReadiness::Blocked: ++Blocked; break;
        }

        TestFalse(TEXT("No 3D record invents connected provider capability"), Entry.bVerifiedConnectedProvider3DAction);
        TestFalse(TEXT("No 3D candidate is fabricated"), Entry.bCandidateGenerated);
        TestFalse(TEXT("No Unreal 3D import is fabricated"), Entry.bImportedForReview);
        TestFalse(TEXT("No 3D runtime acceptance is fabricated"), Entry.bRuntimeAccepted);
    }

    TestEqual(TEXT("Exactly the source-grounded geometry subset is candidate-ready"),
        CandidateReady, FThreeDAssetReadinessCatalog::CandidateGeometryReadyCount);
    TestEqual(TEXT("Variable/incomplete geometry remains reference-only"),
        ReferenceOnly, FThreeDAssetReadinessCatalog::ReferenceOnlyCount);
    TestEqual(TEXT("Authority-blocked geometry remains blocked"),
        Blocked, FThreeDAssetReadinessCatalog::BlockedCount);

    const auto FindBySource = [&Records](const TCHAR* SourceId) -> const FThreeDAssetReadinessRecord*
    {
        const FName Wanted(SourceId);
        return Records.FindByPredicate([Wanted](const FThreeDAssetReadinessRecord& Entry)
        {
            return Entry.SourceRequirementId == Wanted;
        });
    };

    const FThreeDAssetReadinessRecord* Jake = FindBySource(TEXT("character.jake-harlow"));
    TestNotNull(TEXT("Jake has an explicit 3D readiness record"), Jake);
    if (Jake != nullptr)
    {
        TestEqual(TEXT("Jake full rig stays reference-only until primary costume is locked"),
            Jake->SourceReadiness, EThreeDSourceReadiness::ReferenceOnly);
    }

    const FThreeDAssetReadinessRecord* DeepOne = FindBySource(TEXT("fauna.legendary.deep-one"));
    TestNotNull(TEXT("Deep One has an explicit 3D readiness record"), DeepOne);
    if (DeepOne != nullptr)
    {
        TestEqual(TEXT("Deep One cannot become a complete canonical 3D body"),
            DeepOne->SourceReadiness, EThreeDSourceReadiness::ReferenceOnly);
    }

    const FThreeDAssetReadinessRecord* FinalWolf = FindBySource(TEXT("fauna.legendary.final-wolf"));
    TestNotNull(TEXT("Final Wolf has an explicit 3D readiness record"), FinalWolf);
    if (FinalWolf != nullptr)
    {
        TestEqual(TEXT("Final Wolf remains fully blocked until save-history representation exists"),
            FinalWolf->SourceReadiness, EThreeDSourceReadiness::Blocked);
    }

    const FThreeDAssetReadinessRecord* InfiniteBranch = FindBySource(TEXT("flora.region06.infinite-branch"));
    TestNotNull(TEXT("Infinite Branch has an explicit 3D readiness record"), InfiniteBranch);
    if (InfiniteBranch != nullptr)
    {
        TestEqual(TEXT("Infinite Branch cannot be collapsed into one final static mesh"),
            InfiniteBranch->SourceReadiness, EThreeDSourceReadiness::ReferenceOnly);
    }

    const FThreeDAssetReadinessRecord* ShipExterior = FindBySource(TEXT("ship-visual.la-liberacion.exterior"));
    TestNotNull(TEXT("La Liberacion exterior has an explicit 3D readiness record"), ShipExterior);
    if (ShipExterior != nullptr)
    {
        TestEqual(TEXT("Final La Liberacion exterior hull remains blocked"),
            ShipExterior->SourceReadiness, EThreeDSourceReadiness::Blocked);
    }

    TestFalse(TEXT("Connected 3D generation is not falsely claimed"),
        FThreeDAssetReadinessCatalog::HasVerifiedConnectedProvider3DAction());
    TestEqual(TEXT("Six 3D design/evidence gaps stay explicit"),
        FThreeDAssetReadinessCatalog::BuildDesignGaps().Num(), 6);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
