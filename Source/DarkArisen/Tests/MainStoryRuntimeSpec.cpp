#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Story/MainStoryMissionCatalog.h"
#include "Story/MainStorySubsystem.h"
#include "Persistence/DarkArisenSaveGame.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainStoryCatalogSpec,"DarkArisen.Alpha.Story.CatalogAndLegacy",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMainStoryCatalogSpec::RunTest(const FString&)
{
 TArray<FString> Errors;TestTrue(TEXT("34-mission catalog validates"),FMainStoryMissionCatalog::Validate(Errors));TestEqual(TEXT("Exactly 34 missions"),FMainStoryMissionCatalog::Get().Num(),34);
 TestEqual(TEXT("First mission"),FMainStoryMissionCatalog::Get()[0].MissionId,FName(TEXT("Main.C01.01.HomeWater")));TestEqual(TEXT("Credits mission"),FMainStoryMissionCatalog::Get().Last().MissionId,FName(TEXT("Main.C10.05.TheWakeAfter")));
 for(const auto&M:FMainStoryMissionCatalog::Get()){const FString Id=M.MissionId.ToString().ToLower();TestFalse(TEXT("No active Ethan boss/betrayal path"),Id.Contains(TEXT("ethanboss"))||Id.Contains(TEXT("ethan.boss"))||Id.Contains(TEXT("ethanbetrayal")));}return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainStoryCreditsSpec,"DarkArisen.Alpha.Story.StartToCredits",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMainStoryCreditsSpec::RunTest(const FString&)
{
 auto*S=NewObject<UMainStorySubsystem>();S->NewGame();int32 Steps=0;
 for(const auto&M:FMainStoryMissionCatalog::Get()){TestTrue(TEXT("Mission becomes active deterministically"),S->ActivateMission(M.MissionId));TestTrue(TEXT("Mission completes"),S->CompleteMission(M.MissionId));if(++Steps>34){AddError(TEXT("Infinite progression"));break;}}
 TestEqual(TEXT("Exactly 34 progression steps"),Steps,34);TestTrue(TEXT("Marc dies"),S->HasStoryFact(TEXT("Story.MarcDead")));TestTrue(TEXT("Denise dies"),S->HasStoryFact(TEXT("Story.DeniseDead")));TestTrue(TEXT("Ethan is abducted"),S->HasStoryFact(TEXT("Story.EthanAbducted")));TestTrue(TEXT("Ethan is recovered in chapter 8 route"),S->HasStoryFact(TEXT("Story.EthanRecovered")));TestTrue(TEXT("Moran reached"),S->HasWorldFact(TEXT("World.MoranEntered")));TestTrue(TEXT("Driftwood reached"),S->HasWorldFact(TEXT("World.DriftwoodBeachReached")));TestTrue(TEXT("Moran route known"),S->HasWorldFact(TEXT("World.MoranOpeningRouteKnown")));TestTrue(TEXT("La Liberacion owned"),S->HasWorldFact(TEXT("Ship.LaLiberacionOwned")));TestTrue(TEXT("Rexa reached"),S->HasWorldFact(TEXT("World.RexaEntered")));TestTrue(TEXT("Main complete"),S->HasStoryFact(TEXT("Story.MainComplete")));TestTrue(TEXT("Credits reachable"),S->HasWorldFact(TEXT("Story.CreditsReachable")));TestFalse(TEXT("Dream fight never defeats Ethan"),S->GetState()->DefeatedBosses.Contains(TEXT("boss.ethan")));TArray<FString>E;TestTrue(TEXT("Final state validates"),S->Validate(E));return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainStoryPersistenceSpec,"DarkArisen.Alpha.Persistence.Validation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMainStoryPersistenceSpec::RunTest(const FString&)
{
 auto*S=NewObject<UMainStorySubsystem>();S->NewGame();TestEqual(TEXT("Fresh chapter"),S->GetState()->CurrentChapter,1);TestEqual(TEXT("Fresh mission available"),S->GetMissionState(TEXT("Main.C01.01.HomeWater")),EMainMissionState::Available);
 for(int32 I=0;I<3;++I){FName Id=FMainStoryMissionCatalog::Get()[I].MissionId;TestTrue(TEXT("Activate pre-abduction mission"),S->ActivateMission(Id));TestTrue(TEXT("Complete pre-abduction mission"),S->CompleteMission(Id));}
 TestTrue(TEXT("Abduction restored in state"),S->HasStoryFact(TEXT("Story.EthanAbducted")));UDarkArisenSaveGame* Snapshot=DuplicateObject<UDarkArisenSaveGame>(S->GetState(),S);TArray<FString>E;TestTrue(TEXT("Snapshot validates"),S->ValidateState(Snapshot,E));
 Snapshot->StoryFacts.Add(TEXT("Story.EthanRecovered"));Snapshot->StoryFacts.Remove(TEXT("Story.EthanAbducted"));TestFalse(TEXT("Recovered without abducted rejected"),S->ValidateState(Snapshot,E));Snapshot=DuplicateObject<UDarkArisenSaveGame>(S->GetState(),S);Snapshot->StoryFacts.Append({TEXT("Story.DravenKilled"),TEXT("Story.DravenCaptured")});TestFalse(TEXT("Conflicting Draven outcomes rejected"),S->ValidateState(Snapshot,E));
 TestTrue(TEXT("Boss state persists"),S->MarkBossDefeated(TEXT("boss.herrera")));TestTrue(TEXT("Boss state recorded"),S->GetState()->DefeatedBosses.Contains(TEXT("boss.herrera")));return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOpeningCrewProgressionSpec,"DarkArisen.Alpha.Opening.CrewProgression",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FOpeningCrewProgressionSpec::RunTest(const FString&)
{
 auto*S=NewObject<UMainStorySubsystem>();S->NewGame();
 TestTrue(TEXT("Mira can be met through an authored scene"),S->MeetCrew(TEXT("crew.mira")));
 TestFalse(TEXT("Arrival alone cannot recruit Mira"),S->RecruitCrew(TEXT("crew.mira"),TEXT("FirstMate")));
 TestTrue(TEXT("Mira can become explicitly available"),S->SetCrewAvailable(TEXT("crew.mira"),TEXT("FirstMate")));
 TestFalse(TEXT("Role cannot silently change during recruitment"),S->RecruitCrew(TEXT("crew.mira"),TEXT("Navigator")));
 TestTrue(TEXT("Explicit recruitment resolves"),S->RecruitCrew(TEXT("crew.mira"),TEXT("FirstMate")));
 TestTrue(TEXT("Recruitment persists"),S->IsCrewRecruited(TEXT("crew.mira")));
 TestFalse(TEXT("Crew cannot be put aboard before the ship is owned"),S->SetCrewAboard(TEXT("crew.mira"),true));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSaveV1MigrationSpec,"DarkArisen.Alpha.Persistence.V1CrewMigration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FSaveV1MigrationSpec::RunTest(const FString&)
{
 auto*S=NewObject<UMainStorySubsystem>();
 UDarkArisenSaveGame* Legacy=NewObject<UDarkArisenSaveGame>();Legacy->SaveVersion=1;
 FCrewRelationshipState Crew;Crew.CharacterId=TEXT("crew.mira");Crew.bRecruited=true;Legacy->Crew.Add(Crew);
 Legacy->QuestOutcomes.Add(TEXT("CrewRole.crew.mira"),TEXT("FirstMate"));TArray<FString>Errors;
 TestTrue(TEXT("v1 save migrates"),S->MigrateVersion(Legacy,Errors));
 TestEqual(TEXT("save version becomes current"),Legacy->SaveVersion,UDarkArisenSaveGame::CurrentVersion);
 TestTrue(TEXT("legacy recruited crew is marked met"),Legacy->Crew[0].bMet);
 TestTrue(TEXT("legacy recruited crew is marked available"),Legacy->Crew[0].bAvailable);
 TestEqual(TEXT("legacy role is restored"),Legacy->Crew[0].Role,FName(TEXT("FirstMate")));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOpeningPersistenceSpec,"DarkArisen.Alpha.Persistence.OpeningRoute",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FOpeningPersistenceSpec::RunTest(const FString&)
{
 auto*S=NewObject<UMainStorySubsystem>();S->NewGame();FOpeningProgressState P;P.Location=8;P.RaidState=7;P.RecoveryState=5;P.FamilyInteractions.Append({TEXT("character.marc"),TEXT("character.denise"),TEXT("character.ethan")});
 TestTrue(TEXT("Valid opening route state is accepted"),S->SetOpeningProgress(P));UDarkArisenSaveGame* Snapshot=DuplicateObject<UDarkArisenSaveGame>(S->GetState(),S);
 TestEqual(TEXT("Opening location survives save snapshot"),Snapshot->OpeningProgress.Location,static_cast<uint8>(8));TestEqual(TEXT("Family interactions survive save snapshot"),Snapshot->OpeningProgress.FamilyInteractions.Num(),3);
 P.Location=12;TestFalse(TEXT("Out-of-range opening state fails closed"),S->SetOpeningProgress(P));
 P=FOpeningProgressState();P.BoardingEncounterId=TEXT("Encounter.HarlowDeck.Wave01");P.BoardersRequired=1;TestFalse(TEXT("Single-boarder shortcut is rejected"),S->SetOpeningProgress(P));
 P.BoardersRequired=3;P.BoardersDefeated=1;TestTrue(TEXT("Partial real boarding wave persists"),S->SetOpeningProgress(P));
 P.bDravenBoarded=true;TestFalse(TEXT("Draven cannot board before wave completion"),S->SetOpeningProgress(P));
 P.BoardersDefeated=3;TestTrue(TEXT("Draven boarding gate persists after wave completion"),S->SetOpeningProgress(P));
 P=FOpeningProgressState();P.bLaLiberacionHelmSecured=true;TestFalse(TEXT("Helm cannot be secured before boarding ship"),S->SetOpeningProgress(P));
 P.bLaLiberacionBoarded=true;TestTrue(TEXT("Boarded and helm-secured ship state is valid"),S->SetOpeningProgress(P));
 P.bLaLiberacionClearedHarbor=true;TestTrue(TEXT("Harbor-clear state persists after helm"),S->SetOpeningProgress(P));
 return true;
}
#endif
