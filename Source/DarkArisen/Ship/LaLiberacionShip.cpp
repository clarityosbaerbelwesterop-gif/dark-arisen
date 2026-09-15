// Copyright (c) 2026 Dark Arisen. All Rights Reserved.
#include "Ship/LaLiberacionShip.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Interaction/PhysicalMapActor.h"
#include "Ship/NavalCombatComponent.h"
#include "Ship/SeaPassageComponent.h"
#include "Ship/ShipHouseholdComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"
ALaLiberacionShip::ALaLiberacionShip(){PrimaryActorTick.bCanEverTick=true;ShipRoot=CreateDefaultSubobject<USceneComponent>(TEXT("ShipRoot"));SetRootComponent(ShipRoot);WeatherDeckRoot=CreateDefaultSubobject<USceneComponent>(TEXT("WeatherDeckRoot"));WeatherDeckRoot->SetupAttachment(ShipRoot);UpperDeckRoot=CreateDefaultSubobject<USceneComponent>(TEXT("UpperDeckRoot"));UpperDeckRoot->SetupAttachment(ShipRoot);MidDeckRoot=CreateDefaultSubobject<USceneComponent>(TEXT("MidDeckRoot"));MidDeckRoot->SetupAttachment(ShipRoot);HoldDeckRoot=CreateDefaultSubobject<USceneComponent>(TEXT("HoldDeckRoot"));HoldDeckRoot->SetupAttachment(ShipRoot);VoyageComponent=CreateDefaultSubobject<UShipVoyageComponent>(TEXT("VoyageComponent"));HouseholdComponent=CreateDefaultSubobject<UShipHouseholdComponent>(TEXT("HouseholdComponent"));SeaPassageComponent=CreateDefaultSubobject<USeaPassageComponent>(TEXT("SeaPassageComponent"));NavalCombatComponent=CreateDefaultSubobject<UNavalCombatComponent>(TEXT("NavalCombatComponent"));PhysicalMapClass=APhysicalMapActor::StaticClass();}
void ALaLiberacionShip::BeginPlay(){Super::BeginPlay();ensureAlwaysMsgf(HasCompleteFourDeckStructure(),TEXT("La Liberacion requires exactly four authored deck attachment roots."));ensureAlwaysMsgf(SpawnPhysicalMap(),TEXT("La Liberacion requires a physical held map on the upper/great-cabin deck."));if(bStartInCombatHelm&&VoyageComponent){VoyageComponent->SetOwnedAndUnlocked(true);VoyageComponent->SetJakeAtHelm(true);VoyageComponent->SetThrottle(.65f);}}
void ALaLiberacionShip::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);if(!VoyageComponent||!NavalCombatComponent||!VoyageComponent->IsJakeAtHelm())return;APlayerController* PC=GetWorld()?GetWorld()->GetFirstPlayerController():nullptr;if(!PC)return;
    const float Throttle=(PC->IsInputKeyDown(EKeys::W)||PC->IsInputKeyDown(EKeys::Gamepad_LeftStick_Up))?1.f:((PC->IsInputKeyDown(EKeys::S)||PC->IsInputKeyDown(EKeys::Gamepad_LeftStick_Down))?0.25f:0.65f);VoyageComponent->SetThrottle(Throttle);
    float Heading=VoyageComponent->GetHeadingDegrees();if(PC->IsInputKeyDown(EKeys::A)||PC->IsInputKeyDown(EKeys::Gamepad_LeftStick_Left))Heading-=45.f*DeltaSeconds;if(PC->IsInputKeyDown(EKeys::D)||PC->IsInputKeyDown(EKeys::Gamepad_LeftStick_Right))Heading+=45.f*DeltaSeconds;VoyageComponent->SetHelmCommandDegrees(Heading);
    if(PC->WasInputKeyJustPressed(EKeys::Q)||PC->WasInputKeyJustPressed(EKeys::Gamepad_LeftShoulder))NavalCombatComponent->FireBroadside(ENavalBroadside::Port);
    if(PC->WasInputKeyJustPressed(EKeys::E)||PC->WasInputKeyJustPressed(EKeys::Gamepad_RightShoulder))NavalCombatComponent->FireBroadside(ENavalBroadside::Starboard);
}
USceneComponent* ALaLiberacionShip::GetDeckRoot(const EShipDeck Deck) const{switch(Deck){case EShipDeck::Weather:return WeatherDeckRoot;case EShipDeck::Upper:return UpperDeckRoot;case EShipDeck::Mid:return MidDeckRoot;case EShipDeck::Hold:return HoldDeckRoot;default:return nullptr;}}
bool ALaLiberacionShip::CompleteGreatCabinRest(){if(!CanRestInGreatCabin())return false;UWorld* World=GetWorld();UDarkArisenWorldRulesSubsystem* Rules=World?World->GetSubsystem<UDarkArisenWorldRulesSubsystem>():nullptr;return Rules&&Rules->NotifyRestCompleted();}
bool ALaLiberacionShip::RestGreatCabinToDaypart(const EDarkArisenDaypart TargetDaypart){if(!CanRestInGreatCabin())return false;UWorld* World=GetWorld();UDarkArisenWorldRulesSubsystem* Rules=World?World->GetSubsystem<UDarkArisenWorldRulesSubsystem>():nullptr;return Rules&&Rules->CompleteRest(EDarkArisenRestLocation::GreatCabin,TargetDaypart);}
bool ALaLiberacionShip::CanRestInGreatCabin() const{return VoyageComponent&&VoyageComponent->IsJakeInGreatCabin();}
bool ALaLiberacionShip::HasCompleteFourDeckStructure() const{return ShipRoot&&WeatherDeckRoot&&UpperDeckRoot&&MidDeckRoot&&HoldDeckRoot&&UShipVoyageComponent::GetRequiredDeckCount()==4;}
bool ALaLiberacionShip::SpawnPhysicalMap(){if(IsValid(PhysicalMap))return true;UWorld* World=GetWorld();if(!World||!UpperDeckRoot||!PhysicalMapClass)return false;FActorSpawnParameters P;P.Owner=this;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;PhysicalMap=World->SpawnActor<APhysicalMapActor>(PhysicalMapClass,UpperDeckRoot->GetComponentTransform(),P);if(!PhysicalMap)return false;PhysicalMap->AttachToComponent(UpperDeckRoot,FAttachmentTransformRules::KeepWorldTransform);return true;}
