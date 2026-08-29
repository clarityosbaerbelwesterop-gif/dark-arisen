// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/AuthoredDungeonSiteComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDungeonSiteStateSpec,
    "DarkArisen.Content.Dungeons.SiteState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDungeonSiteStateSpec::RunTest(const FString& Parameters)
{
    UAuthoredDungeonSiteComponent* Cenote = NewObject<UAuthoredDungeonSiteComponent>();
    Cenote->DungeonStableId = TEXT("dungeon.rexa.cenote-first-mother");
    TestTrue(TEXT("Known authored Cenote initializes"), Cenote->InitializeDefinition());
    TestEqual(TEXT("Cenote name comes from authored catalog"), Cenote->GetAuthoredDisplayName(), FString(TEXT("The Cenote of the First Mother")));
    TestFalse(TEXT("Environmental Cenote is not physically mapped before entry"), Cenote->CanBeRecordedOnPhysicalMapBeforeEntry());
    TestFalse(TEXT("Cenote cannot be marked on physical map before entry"), Cenote->CanBeRecordedOnPhysicalMapNow());
    TestFalse(TEXT("Image cannot be witnessed before physical entry"), Cenote->MarkUnforgettableImageWitnessed());
    TestFalse(TEXT("Return cannot open before physical entry"), Cenote->OpenReturnShortcutFromInside());

    TestTrue(TEXT("Physical entry records once"), Cenote->MarkEntered());
    TestFalse(TEXT("Entry is finite and cannot be recorded twice"), Cenote->MarkEntered());
    TestTrue(TEXT("Entered dungeon can now be physically annotated"), Cenote->CanBeRecordedOnPhysicalMapNow());
    TestTrue(TEXT("Unforgettable image records"), Cenote->MarkUnforgettableImageWitnessed());
    TestTrue(TEXT("Return opens from inside without invented boss-order dependency"), Cenote->OpenReturnShortcutFromInside());
    TestFalse(TEXT("Structural completion still waits for the authored bottom resolution"), Cenote->IsStructurallyComplete());
    TestTrue(TEXT("Keeper resolution records with caller-authored outcome id"), Cenote->MarkBottomResolved(TEXT("keeper.resolved")));
    TestTrue(TEXT("Cenote structural beats are now complete"), Cenote->IsStructurallyComplete());

    TestTrue(TEXT("Artifact disposition may be recorded once where site-specific content uses it"),
        Cenote->RecordArtifactDisposition(EDungeonArtifactDisposition::Returned));
    TestFalse(TEXT("Artifact disposition is persistent and not silently rewritten"),
        Cenote->RecordArtifactDisposition(EDungeonArtifactDisposition::Sold));

    UAuthoredDungeonSiteComponent* RootTemple = NewObject<UAuthoredDungeonSiteComponent>();
    RootTemple->DungeonStableId = TEXT("dungeon.rexa.root-temple-ixchel");
    TestTrue(TEXT("Cartographic Root Temple initializes"), RootTemple->InitializeDefinition());
    TestTrue(TEXT("Cartographic discovery alone can be represented on the physical map"), RootTemple->CanBeRecordedOnPhysicalMapBeforeEntry());
    TestTrue(TEXT("Cartographic site is physically recordable before entry"), RootTemple->CanBeRecordedOnPhysicalMapNow());

    UAuthoredDungeonSiteComponent* StoneField = NewObject<UAuthoredDungeonSiteComponent>();
    StoneField->DungeonStableId = TEXT("dungeon.fjordlund.stone-field-under-chamber");
    TestTrue(TEXT("No-boss carve-out initializes"), StoneField->InitializeDefinition());
    TestTrue(TEXT("Stone Field remains a no-boss site"), StoneField->IsNoBossSite());
    TestTrue(TEXT("No-boss site enters physically"), StoneField->MarkEntered());
    TestTrue(TEXT("No-boss site still records its unforgettable image"), StoneField->MarkUnforgettableImageWitnessed());
    TestTrue(TEXT("No-boss bottom resolves without a fabricated fight"), StoneField->MarkBottomResolved(TEXT("names.reached")));
    TestTrue(TEXT("No-boss site still opens its mandatory Return"), StoneField->OpenReturnShortcutFromInside());
    TestTrue(TEXT("No-boss site structurally completes without adding a boss"), StoneField->IsStructurallyComplete());

    UAuthoredDungeonSiteComponent* Fifth = NewObject<UAuthoredDungeonSiteComponent>();
    Fifth->DungeonStableId = TEXT("dungeon.cross-network.fifth-connection");
    TestTrue(TEXT("Fifth Connection initializes"), Fifth->InitializeDefinition());
    TestTrue(TEXT("Fifth Connection remains a secret site"), Fifth->IsSecretSite());
    TestTrue(TEXT("Fifth Connection remains no-boss/no-fight"), Fifth->IsNoBossSite());

    UAuthoredDungeonSiteComponent* Missing = NewObject<UAuthoredDungeonSiteComponent>();
    Missing->DungeonStableId = TEXT("dungeon.region06.unwritten-second-tier-e");
    TestFalse(TEXT("Unwritten Region 06 slot fails closed instead of becoming content"), Missing->InitializeDefinition());
    TestFalse(TEXT("Unknown site cannot progress"), Missing->MarkEntered());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
