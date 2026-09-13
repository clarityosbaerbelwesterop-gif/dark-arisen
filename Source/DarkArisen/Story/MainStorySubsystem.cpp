#include "Story/MainStorySubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Story/MainStoryMissionCatalog.h"

namespace Facts
{
const FName MarcDead(TEXT("Story.MarcDead"));
const FName DeniseDead(TEXT("Story.DeniseDead"));
const FName EthanAbducted(TEXT("Story.EthanAbducted"));
const FName EthanRecovered(TEXT("Story.EthanRecovered"));
const FName MainComplete(TEXT("Story.MainComplete"));
const FName DravenKilled(TEXT("Story.DravenKilled"));
const FName DravenCaptured(TEXT("Story.DravenCaptured"));
}

namespace
{
const TArray<FName>& OpeningCrewIds()
{
    static const TArray<FName> Ids = {TEXT("crew.mira"), TEXT("crew.big_tom"), TEXT("crew.esteban")};
    return Ids;
}

bool IsOpeningProgressValid(const FOpeningProgressState& Progress)
{
    if (Progress.Location > 11 || Progress.RaidState > 8 || Progress.RecoveryState > 5) return false;
    if (Progress.BoardersRequired < 0 || Progress.BoardersDefeated < 0 ||
        Progress.BoardersDefeated > Progress.BoardersRequired) return false;
    if (Progress.BoardersRequired == 1) return false;
    if (Progress.BoardersRequired == 0 &&
        (!Progress.BoardingEncounterId.IsNone() || Progress.BoardersDefeated != 0 || Progress.bDravenBoarded)) return false;
    if (Progress.BoardersRequired >= 2 && Progress.BoardingEncounterId.IsNone()) return false;
    if (Progress.bDravenBoarded && Progress.BoardersDefeated < Progress.BoardersRequired) return false;
    if (Progress.bLaLiberacionHelmSecured && !Progress.bLaLiberacionBoarded) return false;
    if (Progress.bLaLiberacionClearedHarbor && !Progress.bLaLiberacionHelmSecured) return false;
    return true;
}
}

void UMainStorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ResetForNewGame();
}

void UMainStorySubsystem::NewGame(){ResetForNewGame();}

void UMainStorySubsystem::ResetForNewGame()
{
    State=NewObject<UDarkArisenSaveGame>(this);
    State->SaveVersion=UDarkArisenSaveGame::CurrentVersion;
    for(const auto& Def:FMainStoryMissionCatalog::Get())
    {
        FMainStoryMissionRuntime Runtime;
        Runtime.MissionId=Def.MissionId;
        State->MissionStates.Add(Runtime);
    }
    State->MissionStates[0].State=EMainMissionState::Available;
    State->CurrentMission=State->MissionStates[0].MissionId;
    State->CurrentChapter=1;
}

bool UMainStorySubsystem::Save(const FString& Slot,const int32 User)
{
    TArray<FString> Errors;
    return Validate(Errors)&&UGameplayStatics::SaveGameToSlot(State,Slot,User);
}

bool UMainStorySubsystem::Load(const FString& Slot,const int32 User)
{
    UDarkArisenSaveGame* Loaded=Cast<UDarkArisenSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,User));
    TArray<FString> Errors;
    if(!Loaded||!MigrateVersion(Loaded,Errors)||!ValidateState(Loaded,Errors))return false;
    State=Loaded;
    RefreshAvailability();
    return true;
}

bool UMainStorySubsystem::MigrateVersion(UDarkArisenSaveGame* Candidate,TArray<FString>& Errors) const
{
    if(!Candidate){Errors.Add(TEXT("Missing save object."));return false;}
    if(Candidate->SaveVersion<=0||Candidate->SaveVersion>UDarkArisenSaveGame::CurrentVersion)
    {Errors.Add(TEXT("Unsupported save version."));return false;}
    if(Candidate->SaveVersion==1)
    {
        for(FCrewRelationshipState& Crew:Candidate->Crew)
        {
            if(!Crew.bRecruited)continue;
            Crew.bMet=true;
            Crew.bAvailable=true;
            if(Crew.Role.IsNone())
            {
                const FName RoleKey(*FString::Printf(TEXT("CrewRole.%s"),*Crew.CharacterId.ToString()));
                if(const FName* LegacyRole=Candidate->QuestOutcomes.Find(RoleKey))Crew.Role=*LegacyRole;
            }
        }
        Candidate->SaveVersion=2;
    }
    Candidate->SaveVersion=UDarkArisenSaveGame::CurrentVersion;
    return true;
}

