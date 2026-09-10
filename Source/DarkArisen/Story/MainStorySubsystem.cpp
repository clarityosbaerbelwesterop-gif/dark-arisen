#include "Story/MainStorySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Story/MainStoryMissionCatalog.h"

namespace Facts {
const FName MarcDead(TEXT("Story.MarcDead")), DeniseDead(TEXT("Story.DeniseDead"));
const FName EthanAbducted(TEXT("Story.EthanAbducted")), EthanRecovered(TEXT("Story.EthanRecovered"));
const FName MainComplete(TEXT("Story.MainComplete")), DravenKilled(TEXT("Story.DravenKilled")), DravenCaptured(TEXT("Story.DravenCaptured"));
}
void UMainStorySubsystem::Initialize(FSubsystemCollectionBase& Collection){Super::Initialize(Collection); ResetForNewGame();}
void UMainStorySubsystem::NewGame(){ResetForNewGame();}
void UMainStorySubsystem::ResetForNewGame()
{
    State=NewObject<UDarkArisenSaveGame>(this); State->SaveVersion=UDarkArisenSaveGame::CurrentVersion;
    for(const auto& Def:FMainStoryMissionCatalog::Get()){FMainStoryMissionRuntime Runtime;Runtime.MissionId=Def.MissionId;State->MissionStates.Add(Runtime);}
    State->MissionStates[0].State=EMainMissionState::Available; State->CurrentMission=State->MissionStates[0].MissionId; State->CurrentChapter=1;
}
bool UMainStorySubsystem::Save(const FString& Slot,int32 User){TArray<FString> Errors;if(!Validate(Errors))return false;return UGameplayStatics::SaveGameToSlot(State,Slot,User);}
bool UMainStorySubsystem::Load(const FString& Slot,int32 User)
{
    UDarkArisenSaveGame* Loaded=Cast<UDarkArisenSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,User)); TArray<FString> Errors;
    if(!Loaded||!MigrateVersion(Loaded,Errors)||!ValidateState(Loaded,Errors)) return false;
    State=Loaded; RefreshAvailability(); return true;
}
bool UMainStorySubsystem::MigrateVersion(UDarkArisenSaveGame* Candidate,TArray<FString>& Errors) const
{
    if(!Candidate){Errors.Add(TEXT("Missing save object."));return false;}
    if(Candidate->SaveVersion<=0||Candidate->SaveVersion>UDarkArisenSaveGame::CurrentVersion){Errors.Add(TEXT("Unsupported save version."));return false;}
    Candidate->SaveVersion=UDarkArisenSaveGame::CurrentVersion; return true;
}
EMainMissionState UMainStorySubsystem::GetMissionState(FName Id) const{if(!State)return EMainMissionState::Locked;const auto* R=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});return R?R->State:EMainMissionState::Locked;}
bool UMainStorySubsystem::ActivateMission(FName Id)
{
    if(!State)return false;auto* R=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});if(!R||R->State!=EMainMissionState::Available)return false;
    for(auto& Other:State->MissionStates)if(Other.State==EMainMissionState::Active)return false; R->State=EMainMissionState::Active;State->CurrentMission=Id;State->CurrentChapter=FMainStoryMissionCatalog::Get()[FMainStoryMissionCatalog::FindIndex(Id)].Chapter;OnMissionChanged.Broadcast(Id,R->State);return true;
}
bool UMainStorySubsystem::CompleteMission(FName Id)
{
    if(!State)return false;auto* R=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});if(!R||R->State!=EMainMissionState::Active)return false;
    R->State=EMainMissionState::Completed;ApplyMissionFacts(Id);RefreshAvailability();OnMissionChanged.Broadcast(Id,R->State);return true;
}
bool UMainStorySubsystem::FailMission(FName Id){if(!State)return false;auto* R=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});if(!R||R->State!=EMainMissionState::Active)return false;R->State=EMainMissionState::Failed;OnMissionChanged.Broadcast(Id,R->State);return true;}
void UMainStorySubsystem::RefreshAvailability()
{
    const auto& Catalog=FMainStoryMissionCatalog::Get();
    for(int32 I=0;I<Catalog.Num();++I){auto& R=State->MissionStates[I];if(R.State!=EMainMissionState::Locked)continue;bool Ready=true;for(FName P:Catalog[I].Prerequisites)Ready&=GetMissionState(P)==EMainMissionState::Completed;if(Ready)R.State=EMainMissionState::Available;}
    if(const auto* Next=State->MissionStates.FindByPredicate([](const auto& R){return R.State==EMainMissionState::Available||R.State==EMainMissionState::Active;})){State->CurrentMission=Next->MissionId;State->CurrentChapter=Catalog[FMainStoryMissionCatalog::FindIndex(Next->MissionId)].Chapter;}
}
void UMainStorySubsystem::ApplyMissionFacts(FName Id)
{
    if(Id==TEXT("Main.C01.03.TheTaking")){State->StoryFacts.Append({Facts::MarcDead,Facts::DeniseDead,Facts::EthanAbducted});}
    else if(Id==TEXT("Main.C01.04.Undertow")){State->WorldFacts.Append({TEXT("World.MoranEntered"),TEXT("World.DriftwoodBeachReached")});}
    else if(Id==TEXT("Main.C02.01.ShatteredCoast")){State->WorldFacts.Add(TEXT("World.MoranOpeningRouteKnown"));}
    else if(Id==TEXT("Main.C02.02.AShipToTake")){State->WorldFacts.Append({TEXT("World.GalleonCoveCleared"),TEXT("Ship.LaLiberacionOwned")});State->CurrentShip=TEXT("ship.la_liberacion");}
    else if(Id==TEXT("Main.C02.03.FirstWake")){State->WorldFacts.Add(TEXT("World.RexaEntered"));}
    else if(Id==TEXT("Main.C08.02.BrothersAlive")){State->StoryFacts.Add(Facts::EthanRecovered);}
    else if(Id==TEXT("Main.C10.04.DravenVoss")){if(!State->StoryFacts.Contains(Facts::DravenCaptured))State->StoryFacts.Add(Facts::DravenKilled);}
    else if(Id==TEXT("Main.C10.05.TheWakeAfter")){State->StoryFacts.Add(Facts::MainComplete);State->WorldFacts.Add(Facts::MainComplete);State->WorldFacts.Add(TEXT("Story.CreditsReachable"));}
}
bool UMainStorySubsystem::SetWorldFact(FName Fact,bool Enabled){if(!State||Fact.IsNone())return false;TSet<FName> Copy=State->WorldFacts;Enabled?Copy.Add(Fact):Copy.Remove(Fact);UDarkArisenSaveGame* Temp=DuplicateObject<UDarkArisenSaveGame>(State,this);Temp->WorldFacts=Copy;TArray<FString>E;if(!ValidateState(Temp,E))return false;State->WorldFacts=MoveTemp(Copy);return true;}
bool UMainStorySubsystem::RecruitCrew(FName Id,FName Role){if(!State||Id.IsNone())return false;auto* C=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});if(!C){FCrewRelationshipState N;N.CharacterId=Id;State->Crew.Add(N);C=&State->Crew.Last();}C->bRecruited=true;C->bAboard=State->WorldFacts.Contains(TEXT("Ship.LaLiberacionOwned"));C->Trust=FMath::Max(C->Trust,10);State->QuestOutcomes.Add(FName(*(TEXT("CrewRole.")+Id.ToString())),Role);return true;}
bool UMainStorySubsystem::MarkBossDefeated(FName Id){static const TSet<FName> Valid={TEXT("boss.herrera"),TEXT("boss.reyes"),TEXT("boss.cruz"),TEXT("boss.de_silva"),TEXT("boss.vega"),TEXT("boss.blackwood"),TEXT("boss.sterling"),TEXT("boss.ashcroft"),TEXT("boss.thorne")};if(!State||!Valid.Contains(Id))return false;State->DefeatedBosses.Add(Id);return true;}
bool UMainStorySubsystem::SetCheckpoint(FName C,FName Spawn){if(!State||C.IsNone()||Spawn.IsNone())return false;State->CheckpointId=C;State->SpawnId=Spawn;return true;}
bool UMainStorySubsystem::SetOpeningProgress(const FOpeningProgressState& Progress)
{
    // Enum ordinals are serialized deliberately so this authority does not depend on a world component.
    if(!State||Progress.Location>11||Progress.RaidState>7||Progress.RecoveryState>5)return false;
    State->OpeningProgress=Progress;
    return true;
}
bool UMainStorySubsystem::HasStoryFact(FName F)const{return State&&State->StoryFacts.Contains(F);} bool UMainStorySubsystem::HasWorldFact(FName F)const{return State&&State->WorldFacts.Contains(F);}
bool UMainStorySubsystem::Validate(TArray<FString>& E)const{return ValidateState(State,E);}
bool UMainStorySubsystem::ValidateState(const UDarkArisenSaveGame* S,TArray<FString>& E)const
{
    E.Reset();TArray<FString> CatalogErrors;FMainStoryMissionCatalog::Validate(CatalogErrors);E.Append(CatalogErrors);if(!S){E.Add(TEXT("Missing runtime state."));return false;}if(S->SaveVersion!=UDarkArisenSaveGame::CurrentVersion)E.Add(TEXT("Save requires migration."));if(S->MissionStates.Num()!=34)E.Add(TEXT("Save mission count invalid."));
    auto SF=[S](FName F){return S->StoryFacts.Contains(F);};auto WF=[S](FName F){return S->WorldFacts.Contains(F);};
    if(SF(Facts::EthanRecovered)&&!SF(Facts::EthanAbducted))E.Add(TEXT("Ethan cannot be recovered before abduction."));if(SF(Facts::DravenKilled)&&SF(Facts::DravenCaptured))E.Add(TEXT("Draven outcomes are mutually exclusive."));if(WF(TEXT("Ship.LaLiberacionOwned"))&&S->CurrentChapter<2)E.Add(TEXT("La Liberacion cannot be owned before chapter 2."));auto CandidateMission=[S](FName Id){const auto* R=S->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});return R?R->State:EMainMissionState::Locked;};
    bool FoundIncomplete=false;for(const auto& R:S->MissionStates){if(R.State==EMainMissionState::Completed){if(FoundIncomplete)E.Add(TEXT("Completed mission appears after an incomplete predecessor."));}else if(R.State!=EMainMissionState::Failed)FoundIncomplete=true;}
    if(S->OpeningProgress.Location>11||S->OpeningProgress.RaidState>7||S->OpeningProgress.RecoveryState>5)E.Add(TEXT("Opening route state is invalid."));
    if(SF(Facts::MainComplete)&&CandidateMission(TEXT("Main.C10.05.TheWakeAfter"))!=EMainMissionState::Completed)E.Add(TEXT("Main completion requires the finale."));return E.IsEmpty();
}
