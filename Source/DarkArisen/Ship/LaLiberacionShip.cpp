// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Ship/LaLiberacionShip.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Interaction/PhysicalMapActor.h"
#include "Ship/SeaPassageComponent.h"
#include "Ship/ShipHouseholdComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

ALaLiberacionShip::ALaLiberacionShip()
{
    PrimaryActorTick.bCanEverTick = false;

    ShipRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShipRoot"));
    SetRootComponent(ShipRoot);

    WeatherDeckRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeatherDeckRoot"));
    WeatherDeckRoot->SetupAttachment(ShipRoot);

    UpperDeckRoot = CreateDefaultSubobject<USceneComponent>(TEXT("UpperDeckRoot"));
    UpperDeckRoot->SetupAttachment(ShipRoot);

    MidDeckRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MidDeckRoot"));
    MidDeckRoot->SetupAttachment(ShipRoot);

    HoldDeckRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HoldDeckRoot"));
    HoldDeckRoot->SetupAttachment(ShipRoot);

    VoyageComponent = CreateDefaultSubobject<UShipVoyageComponent>(TEXT("VoyageComponent"));
    HouseholdComponent = CreateDefaultSubobject<UShipHouseholdComponent>(TEXT("HouseholdComponent"));
    SeaPassageComponent = CreateDefaultSubobject<USeaPassageComponent>(TEXT("SeaPassageComponent"));
    PhysicalMapClass = APhysicalMapActor::StaticClass();
}

void ALaLiberacionShip::BeginPlay()
{
    Super::BeginPlay();
    ensureAlwaysMsgf(
        HasCompleteFourDeckStructure(),
        TEXT("La Liberacion requires exactly four authored deck attachment roots."));
    ensureAlwaysMsgf(
        SpawnPhysicalMap(),
        TEXT("La Liberacion requires a physical held map on the upper/great-cabin deck."));
}

USceneComponent* ALaLiberacionShip::GetDeckRoot(const EShipDeck Deck) const
{
    switch (Deck)
    {
    case EShipDeck::Weather:
        return WeatherDeckRoot;
    case EShipDeck::Upper:
        return UpperDeckRoot;
    case EShipDeck::Mid:
        return MidDeckRoot;
    case EShipDeck::Hold:
        return HoldDeckRoot;
    default:
        return nullptr;
    }
}

bool ALaLiberacionShip::CompleteGreatCabinRest()
{
    if (!CanRestInGreatCabin())
    {
        return false;
    }

    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    return Rules && Rules->NotifyRestCompleted();
}

bool ALaLiberacionShip::CanRestInGreatCabin() const
{
    return VoyageComponent != nullptr && VoyageComponent->IsJakeInGreatCabin();
}

bool ALaLiberacionShip::HasCompleteFourDeckStructure() const
{
    return ShipRoot != nullptr
        && WeatherDeckRoot != nullptr
        && UpperDeckRoot != nullptr
        && MidDeckRoot != nullptr
        && HoldDeckRoot != nullptr
        && UShipVoyageComponent::GetRequiredDeckCount() == 4;
}

bool ALaLiberacionShip::SpawnPhysicalMap()
{
    if (IsValid(PhysicalMap))
    {
        return true;
    }

    UWorld* World = GetWorld();
    if (!World || !UpperDeckRoot || !PhysicalMapClass)
    {
        return false;
    }

    FActorSpawnParameters Parameters;
    Parameters.Owner = this;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    PhysicalMap = World->SpawnActor<APhysicalMapActor>(
        PhysicalMapClass,
        UpperDeckRoot->GetComponentTransform(),
        Parameters);
    if (!PhysicalMap)
    {
        return false;
    }

    PhysicalMap->AttachToComponent(
        UpperDeckRoot,
        FAttachmentTransformRules::KeepWorldTransform);
    return true;
}
