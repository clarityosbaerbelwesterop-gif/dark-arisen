#include "Opening/OpeningEventTriggerComponent.h"
#include "Story/DarkArisenWorldDirector.h"
bool UOpeningEventTriggerComponent::Execute(AActor* InstigatorActor)
{
 if(bOneShot&&bConsumed)return false;
 ADarkArisenWorldDirector* Director=ADarkArisenWorldDirector::Resolve(this);
 UOpeningRuntimeComponent* Opening=Director?Director->OpeningRuntime:nullptr;
 if(!Opening)return false;
 bool Result=false;
 switch(Action)
 {
  case EOpeningEventAction::FamilyInteraction:Result=Opening->SignalFamilyInteraction(EventId);break;
  case EOpeningEventAction::FleetDetected:Result=Opening->SignalFleetDetected();break;
  case EOpeningEventAction::BoardingStarted:Result=Opening->SignalBoardingStarted();break;
  case EOpeningEventAction::BeginBoardingEncounter:Result=Opening->BeginBoardingEncounter(EventId,RequiredBoarders);break;
  case EOpeningEventAction::BoarderDefeated:Result=Opening->SignalBoarderDefeated(EventId);break;
  case EOpeningEventAction::DravenBoarded:Result=Opening->SignalDravenBoarded(EventId);break;
  case EOpeningEventAction::TakingStarted:Result=Opening->SignalTakingStarted();break;
  case EOpeningEventAction::TakingPresentationComplete:Result=Opening->SignalTakingCinematicComplete();break;
  case EOpeningEventAction::EnteredWater:Result=Opening->SignalEnteredWater();break;
  case EOpeningEventAction::ReachedOuterReef:Result=Opening->SignalReachedOuterReef();break;
  case EOpeningEventAction::ReachedLocation:Result=Opening->SignalReachedLocation(TargetLocation);break;
  case EOpeningEventAction::RecoveryComplete:Result=Opening->SignalRecoveryComplete();break;
  case EOpeningEventAction::CrewMet:Result=Opening->SignalCrewMet(EventId);break;
  case EOpeningEventAction::CrewAvailable:Result=Opening->SignalCrewRecruitmentAvailable(EventId);break;
  case EOpeningEventAction::CrewRecruited:Result=Opening->SignalCrewRecruited(EventId);break;
  case EOpeningEventAction::GalleonCoveCleared:Result=Opening->SignalGalleonCoveCleared();break;
  case EOpeningEventAction::LaLiberacionBoarded:Result=Opening->SignalLaLiberacionBoarded();break;
  case EOpeningEventAction::LaLiberacionHelmSecured:Result=Opening->SignalLaLiberacionHelmSecured();break;
  case EOpeningEventAction::LaLiberacionHarborCleared:Result=Opening->SignalLaLiberacionHarborCleared();break;
  case EOpeningEventAction::BeginFirstWake:Result=Opening->BeginFirstWake();break;
  case EOpeningEventAction::ReachRexaHarbor:Result=Opening->ReachRexaHarbor();break;
  case EOpeningEventAction::RecoverCheckpoint:Result=Opening->RecoverAtCheckpoint(InstigatorActor);break;
  default:break;
 }
 if(Result&&bOneShot)bConsumed=true;
 return Result;
}
