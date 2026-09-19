#include "Opening/OpeningRuntimeComponent.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "JakeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Story/MainStorySubsystem.h"

namespace
{
bool IsOpeningCrew(const FName Id)
{
    return Id==TEXT("crew.mira")||Id==TEXT("crew.big_tom")||Id==TEXT("crew.esteban");
}

FName OpeningCrewRole(const FName Id)
{
    if(Id==TEXT("crew.mira"))return TEXT("FirstMate");
    if(Id==TEXT("crew.big_tom"))return TEXT("Smith");
    if(Id==TEXT("crew.esteban"))return TEXT("Navigator");
    return NAME_None;
}
}

UOpeningRuntimeComponent::UOpeningRuntimeComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UOpeningRuntimeComponent::BeginPlay(){Super::BeginPlay();RestoreProgress();}

UMainStorySubsystem* UOpeningRuntimeComponent::Story() const
{
    return GetWorld()&&GetWorld()->GetGameInstance()
        ?GetWorld()->GetGameInstance()->GetSubsystem<UMainStorySubsystem>():nullptr;
}

bool UOpeningRuntimeComponent::SignalFamilyInteraction(const FName Id)
{
    if(Location!=EOpeningLocation::FamilyMerchantShip||RaidState!=EOpeningRaidState::Peaceful||
       !(Id==TEXT("character.marc")||Id==TEXT("character.denise")||Id==TEXT("character.ethan")))return false;
    FamilyInteractions.Add(Id);
    if(!CommitProgress())return false;
    if(FamilyInteractions.Num()==3)
    {
        if(auto* S=Story())
        {
            if(S->GetMissionState(TEXT("Main.C01.01.HomeWater"))==EMainMissionState::Available)
                S->ActivateMission(TEXT("Main.C01.01.HomeWater"));
            if(S->GetMissionState(TEXT("Main.C01.01.HomeWater"))==EMainMissionState::Active)
                S->CompleteMission(TEXT("Main.C01.01.HomeWater"));
        }
        OnJournalObservation.Broadcast(TEXT("Journal.HomeWater.FamilyRoutes"));
    }
    return true;
}

bool UOpeningRuntimeComponent::SignalFleetDetected()
{
    if(RaidState!=EOpeningRaidState::Peaceful||FamilyInteractions.Num()<3)return false;
    const auto Previous=RaidState;
    RaidState=EOpeningRaidState::FleetDetected;
    if(!CommitProgress()){RaidState=Previous;return false;}
    if(auto* S=Story())
    {
        if(S->GetMissionState(TEXT("Main.C01.02.BlackSails"))==EMainMissionState::Available)
            S->ActivateMission(TEXT("Main.C01.02.BlackSails"));
    }
    OnCinematicRequested.Broadcast(TEXT("Cinematic.Opening.BlackSailsSighted"));
    return true;
}

bool UOpeningRuntimeComponent::SignalBoardingStarted()
{
    if(RaidState!=EOpeningRaidState::FleetDetected)return false;
    const auto Previous=RaidState;
    RaidState=EOpeningRaidState::Boarding;
    if(CommitProgress())return true;
    RaidState=Previous;
    return false;
}

bool UOpeningRuntimeComponent::BeginBoardingEncounter(const FName EncounterId,const int32 RequiredBoarders)
{
    if(RaidState!=EOpeningRaidState::Boarding||EncounterId.IsNone()||RequiredBoarders<2)return false;
    const auto PreviousState=RaidState;
    const FName PreviousId=BoardingEncounterId;
    const int32 PreviousRequired=BoardersRequired;
    RaidState=EOpeningRaidState::FirstEncounter;
    BoardingEncounterId=EncounterId;
    BoardersRequired=RequiredBoarders;
    BoardersDefeated=0;
    bDravenBoarded=false;
    if(CommitProgress())return true;
    RaidState=PreviousState;
    BoardingEncounterId=PreviousId;
    BoardersRequired=PreviousRequired;
    return false;
}

bool UOpeningRuntimeComponent::SignalBoarderDefeated(const FName EncounterId)
{
    if(RaidState!=EOpeningRaidState::FirstEncounter||EncounterId.IsNone()||
       EncounterId!=BoardingEncounterId||BoardersRequired<2||BoardersDefeated>=BoardersRequired)return false;
    ++BoardersDefeated;
    if(CommitProgress())return true;
    --BoardersDefeated;
    return false;
}

bool UOpeningRuntimeComponent::SignalFirstBoarderDefeated()
{
    return SignalBoarderDefeated(BoardingEncounterId);
}

