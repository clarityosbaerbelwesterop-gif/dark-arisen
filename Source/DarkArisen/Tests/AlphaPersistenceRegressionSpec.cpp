#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Ship/LaLiberacionShip.h"
#include "Ship/ShipVoyageComponent.h"
#include "Story/MainStorySubsystem.h"
#include "World/DarkArisenWorldRulesSubsystem.h"
#include "World/NPCLivingWorldSubsystem.h"

namespace
{
struct FAlphaPersistenceTestWorld
{
    UWorld* World;
    explicit FAlphaPersistenceTestWorld(const FName Name)
        : World(UWorld::CreateWorld(EWorldType::Game, false, Name)) {}
    ~FAlphaPersistenceTestWorld() { if (World) World->DestroyWorld(false); }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlphaCorruptMissionSaveSpec,
    "DarkArisen.Alpha.Persistence.RejectCorruptMissionOrder",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAlphaCorruptMissionSaveSpec::RunTest(const FString&)
{
    auto* Story = NewObject<UMainStorySubsystem>();
    Story->NewGame();
    TArray<FString> Errors;
    TestTrue(TEXT("Fresh canonical save is valid"), Story->Validate(Errors));
    const auto Fresh = [Story]() { return DuplicateObject<UDarkArisenSaveGame>(Story->GetState(), Story); };
    auto* Candidate = Fresh();
    Candidate->MissionStates.Swap(0, 1);
    TestFalse(TEXT("Reordered mission array is rejected before indexed refresh"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->MissionStates[1].MissionId = Candidate->MissionStates[0].MissionId;
    TestFalse(TEXT("Duplicate mission identities are rejected"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->MissionStates[1].State = EMainMissionState::Active;
    TestFalse(TEXT("Future active mission cannot bypass its prerequisite"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->MissionStates[0].State = EMainMissionState::Failed;
    Candidate->MissionStates[1].State = EMainMissionState::Completed;
    TestFalse(TEXT("A failed predecessor does not permit later completion"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->MissionStates[0].State = static_cast<EMainMissionState>(255);
    TestFalse(TEXT("Unknown enum values fail closed"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->CurrentChapter = 10;
    TestFalse(TEXT("Current chapter must agree with the current mission"), Story->ValidateState(Candidate, Errors));
    Candidate = Fresh();
    Candidate->CurrentMission = TEXT("Main.Unknown");
    TestFalse(TEXT("An unresolvable current mission is rejected"), Story->ValidateState(Candidate, Errors));
    TestFalse(TEXT("Headless tests cannot write player save slots"), Story->Save(TEXT("DarkArisenAlpha"), 0));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlphaWorldTransferSpec,
    "DarkArisen.Alpha.Persistence.WorldTransferWithoutDiskSave",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAlphaWorldTransferSpec::RunTest(const FString&)
{
    FAlphaPersistenceTestWorld Source(TEXT("AlphaPersistenceSource"));
    FAlphaPersistenceTestWorld Destination(TEXT("AlphaPersistenceDestination"));
    if (!TestNotNull(TEXT("Source world"), Source.World)
        || !TestNotNull(TEXT("Destination world"), Destination.World)) return false;
    auto* SourceRules = Source.World->GetSubsystem<UDarkArisenWorldRulesSubsystem>();
    auto* DestinationRules = Destination.World->GetSubsystem<UDarkArisenWorldRulesSubsystem>();
    auto* SourceNPCs = Source.World->GetSubsystem<UNPCLivingWorldSubsystem>();
    auto* DestinationNPCs = Destination.World->GetSubsystem<UNPCLivingWorldSubsystem>();
    if (!TestNotNull(TEXT("Source rules"), SourceRules) || !TestNotNull(TEXT("Destination rules"), DestinationRules)
        || !TestNotNull(TEXT("Source NPC authority"), SourceNPCs)
        || !TestNotNull(TEXT("Destination NPC authority"), DestinationNPCs)) return false;

    auto* Story = NewObject<UMainStorySubsystem>();
    Story->NewGame();
    TestTrue(TEXT("Fresh world restores"), Story->RestoreWorldState(Source.World));
    TestEqual(TEXT("New game starts in chapter one"), SourceRules->GetChapter(), 1);
    SourceRules->CompleteRest(EDarkArisenRestLocation::SafeHouse, EDarkArisenDaypart::Dusk);
    SourceRules->BeginLakeToDockAutosaveSuppression();
    TestTrue(TEXT("Authored NPC registered"), SourceNPCs->RegisterNPC(TEXT("npc.test"), TEXT("community.test")));
    FNPCLivingMemory Memory;
    Memory.EventId = TEXT("event.test");
    Memory.Emotion = ENPCMemoryEmotion::Negative;
    Memory.OriginalWeight = 80;
    Memory.EffectiveWeight = 80.f;
    TestTrue(TEXT("Witnessed memory recorded"), SourceNPCs->RecordMemory(TEXT("npc.test"), Memory));
    TestTrue(TEXT("Live state is captured without disk IO"), Story->CaptureWorldState(Source.World));
    TestTrue(TEXT("Destination restores before population registration"), Story->RestoreWorldState(Destination.World));
    TestEqual(TEXT("World time survives travel"), DestinationRules->GetTotalWorldMinutes(), SourceRules->GetTotalWorldMinutes());
    TestTrue(TEXT("Autosave suppression survives travel"), DestinationRules->IsAutosaveSuppressed());
    TestFalse(TEXT("Travel does not manufacture an autosave request"), DestinationRules->ConsumePendingAutosaveRequest());
    DestinationNPCs->RegisterNPC(TEXT("npc.test"), TEXT("community.test"));
    FNPCLivingRecord Record;
    TestTrue(TEXT("Resident re-registration retains saved memory"), DestinationNPCs->TryGetRecord(TEXT("npc.test"), Record));
    TestEqual(TEXT("Exactly one memory retained"), Record.Memories.Num(), 1);
    TestEqual(TEXT("Reputation survives travel"), Record.PersonalReputation, -40);
    Story->NewGame();
    TestFalse(TEXT("New game clears world snapshots"), Story->GetState()->WorldRules.bValid);
    TestFalse(TEXT("New game clears NPC snapshots"), Story->GetState()->LivingNPCWorld.bValid);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlphaShipMapCoordinatesSpec,
    "DarkArisen.Alpha.Persistence.ShipCoordinatesStayInTheirMap",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAlphaShipMapCoordinatesSpec::RunTest(const FString&)
{
    FAlphaPersistenceTestWorld Fixture(TEXT("AlphaShipPersistence"));
    if (!TestNotNull(TEXT("Ship test world"), Fixture.World)) return false;
    ALaLiberacionShip* Ship = Fixture.World->SpawnActor<ALaLiberacionShip>();
    if (!TestNotNull(TEXT("Ship owner"), Ship)) return false;
    auto* Voyage = Ship->VoyageComponent;
    FShipVoyageSnapshot Snapshot = Voyage->CaptureSnapshot();
    Snapshot.SourceLevel = TEXT("AnEntirelyDifferentMap");
    Snapshot.WorldTransform = FTransform(FVector(99999.f, 0.f, 0.f));
    Snapshot.HullIntegrity = 500.f;
    const FVector Arrival = Ship->GetActorLocation();
    TestTrue(TEXT("Ship state restores across maps"), Voyage->RestoreSnapshot(Snapshot));
    TestTrue(TEXT("Foreign coordinates cannot relocate the destination ship"), Ship->GetActorLocation().Equals(Arrival));
    TestEqual(TEXT("Hull state still persists"), Voyage->CaptureSnapshot().HullIntegrity, 500.f);
    return true;
}

#endif