EMainMissionState UMainStorySubsystem::GetMissionState(const FName Id) const
{
    if(!State)return EMainMissionState::Locked;
    const auto* Runtime=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});
    return Runtime?Runtime->State:EMainMissionState::Locked;
}

bool UMainStorySubsystem::ActivateMission(const FName Id)
{
    if(!State)return false;
    auto* Runtime=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});
    if(!Runtime||Runtime->State!=EMainMissionState::Available)return false;
    for(const auto& Other:State->MissionStates)if(Other.State==EMainMissionState::Active)return false;
    const int32 Index=FMainStoryMissionCatalog::FindIndex(Id);
    if(Index==INDEX_NONE)return false;
    Runtime->State=EMainMissionState::Active;
    State->CurrentMission=Id;
    State->CurrentChapter=FMainStoryMissionCatalog::Get()[Index].Chapter;
    OnMissionChanged.Broadcast(Id,Runtime->State);
    return true;
}

bool UMainStorySubsystem::CompleteMission(const FName Id)
{
    if(!State)return false;
    auto* Runtime=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});
    if(!Runtime||Runtime->State!=EMainMissionState::Active)return false;
    Runtime->State=EMainMissionState::Completed;
    ApplyMissionFacts(Id);
    RefreshAvailability();
    OnMissionChanged.Broadcast(Id,Runtime->State);
    return true;
}

bool UMainStorySubsystem::FailMission(const FName Id)
{
    if(!State)return false;
    auto* Runtime=State->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});
    if(!Runtime||Runtime->State!=EMainMissionState::Active)return false;
    Runtime->State=EMainMissionState::Failed;
    OnMissionChanged.Broadcast(Id,Runtime->State);
    return true;
}

void UMainStorySubsystem::RefreshAvailability()
{
    const auto& Catalog=FMainStoryMissionCatalog::Get();
    for(int32 I=0;I<Catalog.Num();++I)
    {
        auto& Runtime=State->MissionStates[I];
        if(Runtime.State!=EMainMissionState::Locked)continue;
        bool Ready=true;
        for(FName P:Catalog[I].Prerequisites)Ready&=GetMissionState(P)==EMainMissionState::Completed;
        if(Ready)Runtime.State=EMainMissionState::Available;
    }
    if(const auto* Next=State->MissionStates.FindByPredicate([](const auto& R){return R.State==EMainMissionState::Available||R.State==EMainMissionState::Active;}))
    {
        State->CurrentMission=Next->MissionId;
        const int32 Index=FMainStoryMissionCatalog::FindIndex(Next->MissionId);
        if(Index!=INDEX_NONE)State->CurrentChapter=Catalog[Index].Chapter;
    }
}

void UMainStorySubsystem::ApplyMissionFacts(const FName Id)
{
    if(Id==TEXT("Main.C01.03.TheTaking"))State->StoryFacts.Append({Facts::MarcDead,Facts::DeniseDead,Facts::EthanAbducted});
    else if(Id==TEXT("Main.C01.04.Undertow"))State->WorldFacts.Append({TEXT("World.MoranEntered"),TEXT("World.DriftwoodBeachReached")});
    else if(Id==TEXT("Main.C02.01.ShatteredCoast"))State->WorldFacts.Add(TEXT("World.MoranOpeningRouteKnown"));
    else if(Id==TEXT("Main.C02.02.AShipToTake"))
    {
        State->WorldFacts.Append({TEXT("World.GalleonCoveCleared"),TEXT("Ship.LaLiberacionOwned")});
        State->CurrentShip=TEXT("ship.la_liberacion");
        for(FCrewRelationshipState& Crew:State->Crew)if(Crew.bRecruited)Crew.bAboard=true;
    }
    else if(Id==TEXT("Main.C02.03.FirstWake"))State->WorldFacts.Add(TEXT("World.RexaEntered"));
    else if(Id==TEXT("Main.C08.02.BrothersAlive"))State->StoryFacts.Add(Facts::EthanRecovered);
    else if(Id==TEXT("Main.C10.04.DravenVoss")){if(!State->StoryFacts.Contains(Facts::DravenCaptured))State->StoryFacts.Add(Facts::DravenKilled);}
    else if(Id==TEXT("Main.C10.05.TheWakeAfter")){State->StoryFacts.Add(Facts::MainComplete);State->WorldFacts.Add(Facts::MainComplete);State->WorldFacts.Add(TEXT("Story.CreditsReachable"));}
}