bool UOpeningRuntimeComponent::SignalDravenBoarded(const FName EncounterId)
{
    if(RaidState!=EOpeningRaidState::FirstEncounter||EncounterId!=BoardingEncounterId||
       BoardersRequired<2||BoardersDefeated<BoardersRequired)return false;
    RaidState=EOpeningRaidState::DravenAboard;
    bDravenBoarded=true;
    if(!CommitProgress())
    {
        RaidState=EOpeningRaidState::FirstEncounter;
        bDravenBoarded=false;
        return false;
    }
    OnCinematicRequested.Broadcast(TEXT("Presentation.Opening.DravenBoards"));
    return true;
}

bool UOpeningRuntimeComponent::SignalTakingStarted()
{
    if(RaidState!=EOpeningRaidState::DravenAboard||!bDravenBoarded)return false;
    auto* S=Story();
    if(!S||S->GetMissionState(TEXT("Main.C01.02.BlackSails"))!=EMainMissionState::Active)return false;
    if(!S->CompleteMission(TEXT("Main.C01.02.BlackSails")))return false;
    if(!S->ActivateMission(TEXT("Main.C01.03.TheTaking")))return false;
    RaidState=EOpeningRaidState::Taking;
    if(!CommitProgress())return false;
    OnCinematicRequested.Broadcast(TEXT("Cinematic.Opening.TheTaking"));
    return true;
}

bool UOpeningRuntimeComponent::SignalTakingCinematicComplete()
{
    if(RaidState!=EOpeningRaidState::Taking)return false;
    auto* S=Story();
    if(!S||S->GetMissionState(TEXT("Main.C01.03.TheTaking"))!=EMainMissionState::Active)return false;
    if(!S->CompleteMission(TEXT("Main.C01.03.TheTaking")))return false;
    if(!S->ActivateMission(TEXT("Main.C01.04.Undertow")))return false;
    RaidState=EOpeningRaidState::Overboard;
    RecoveryState=EWaterRecoveryState::Overboard;
    if(!CommitProgress())return false;
    OnCinematicRequested.Broadcast(TEXT("Cinematic.Opening.JakeOverboard"));
    return true;
}

bool UOpeningRuntimeComponent::SignalEnteredWater()
{
    if(RaidState!=EOpeningRaidState::Overboard||RecoveryState!=EWaterRecoveryState::Overboard||
       Location!=EOpeningLocation::FamilyMerchantShip)return false;
    RecoveryState=EWaterRecoveryState::Swimming;
    if(AdvanceLocation(EOpeningLocation::FamilyMerchantShip,EOpeningLocation::OpenWater))return true;
    RecoveryState=EWaterRecoveryState::Overboard;
    return false;
}

bool UOpeningRuntimeComponent::SignalReachedOuterReef()
{
    if(RecoveryState!=EWaterRecoveryState::Swimming)return false;
    RecoveryState=EWaterRecoveryState::OuterReef;
    if(AdvanceLocation(EOpeningLocation::OpenWater,EOpeningLocation::OuterReef,TEXT("World.OuterReefReached")))return true;
    RecoveryState=EWaterRecoveryState::Swimming;
    return false;
}

bool UOpeningRuntimeComponent::SignalReachedLocation(const EOpeningLocation NewLocation)
{
    if(NewLocation==EOpeningLocation::DriftwoodBeach&&Location==EOpeningLocation::OuterReef)
    {
        RecoveryState=EWaterRecoveryState::BeachCrawl;
        return AdvanceLocation(Location,NewLocation,TEXT("World.DriftwoodBeachReached"));
    }

    // Ship ownership, sea departure and Rexa arrival each have dedicated gameplay gates.
    if(NewLocation==EOpeningLocation::LaLiberacion||NewLocation==EOpeningLocation::OpenSea||
       NewLocation==EOpeningLocation::RexaHarbor)return false;

    const int32 Current=static_cast<int32>(Location);
    const int32 Next=static_cast<int32>(NewLocation);
    if(Next!=Current+1||NewLocation==EOpeningLocation::OpenWater||NewLocation==EOpeningLocation::OuterReef)return false;
    static const FName Facts[]={
        NAME_None,NAME_None,NAME_None,TEXT("World.DriftwoodBeachReached"),
        TEXT("World.DriftwoodCampReached"),TEXT("World.MirasCoveReached"),
        TEXT("World.MangroveShallowsReached"),TEXT("World.KoasTradingPostReached"),
        TEXT("World.GalleonCoveReached"),NAME_None,NAME_None,NAME_None};
    return AdvanceLocation(Location,NewLocation,Facts[Next]);
}

