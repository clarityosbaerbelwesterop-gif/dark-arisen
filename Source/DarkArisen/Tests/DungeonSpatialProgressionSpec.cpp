// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/DungeonSpatialProgressionComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDungeonSpatialProgressionSpec,
    "DarkArisen.Content.Dungeons.SpatialProgression",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDungeonSpatialProgressionSpec::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("Outside advances only to Threshold"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Outside, EDungeonSpatialBeat::Threshold));
    TestFalse(TEXT("Outside cannot skip directly to Descent"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Outside, EDungeonSpatialBeat::Descent));
    TestTrue(TEXT("Threshold advances to Descent"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Threshold, EDungeonSpatialBeat::Descent));
    TestTrue(TEXT("Descent advances to Turn"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Descent, EDungeonSpatialBeat::Turn));
    TestTrue(TEXT("Turn advances to Depth"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Turn, EDungeonSpatialBeat::Depth));
    TestTrue(TEXT("Depth advances to Floor"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Depth, EDungeonSpatialBeat::Floor));
    TestTrue(TEXT("Floor advances to inside-opened Return"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Floor, EDungeonSpatialBeat::ReturnOpened));
    TestFalse(TEXT("Return cannot be recorded before Floor"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Depth, EDungeonSpatialBeat::ReturnOpened));
    TestFalse(TEXT("Progression cannot move backwards"),
        UDungeonSpatialProgressionComponent::IsLegalNextBeat(
            EDungeonSpatialBeat::Depth, EDungeonSpatialBeat::Turn));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