bool UMainStorySubsystem::SetWorldFact(const FName Fact,const bool Enabled)
{
    if(!State||Fact.IsNone())return false;
    TSet<FName> Copy=State->WorldFacts;
    Enabled?Copy.Add(Fact):Copy.Remove(Fact);
    UDarkArisenSaveGame* Temp=DuplicateObject<UDarkArisenSaveGame>(State,this);
    Temp->WorldFacts=Copy;
    TArray<FString> Errors;
    if(!ValidateState(Temp,Errors))return false;
    State->WorldFacts=MoveTemp(Copy);
    return true;
}

bool UMainStorySubsystem::MeetCrew(const FName Id)
{
    if(!State||Id.IsNone())return false;
    auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    if(!Crew)
    {
        FCrewRelationshipState NewEntry;
        NewEntry.CharacterId=Id;
        NewEntry.bMet=true;
        State->Crew.Add(NewEntry);
        return true;
    }
    Crew->bMet=true;
    return true;
}

bool UMainStorySubsystem::SetCrewAvailable(const FName Id,const FName Role)
{
    if(!State||Id.IsNone()||Role.IsNone())return false;
    auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    if(!Crew||!Crew->bMet||Crew->bRecruited)return false;
    Crew->bAvailable=true;
    Crew->Role=Role;
    State->QuestOutcomes.Add(FName(*FString::Printf(TEXT("CrewRole.%s"),*Id.ToString())),Role);
    return true;
}

bool UMainStorySubsystem::RecruitCrew(const FName Id,const FName Role)
{
    if(!State||Id.IsNone()||Role.IsNone())return false;
    auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    if(!Crew||!Crew->bMet||!Crew->bAvailable||Crew->bRecruited)return false;
    if(!Crew->Role.IsNone()&&Crew->Role!=Role)return false;
    Crew->Role=Role;
    Crew->bRecruited=true;
    Crew->bAboard=State->WorldFacts.Contains(TEXT("Ship.LaLiberacionOwned"));
    Crew->Trust=FMath::Max(Crew->Trust,10);
    State->QuestOutcomes.Add(FName(*FString::Printf(TEXT("CrewRole.%s"),*Id.ToString())),Role);
    return true;
}

bool UMainStorySubsystem::SetCrewAboard(const FName Id,const bool bAboard)
{
    if(!State||Id.IsNone())return false;
    auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    if(!Crew||!Crew->bRecruited)return false;
    if(bAboard&&!State->WorldFacts.Contains(TEXT("Ship.LaLiberacionOwned")))return false;
    Crew->bAboard=bAboard;
    return true;
}

bool UMainStorySubsystem::IsCrewMet(const FName Id) const
{
    if(!State)return false;
    const auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    return Crew&&Crew->bMet;
}

bool UMainStorySubsystem::IsCrewAvailable(const FName Id) const
{
    if(!State)return false;
    const auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    return Crew&&Crew->bAvailable;
}

bool UMainStorySubsystem::IsCrewRecruited(const FName Id) const
{
    if(!State)return false;
    const auto* Crew=State->Crew.FindByPredicate([Id](const auto& E){return E.CharacterId==Id;});
    return Crew&&Crew->bRecruited;
}

bool UMainStorySubsystem::AreOpeningCrewRecruited() const
{
    for(const FName Id:OpeningCrewIds())if(!IsCrewRecruited(Id))return false;
    return true;
}

bool UMainStorySubsystem::MarkBossDefeated(const FName Id)
{
    static const TSet<FName> Valid={TEXT("boss.herrera"),TEXT("boss.reyes"),TEXT("boss.cruz"),TEXT("boss.de_silva"),TEXT("boss.vega"),TEXT("boss.blackwood"),TEXT("boss.sterling"),TEXT("boss.ashcroft"),TEXT("boss.thorne")};
    if(!State||!Valid.Contains(Id))return false;
    State->DefeatedBosses.Add(Id);
    return true;
}