bool UOpeningRuntimeComponent::SignalRecoveryComplete()
{
    if(Location!=EOpeningLocation::DriftwoodBeach||RecoveryState!=EWaterRecoveryState::BeachCrawl)return false;
    RecoveryState=EWaterRecoveryState::Recovered;
    RaidState=EOpeningRaidState::Complete;
    if(!CommitProgress())return false;
    if(auto* S=Story())
    {
        S->SetWorldFact(TEXT("World.MoranEntered"));
        S->SetCheckpoint(TEXT("Checkpoint.Moran.DriftwoodBeach"),TEXT("Spawn.Moran.DriftwoodBeach.Recovery"));
        if(S->GetMissionState(TEXT("Main.C01.04.Undertow"))==EMainMissionState::Active)
            S->CompleteMission(TEXT("Main.C01.04.Undertow"));
        if(S->GetMissionState(TEXT("Main.C02.01.ShatteredCoast"))==EMainMissionState::Available)
            S->ActivateMission(TEXT("Main.C02.01.ShatteredCoast"));
    }
    OnJournalObservation.Broadcast(TEXT("Journal.DriftwoodBeach.SmokeBeyondWreckTimber"));
    return true;
}

bool UOpeningRuntimeComponent::SignalCrewMet(const FName CrewId)
{
    auto* S=Story();
    if(!S||!IsOpeningCrew(CrewId)||Location<EOpeningLocation::MirasCove||Location>EOpeningLocation::GalleonCove)return false;
    if(CrewId==TEXT("crew.mira")&&Location!=EOpeningLocation::MirasCove)return false;
    if(CrewId==TEXT("crew.esteban")&&Location!=EOpeningLocation::GalleonCove)return false;
    return S->MeetCrew(CrewId);
}

bool UOpeningRuntimeComponent::SignalCrewRecruitmentAvailable(const FName CrewId)
{
    auto* S=Story();
    const FName Role=OpeningCrewRole(CrewId);
    if(!S||Role.IsNone()||!S->IsCrewMet(CrewId))return false;
    return S->SetCrewAvailable(CrewId,Role);
}

bool UOpeningRuntimeComponent::SignalCrewRecruited(const FName CrewId)
{
    auto* S=Story();
    const FName Role=OpeningCrewRole(CrewId);
    if(!S||Role.IsNone()||!S->IsCrewAvailable(CrewId))return false;
    return S->RecruitCrew(CrewId,Role);
}

bool UOpeningRuntimeComponent::SignalGalleonCoveCleared()
{
    if(Location!=EOpeningLocation::GalleonCove)return false;
    auto* S=Story();
    if(!S||!S->SetWorldFact(TEXT("World.GalleonCoveCleared")))return false;
    if(S->GetMissionState(TEXT("Main.C02.01.ShatteredCoast"))==EMainMissionState::Active&&
       !S->CompleteMission(TEXT("Main.C02.01.ShatteredCoast")))return false;
    if(S->GetMissionState(TEXT("Main.C02.02.AShipToTake"))==EMainMissionState::Available&&
       !S->ActivateMission(TEXT("Main.C02.02.AShipToTake")))return false;
    return true;
}

bool UOpeningRuntimeComponent::ClaimLaLiberacion()
{
    return SignalLaLiberacionBoarded();
}

bool UOpeningRuntimeComponent::SignalLaLiberacionBoarded()
{
    auto* S=Story();
    if(!S||Location!=EOpeningLocation::GalleonCove||!S->HasWorldFact(TEXT("World.GalleonCoveCleared"))||
       S->GetMissionState(TEXT("Main.C02.02.AShipToTake"))!=EMainMissionState::Active||bLaLiberacionBoarded)return false;
    bLaLiberacionBoarded=true;
    if(CommitProgress())return true;
    bLaLiberacionBoarded=false;
    return false;
}

bool UOpeningRuntimeComponent::SignalLaLiberacionHelmSecured()
{
    auto* S=Story();
    if(!S||!bLaLiberacionBoarded||bLaLiberacionHelmSecured||!S->AreOpeningCrewRecruited())return false;
    bLaLiberacionHelmSecured=true;
    if(CommitProgress())return true;
    bLaLiberacionHelmSecured=false;
    return false;
}

bool UOpeningRuntimeComponent::SignalLaLiberacionHarborCleared()
{
    auto* S=Story();
    if(!S||Location!=EOpeningLocation::GalleonCove||!bLaLiberacionHelmSecured||
       S->GetMissionState(TEXT("Main.C02.02.AShipToTake"))!=EMainMissionState::Active)return false;
    bLaLiberacionClearedHarbor=true;
    if(!CommitProgress()){bLaLiberacionClearedHarbor=false;return false;}
    if(!S->CompleteMission(TEXT("Main.C02.02.AShipToTake")))return false;
    return AdvanceLocation(EOpeningLocation::GalleonCove,EOpeningLocation::LaLiberacion);
}

