// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Bosses/IsabelCruzCharacter.h"
#include "Bosses/Tier1CommanderCatalog.h"
#include "Bosses/Tier1CommanderEncounterComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenTier1CommanderCatalogSpec,
    "DarkArisen.Content.Bosses.Tier1CommanderCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenTier1CommanderCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("The Nine Who Hold catalog validates"), FTier1CommanderCatalog::Validate(Errors));
    TestEqual(TEXT("Commander catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FTier1CommanderDefinition> All = FTier1CommanderCatalog::BuildAll();
    TestEqual(TEXT("Exactly nine Tier-1 commanders exist"), All.Num(), 9);

    FTier1CommanderDefinition Herrera;
    TestTrue(TEXT("Herrera is present"), FTier1CommanderCatalog::TryGet(TEXT("boss.herrera"), Herrera));
    TestTrue(TEXT("Herrera keeps the undercity variant"), Herrera.bHasUndercityVariant);
    TestEqual(TEXT("Herrera first threshold is 70 percent"), Herrera.PhaseHealthFractions[0], 0.70f);
    TestEqual(TEXT("Herrera second threshold is 35 percent"), Herrera.PhaseHealthFractions[1], 0.35f);

    FTier1CommanderDefinition Cruz;
    TestTrue(TEXT("Cruz is present"), FTier1CommanderCatalog::TryGet(TEXT("boss.cruz"), Cruz));
    TestEqual(TEXT("Cruz catalog matches native M2 65 percent threshold"), Cruz.PhaseHealthFractions[0], AIsabelCruzCharacter::ListPhaseHealthFraction);
    TestEqual(TEXT("Cruz catalog matches native M2 30 percent threshold"), Cruz.PhaseHealthFractions[1], AIsabelCruzCharacter::EighteenFortySixPhaseHealthFraction);

    FTier1CommanderDefinition Sterling;
    TestTrue(TEXT("Sterling is present"), FTier1CommanderCatalog::TryGet(TEXT("boss.sterling"), Sterling));
    TestEqual(TEXT("Sterling remains a naval engagement"), Sterling.EncounterMode, ETier1CommanderEncounterMode::NavalEngagement);
    TestTrue(TEXT("Sterling keeps boarding variant"), Sterling.bHasBoardingVariant);
    TestTrue(TEXT("Sterling keeps alliance avoidance"), Sterling.bHasAllianceAvoidancePath);

    FTier1CommanderDefinition Blackwood;
    TestTrue(TEXT("Blackwood is present"), FTier1CommanderCatalog::TryGet(TEXT("boss.blackwood"), Blackwood));
    TestFalse(TEXT("Blackwood is not a direct combatant"), Blackwood.bDirectCombatant);
    TestTrue(TEXT("Blackwood archive objective remains fire-sensitive"), Blackwood.bObjectiveCanBeDestroyedByFire);

    FTier1CommanderDefinition Thorne;
    TestTrue(TEXT("Thorne is present"), FTier1CommanderCatalog::TryGet(TEXT("boss.thorne"), Thorne));
    TestTrue(TEXT("Thorne garrison weakens over encounter time"), Thorne.bGarrisonWeakensOverTime);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenTier1CommanderEncounterSpec,
    "DarkArisen.Content.Bosses.Tier1CommanderEncounterState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenTier1CommanderEncounterSpec::RunTest(const FString& Parameters)
{
    UTier1CommanderEncounterComponent* Reyes = NewObject<UTier1CommanderEncounterComponent>();
    Reyes->CommanderStableId = TEXT("boss.reyes");
    TestTrue(TEXT("Reyes definition initializes"), Reyes->InitializeDefinition());
    TestTrue(TEXT("Reyes encounter starts"), Reyes->BeginEncounter());
    TestEqual(TEXT("Reyes starts Professional"), Reyes->GetCurrentPhaseId(), FName(TEXT("professional")));
    TestTrue(TEXT("Reyes crosses to phase two at 60 percent"), Reyes->UpdateHealthFraction(0.60f));
    TestEqual(TEXT("Reyes phase two is Arithmetic Arrives"), Reyes->GetCurrentPhaseId(), FName(TEXT("arithmetic-arrives")));
    TestTrue(TEXT("Reyes crosses to phase three at 25 percent"), Reyes->UpdateHealthFraction(0.25f));
    TestTrue(TEXT("Reyes can be spared"), Reyes->TryResolveSpared());
    TestEqual(TEXT("Reyes spared state persists"), Reyes->Resolution, ETier1CommanderResolution::Spared);

    UTier1CommanderEncounterComponent* Vega = NewObject<UTier1CommanderEncounterComponent>();
    Vega->CommanderStableId = TEXT("boss.vega");
    TestTrue(TEXT("Vega initializes"), Vega->InitializeDefinition());
    TestTrue(TEXT("Vega encounter starts"), Vega->BeginEncounter());
    TestFalse(TEXT("Vega refuses a clean spare"), Vega->TryResolveSpared());
    TestTrue(TEXT("Vega spare refusal is observed"), Vega->bSpareRefusedObserved);
    TestEqual(TEXT("Vega remains unresolved after refusing mercy"), Vega->Resolution, ETier1CommanderResolution::Unresolved);
    TestTrue(TEXT("Walking away can resolve Vega as avoided"), Vega->ResolveAvoided(TEXT("vega.leave-wall")));

    UTier1CommanderEncounterComponent* DeSilva = NewObject<UTier1CommanderEncounterComponent>();
    DeSilva->CommanderStableId = TEXT("boss.de_silva");
    TestTrue(TEXT("de Silva initializes"), DeSilva->InitializeDefinition());
    TestFalse(TEXT("de Silva is not the direct fighter"), DeSilva->IsDirectCombatant());
    TestTrue(TEXT("de Silva encounter starts"), DeSilva->BeginEncounter());
    TestEqual(TEXT("de Silva starts Opening Bid"), DeSilva->GetCurrentPhaseId(), FName(TEXT("opening-bid")));
    TestFalse(TEXT("scripted stage cannot skip ahead"), DeSilva->AdvanceScriptedStage(TEXT("ledgers")));
    TestTrue(TEXT("scripted stage advances in order"), DeSilva->AdvanceScriptedStage(TEXT("sweetener")));
    TestTrue(TEXT("scripted stage advances to ledgers"), DeSilva->AdvanceScriptedStage(TEXT("ledgers")));

    UTier1CommanderEncounterComponent* Sterling = NewObject<UTier1CommanderEncounterComponent>();
    Sterling->CommanderStableId = TEXT("boss.sterling");
    TestTrue(TEXT("Sterling initializes"), Sterling->InitializeDefinition());
    TestTrue(TEXT("Sterling alliance path can resolve before combat"), Sterling->ResolveAvoided(TEXT("sterling.alliance")));
    TestEqual(TEXT("Sterling remains alive via avoided route"), Sterling->Resolution, ETier1CommanderResolution::Avoided);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
