// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementDirector.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Rexa/RexaSettlementResident.h"
#include "Rexa/RexaSettlementRoster.h"

ARexaSettlementDirector::ARexaSettlementDirector()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    ResidentClass = ARexaSettlementResident::StaticClass();
}

void ARexaSettlementDirector::BeginPlay()
{
    Super::BeginPlay();
    if (bSpawnOnBeginPlay) SpawnAuthoredSettlement();
}

bool ARexaSettlementDirector::SpawnAuthoredSettlement()
{
    SpawnedResidents.RemoveAll([](const TObjectPtr<ARexaSettlementResident>& Resident)
    {
        return !IsValid(Resident);
    });
    if (!SpawnedResidents.IsEmpty())
        return SpawnedResidents.Num() == URexaSettlementRoster::RequiredResidentCount;

    UWorld* World = GetWorld();
    const TArray<FRexaResidentDefinition> Residents =
        URexaSettlementRoster::GetAuthoredResidents();
    if (!World || !ResidentClass || !URexaSettlementRoster::IsRosterValid(Residents))
        return false;

    for (const FRexaResidentDefinition& Definition : Residents)
    {
        FActorSpawnParameters Parameters;
        Parameters.Owner = this;
        Parameters.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        ARexaSettlementResident* Spawned = World->SpawnActor<ARexaSettlementResident>(
            ResidentClass,
            GetActorTransform().TransformPosition(Definition.HomeOffsetCentimetres),
            GetActorRotation(),
            Parameters);
        if (!Spawned || !Spawned->InitializeFromDefinition(Definition))
        {
            if (Spawned) Spawned->Destroy();
            ClearSpawnedSettlement();
            return false;
        }
        SpawnedResidents.Add(Spawned);
    }
    return SpawnedResidents.Num() == URexaSettlementRoster::RequiredResidentCount;
}

void ARexaSettlementDirector::ClearSpawnedSettlement()
{
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if (IsValid(Resident)) Resident->Destroy();
    }
    SpawnedResidents.Reset();
}

int32 ARexaSettlementDirector::GetSpawnedResidentCount() const
{
    return SpawnedResidents.Num();
}
