// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/CreatureVisualProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenCreatureVisualProductionSpec,
    "DarkArisen.Production.CreatureVisualProduction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenCreatureVisualProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Creature visual production catalog validates structurally"),
        FCreatureVisualProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Creature visual production catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FCreatureVisualProductionBrief> Briefs = FCreatureVisualProductionCatalog::BuildBriefs();
    TestEqual(TEXT("Thirty source-grounded creature visual anchors are registered"),
        Briefs.Num(), FCreatureVisualProductionCatalog::RequiredBriefCount);

    int32 Ready = 0;
    int32 Blocked = 0;
    int32 HardNonCombat = 0;
    for (const FCreatureVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;
        if (Brief.bMustRemainNonCombat) ++HardNonCombat;
    }

    TestEqual(TEXT("Twenty-eight creature visual references are provider-ready"),
        Ready, FCreatureVisualProductionCatalog::ProviderReadyBriefCount);
    TestEqual(TEXT("Two creature visual identities remain provider-blocked"),
        Blocked, FCreatureVisualProductionCatalog::ProviderBlockedBriefCount);
    TestTrue(TEXT("Catalog retains explicit hard non-combat creature encounters"), HardNonCombat > 0);

    const FCreatureVisualProductionBrief* MemoryFish = Briefs.FindByPredicate([](const FCreatureVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("creature-visual.memory-fish");
    });
    TestNotNull(TEXT("Memory Fish visual identity is tracked"), MemoryFish);
    if (MemoryFish != nullptr)
    {
        TestFalse(TEXT("Memory Fish fixed look is not provider-generated"), MemoryFish->bProviderReferenceReady);
        TestTrue(TEXT("Memory Fish retains explicit visual design gap"),
            MemoryFish->AntiInventionBoundary.Contains(TEXT("DESIGN-GAP")));
    }

    const FCreatureVisualProductionBrief* HouseHound = Briefs.FindByPredicate([](const FCreatureVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("creature-visual.highmoore.house-hound");
    });
    TestNotNull(TEXT("Highmoore House Hound visual family is tracked"), HouseHound);
    if (HouseHound != nullptr)
    {
        TestFalse(TEXT("Unauthored House-specific hound breed is not provider-generated"), HouseHound->bProviderReferenceReady);
    }

    const FCreatureVisualProductionBrief* DeepOne = Briefs.FindByPredicate([](const FCreatureVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("creature-visual.deep-one");
    });
    TestNotNull(TEXT("Deep One partial visual reference exists"), DeepOne);
    if (DeepOne != nullptr)
    {
        TestTrue(TEXT("Deep One brief forbids completing the unauthored body"),
            DeepOne->AntiInventionBoundary.Contains(TEXT("do not design a complete canonical body")));
        TestTrue(TEXT("Deep One remains non-combat"), DeepOne->bMustRemainNonCombat);
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
