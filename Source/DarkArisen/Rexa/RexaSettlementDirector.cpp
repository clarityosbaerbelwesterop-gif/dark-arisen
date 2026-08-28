// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementDirector.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Rexa/RexaSettlementAnchor.h"
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

bool ARexaSettlementDirector::ApplyGameMinute(const int64 GameMinute)
{
    if (GameMinute < 0) return false;
    if (SpawnedResidents.IsEmpty() && !SpawnAuthoredSettlement()) return false;

    TMap<FName, ARexaSettlementAnchor*> Anchors;
    if (!BuildAnchorRegistry(Anchors)) return false;

    struct FRouteCommand
    {
        ARexaSettlementResident* Resident = nullptr;
        ARexaSettlementAnchor* Anchor = nullptr;
    };
    TArray<FRouteCommand> Commands;
    Commands.Reserve(SpawnedResidents.Num());
    const int64 MinuteOfDay = GameMinute % 1440;
    const bool bMidday = MinuteOfDay >= 660 && MinuteOfDay < 900;
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if (!IsValid(Resident)) return false;
        const FName RequiredAnchorId =
            Resident->GetResidentDefinition().GetPurposeAnchorAtGameMinute(GameMinute);
        ARexaSettlementAnchor* const* Anchor = Anchors.Find(RequiredAnchorId);
        if (!Anchor || !IsValid(*Anchor) || (bMidday && !(*Anchor)->bShelteredFromMiddayHeat))
            return false;
        Commands.Add({Resident, *Anchor});
    }

    for (const FRouteCommand& Command : Commands)
    {
        if (!Command.Resident->MoveToPurposeAnchor(GameMinute, Command.Anchor))
        {
            for (ARexaSettlementResident* Resident : SpawnedResidents)
            {
                if (IsValid(Resident)) Resident->ClearPurposeRoute();
            }
            return false;
        }
    }
    return Commands.Num() == URexaSettlementRoster::RequiredResidentCount;
}

bool ARexaSettlementDirector::BuildAnchorRegistry(
    TMap<FName, ARexaSettlementAnchor*>& OutAnchors) const
{
    OutAnchors.Reset();
    UWorld* World = GetWorld();
    if (!World || SettlementId.IsNone()) return false;
    for (TActorIterator<ARexaSettlementAnchor> It(World); It; ++It)
    {
        ARexaSettlementAnchor* Anchor = *It;
        if (!IsValid(Anchor) || Anchor->SettlementId != SettlementId) continue;
        if (!Anchor->IsAuthoredAnchorValid() || OutAnchors.Contains(Anchor->AnchorId))
            return false;
        OutAnchors.Add(Anchor->AnchorId, Anchor);
    }
    return !OutAnchors.IsEmpty();
}