bool UMainStorySubsystem::SetCheckpoint(const FName Checkpoint,const FName Spawn)
{
    if(!State||Checkpoint.IsNone()||Spawn.IsNone())return false;
    State->CheckpointId=Checkpoint;
    State->SpawnId=Spawn;
    return true;
}

bool UMainStorySubsystem::SetOpeningProgress(const FOpeningProgressState& Progress)
{
    if(!State||!IsOpeningProgressValid(Progress))return false;
    State->OpeningProgress=Progress;
    return true;
}

bool UMainStorySubsystem::HasStoryFact(const FName Fact) const{return State&&State->StoryFacts.Contains(Fact);}
bool UMainStorySubsystem::HasWorldFact(const FName Fact) const{return State&&State->WorldFacts.Contains(Fact);}
bool UMainStorySubsystem::Validate(TArray<FString>& Errors) const{return ValidateState(State,Errors);}

bool UMainStorySubsystem::ValidateState(const UDarkArisenSaveGame* Candidate,TArray<FString>& Errors) const
{
    Errors.Reset();
    TArray<FString> CatalogErrors;
    FMainStoryMissionCatalog::Validate(CatalogErrors);
    Errors.Append(CatalogErrors);
    if(!Candidate){Errors.Add(TEXT("Missing runtime state."));return false;}
    if(Candidate->SaveVersion!=UDarkArisenSaveGame::CurrentVersion)Errors.Add(TEXT("Save requires migration."));
    if(Candidate->MissionStates.Num()!=34)Errors.Add(TEXT("Save mission count invalid."));

    const auto SF=[Candidate](const FName F){return Candidate->StoryFacts.Contains(F);};
    const auto WF=[Candidate](const FName F){return Candidate->WorldFacts.Contains(F);};
    if(SF(Facts::EthanRecovered)&&!SF(Facts::EthanAbducted))Errors.Add(TEXT("Ethan cannot be recovered before abduction."));
    if(SF(Facts::DravenKilled)&&SF(Facts::DravenCaptured))Errors.Add(TEXT("Draven outcomes are mutually exclusive."));
    if(WF(TEXT("Ship.LaLiberacionOwned"))&&Candidate->CurrentChapter<2)Errors.Add(TEXT("La Liberacion cannot be owned before chapter 2."));

    const auto Mission=[Candidate](const FName Id)
    {
        const auto* Runtime=Candidate->MissionStates.FindByPredicate([Id](const auto& V){return V.MissionId==Id;});
        return Runtime?Runtime->State:EMainMissionState::Locked;
    };

    bool FoundIncomplete=false;
    for(const auto& Runtime:Candidate->MissionStates)
    {
        if(Runtime.State==EMainMissionState::Completed){if(FoundIncomplete)Errors.Add(TEXT("Completed mission appears after an incomplete predecessor."));}
        else if(Runtime.State!=EMainMissionState::Failed)FoundIncomplete=true;
    }

    for(const FCrewRelationshipState& Crew:Candidate->Crew)
    {
        if(Crew.CharacterId.IsNone())Errors.Add(TEXT("Crew entry has no character id."));
        if(Crew.bAvailable&&!Crew.bMet)Errors.Add(TEXT("Crew cannot be available before being met."));
        if(Crew.bRecruited&&(!Crew.bMet||!Crew.bAvailable||Crew.Role.IsNone()))
            Errors.Add(TEXT("Crew recruitment is missing an authored meeting, availability beat, or role."));
        if(Crew.bAboard&&!Crew.bRecruited)Errors.Add(TEXT("Non-recruited crew cannot be aboard."));
    }

    if(!IsOpeningProgressValid(Candidate->OpeningProgress))Errors.Add(TEXT("Opening route state is invalid."));
    if(SF(Facts::MainComplete)&&Mission(TEXT("Main.C10.05.TheWakeAfter"))!=EMainMissionState::Completed)
        Errors.Add(TEXT("Main completion requires the finale."));
    return Errors.IsEmpty();
}