bool UOpeningRuntimeComponent::BeginFirstWake()
{
    auto* S=Story();
    if(!S||Location!=EOpeningLocation::LaLiberacion||!CanUseHelm()||
       S->GetMissionState(TEXT("Main.C02.03.FirstWake"))!=EMainMissionState::Available)return false;
    if(!S->ActivateMission(TEXT("Main.C02.03.FirstWake")))return false;
    return AdvanceLocation(EOpeningLocation::LaLiberacion,EOpeningLocation::OpenSea,
        TEXT("Route.Moran.GalleonCove.OpenSea.RexaHarbor"));
}

bool UOpeningRuntimeComponent::ReachRexaHarbor()
{
    auto* S=Story();
    if(!S||Location!=EOpeningLocation::OpenSea||
       S->GetMissionState(TEXT("Main.C02.03.FirstWake"))!=EMainMissionState::Active)return false;
    if(!AdvanceLocation(EOpeningLocation::OpenSea,EOpeningLocation::RexaHarbor,TEXT("World.RexaEntered")))return false;
    return S->CompleteMission(TEXT("Main.C02.03.FirstWake"));
}

bool UOpeningRuntimeComponent::RecoverAtCheckpoint(AActor* Jake)
{
    auto* S=Story();
    const UDarkArisenSaveGame* State=S?S->GetState():nullptr;
    AJakeCharacter* JakeCharacter=Cast<AJakeCharacter>(Jake);
    if(!JakeCharacter||!State||State->CheckpointId.IsNone()||State->SpawnId.IsNone())return false;

    TArray<AActor*> Anchors;
    UGameplayStatics::GetAllActorsWithTag(this,State->SpawnId,Anchors);
    if(Anchors.Num()!=1||!IsValid(Anchors[0]))return false;
    if(!JakeCharacter->RestoreAtCheckpoint(Anchors[0]->GetActorTransform()))return false;
    OnCheckpointRecovered.Broadcast(State->CheckpointId,State->SpawnId);
    return true;
}

bool UOpeningRuntimeComponent::CanUseHelm() const
{
    const auto* S=Story();
    return S&&bLaLiberacionHelmSecured&&S->AreOpeningCrewRecruited()&&
        (Location==EOpeningLocation::GalleonCove||Location>=EOpeningLocation::LaLiberacion);
}

bool UOpeningRuntimeComponent::AdvanceLocation(
    const EOpeningLocation Expected,const EOpeningLocation Next,const FName Fact)
{
    if(Location!=Expected)return false;
    const auto Old=Location;
    Location=Next;
    if(!Fact.IsNone())
    {
        if(auto* S=Story())
        {
            if(!S->SetWorldFact(Fact)){Location=Old;return false;}
        }
        else{Location=Old;return false;}
    }
    if(!CommitProgress()){Location=Old;return false;}
    OnLocationChanged.Broadcast(Old,Location);
    return true;
}

void UOpeningRuntimeComponent::RestoreProgress()
{
    const auto* S=Story();
    if(!S||!S->GetState())return;
    const auto& P=S->GetState()->OpeningProgress;
    Location=static_cast<EOpeningLocation>(P.Location);
    RaidState=static_cast<EOpeningRaidState>(P.RaidState);
    RecoveryState=static_cast<EWaterRecoveryState>(P.RecoveryState);
    FamilyInteractions=P.FamilyInteractions;
    BoardingEncounterId=P.BoardingEncounterId;
    BoardersRequired=P.BoardersRequired;
    BoardersDefeated=P.BoardersDefeated;
    bDravenBoarded=P.bDravenBoarded;
    bLaLiberacionBoarded=P.bLaLiberacionBoarded;
    bLaLiberacionHelmSecured=P.bLaLiberacionHelmSecured;
    bLaLiberacionClearedHarbor=P.bLaLiberacionClearedHarbor;
}

bool UOpeningRuntimeComponent::CommitProgress()
{
    auto* S=Story();
    if(!S)return false;
    FOpeningProgressState P;
    P.Location=static_cast<uint8>(Location);
    P.RaidState=static_cast<uint8>(RaidState);
    P.RecoveryState=static_cast<uint8>(RecoveryState);
    P.FamilyInteractions=FamilyInteractions;
    P.BoardingEncounterId=BoardingEncounterId;
    P.BoardersRequired=BoardersRequired;
    P.BoardersDefeated=BoardersDefeated;
    P.bDravenBoarded=bDravenBoarded;
    P.bLaLiberacionBoarded=bLaLiberacionBoarded;
    P.bLaLiberacionHelmSecured=bLaLiberacionHelmSecured;
    P.bLaLiberacionClearedHarbor=bLaLiberacionClearedHarbor;
    return S->SetOpeningProgress(P);
}
