#include "Opening/OpeningRuntimeComponent.h"
#include "GameFramework/Actor.h"
#include "Story/MainStorySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UOpeningRuntimeComponent::UOpeningRuntimeComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UOpeningRuntimeComponent::BeginPlay(){Super::BeginPlay();RestoreProgress();}
UMainStorySubsystem* UOpeningRuntimeComponent::Story()const{return GetWorld()&&GetWorld()->GetGameInstance()?GetWorld()->GetGameInstance()->GetSubsystem<UMainStorySubsystem>():nullptr;}
bool UOpeningRuntimeComponent::SignalFamilyInteraction(FName Id){if(Location!=EOpeningLocation::FamilyMerchantShip||RaidState!=EOpeningRaidState::Peaceful||!(Id==TEXT("character.marc")||Id==TEXT("character.denise")||Id==TEXT("character.ethan")))return false;FamilyInteractions.Add(Id);if(!CommitProgress())return false;if(FamilyInteractions.Num()==3){if(auto*S=Story()){S->ActivateMission(TEXT("Main.C01.01.HomeWater"));S->CompleteMission(TEXT("Main.C01.01.HomeWater"));}OnJournalObservation.Broadcast(TEXT("Journal.HomeWater.FamilyRoutes"));}return true;}
bool UOpeningRuntimeComponent::SignalFleetDetected(){if(RaidState!=EOpeningRaidState::Peaceful||FamilyInteractions.Num()<3)return false;RaidState=EOpeningRaidState::FleetDetected;if(!CommitProgress()){RaidState=EOpeningRaidState::Peaceful;return false;}if(auto*S=Story())S->ActivateMission(TEXT("Main.C01.02.BlackSails"));OnCinematicRequested.Broadcast(TEXT("Cinematic.Opening.BlackSailsSighted"));return true;}
bool UOpeningRuntimeComponent::SignalBoardingStarted(){if(RaidState!=EOpeningRaidState::FleetDetected)return false;RaidState=EOpeningRaidState::Boarding;return CommitProgress();}
bool UOpeningRuntimeComponent::SignalFirstBoarderDefeated(){if(RaidState!=EOpeningRaidState::Boarding&&RaidState!=EOpeningRaidState::FirstEncounter)return false;RaidState=EOpeningRaidState::Taking;if(!CommitProgress())return false;if(auto*S=Story()){S->CompleteMission(TEXT("Main.C01.02.BlackSails"));S->ActivateMission(TEXT("Main.C01.03.TheTaking"));}return true;}
bool UOpeningRuntimeComponent::SignalTakingCinematicComplete(){if(RaidState!=EOpeningRaidState::Taking)return false;RaidState=EOpeningRaidState::Overboard;if(!CommitProgress())return false;if(auto*S=Story()){S->CompleteMission(TEXT("Main.C01.03.TheTaking"));S->ActivateMission(TEXT("Main.C01.04.Undertow"));}OnCinematicRequested.Broadcast(TEXT("Cinematic.Opening.JakeOverboard"));return true;}
bool UOpeningRuntimeComponent::SignalEnteredWater(){if(RaidState!=EOpeningRaidState::Overboard||Location!=EOpeningLocation::FamilyMerchantShip)return false;RecoveryState=EWaterRecoveryState::Swimming;if(AdvanceLocation(EOpeningLocation::FamilyMerchantShip,EOpeningLocation::OpenWater))return true;RecoveryState=EWaterRecoveryState::Dry;return false;}
bool UOpeningRuntimeComponent::SignalReachedOuterReef(){if(RecoveryState!=EWaterRecoveryState::Swimming)return false;RecoveryState=EWaterRecoveryState::OuterReef;if(AdvanceLocation(EOpeningLocation::OpenWater,EOpeningLocation::OuterReef,TEXT("World.OuterReefReached")))return true;RecoveryState=EWaterRecoveryState::Swimming;return false;}
bool UOpeningRuntimeComponent::SignalReachedLocation(EOpeningLocation NewLocation)
{
    if(NewLocation==EOpeningLocation::DriftwoodBeach&&Location==EOpeningLocation::OuterReef){RecoveryState=EWaterRecoveryState::BeachCrawl;return AdvanceLocation(Location,NewLocation,TEXT("World.DriftwoodBeachReached"));}
    const int32 Current=static_cast<int32>(Location),Next=static_cast<int32>(NewLocation);if(Next!=Current+1||NewLocation==EOpeningLocation::OpenWater||NewLocation==EOpeningLocation::OuterReef)return false;
    static const FName Facts[]={NAME_None,NAME_None,NAME_None,TEXT("World.DriftwoodBeachReached"),TEXT("World.DriftwoodCampReached"),TEXT("World.MirasCoveReached"),TEXT("World.MangroveShallowsReached"),TEXT("World.KoasTradingPostReached"),TEXT("World.GalleonCoveReached"),TEXT("Ship.LaLiberacionOwned"),TEXT("Route.Moran.GalleonCove.OpenSea.RexaHarbor"),TEXT("World.RexaEntered")};
    if(!AdvanceLocation(Location,NewLocation,Facts[Next]))return false;if(auto*S=Story()){if(NewLocation==EOpeningLocation::MirasCove)S->RecruitCrew(TEXT("crew.mira"),TEXT("FirstMate"));else if(NewLocation==EOpeningLocation::MangroveShallows)S->RecruitCrew(TEXT("crew.big_tom"),TEXT("Smith"));else if(NewLocation==EOpeningLocation::KoasTradingPost)S->RecruitCrew(TEXT("crew.esteban"),TEXT("Navigator"));}return true;
}
bool UOpeningRuntimeComponent::SignalRecoveryComplete(){if(Location!=EOpeningLocation::DriftwoodBeach||RecoveryState!=EWaterRecoveryState::BeachCrawl)return false;RecoveryState=EWaterRecoveryState::Recovered;RaidState=EOpeningRaidState::Complete;if(!CommitProgress())return false;if(auto*S=Story()){S->SetWorldFact(TEXT("World.MoranEntered"));S->SetCheckpoint(TEXT("Checkpoint.Moran.DriftwoodBeach"),TEXT("Spawn.Moran.DriftwoodBeach.Recovery"));S->CompleteMission(TEXT("Main.C01.04.Undertow"));S->ActivateMission(TEXT("Main.C02.01.ShatteredCoast"));}OnJournalObservation.Broadcast(TEXT("Journal.DriftwoodBeach.SmokeBeyondWreckTimber"));return true;}
bool UOpeningRuntimeComponent::SignalGalleonCoveCleared(){if(Location!=EOpeningLocation::GalleonCove)return false;if(auto*S=Story()){S->SetWorldFact(TEXT("World.GalleonCoveCleared"));return true;}return false;}
bool UOpeningRuntimeComponent::ClaimLaLiberacion(){auto*S=Story();if(!S||Location!=EOpeningLocation::GalleonCove||!S->HasWorldFact(TEXT("World.GalleonCoveCleared")))return false;S->CompleteMission(TEXT("Main.C02.01.ShatteredCoast"));S->ActivateMission(TEXT("Main.C02.02.AShipToTake"));if(!S->CompleteMission(TEXT("Main.C02.02.AShipToTake")))return false;return SignalReachedLocation(EOpeningLocation::LaLiberacion);}
bool UOpeningRuntimeComponent::BeginFirstWake(){auto*S=Story();if(!S||Location!=EOpeningLocation::LaLiberacion||!CanUseHelm())return false;if(!S->ActivateMission(TEXT("Main.C02.03.FirstWake")))return false;return SignalReachedLocation(EOpeningLocation::OpenSea);}
bool UOpeningRuntimeComponent::ReachRexaHarbor(){auto*S=Story();if(!S||Location!=EOpeningLocation::OpenSea)return false;if(!SignalReachedLocation(EOpeningLocation::RexaHarbor))return false;return S->CompleteMission(TEXT("Main.C02.03.FirstWake"));}
bool UOpeningRuntimeComponent::RecoverAtCheckpoint(AActor* Jake)const{const auto*S=Story();if(!Jake||!S||!S->GetState()||S->GetState()->SpawnId.IsNone())return false;OnCinematicRequested.Broadcast(TEXT("Presentation.DeathRecovery.Fade"));return true;}
bool UOpeningRuntimeComponent::CanUseHelm()const{const auto*S=Story();return S&&S->HasWorldFact(TEXT("Ship.LaLiberacionOwned"))&&Location>=EOpeningLocation::LaLiberacion;}
bool UOpeningRuntimeComponent::AdvanceLocation(EOpeningLocation Expected,EOpeningLocation Next,FName Fact){if(Location!=Expected)return false;const auto Old=Location;Location=Next;if(!Fact.IsNone())if(auto*S=Story())if(!S->SetWorldFact(Fact)){Location=Old;return false;}if(!CommitProgress()){Location=Old;return false;}OnLocationChanged.Broadcast(Old,Location);return true;}
void UOpeningRuntimeComponent::RestoreProgress()
{
    const auto*S=Story();if(!S||!S->GetState())return;const auto&P=S->GetState()->OpeningProgress;
    Location=static_cast<EOpeningLocation>(P.Location);RaidState=static_cast<EOpeningRaidState>(P.RaidState);
    RecoveryState=static_cast<EWaterRecoveryState>(P.RecoveryState);FamilyInteractions=P.FamilyInteractions;
}
bool UOpeningRuntimeComponent::CommitProgress()
{
    auto*S=Story();if(!S)return false;FOpeningProgressState P;P.Location=static_cast<uint8>(Location);
    P.RaidState=static_cast<uint8>(RaidState);P.RecoveryState=static_cast<uint8>(RecoveryState);P.FamilyInteractions=FamilyInteractions;
    return S->SetOpeningProgress(P);
}
