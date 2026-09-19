// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementDirector.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Rexa/RexaSettlementAnchor.h"
#include "Rexa/RexaSettlementResident.h"
#include "Rexa/RexaSettlementRoster.h"
#include "World/CombatProximitySubsystem.h"
#include "World/DarkArisenWorldRulesSubsystem.h"
#include "World/NPCLivingWorldSubsystem.h"

ARexaSettlementDirector::ARexaSettlementDirector()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    ResidentClass = ARexaSettlementResident::StaticClass();
}

void ARexaSettlementDirector::BeginPlay()
{
    Super::BeginPlay();
    if (UCombatProximitySubsystem* Proximity =
        GetWorld() ? GetWorld()->GetSubsystem<UCombatProximitySubsystem>() : nullptr)
        Proximity->RegisterResponder(this);
    if (bSpawnOnBeginPlay) SpawnAuthoredSettlement();
    if (UDarkArisenWorldRulesSubsystem* WorldRules =
        GetWorld() ? GetWorld()->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr)
        ApplyGameMinute(WorldRules->GetTotalWorldMinutes());
}

void ARexaSettlementDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UCombatProximitySubsystem* Proximity =
        GetWorld() ? GetWorld()->GetSubsystem<UCombatProximitySubsystem>() : nullptr)
        Proximity->UnregisterResponder(this);
    ClearSpawnedSettlement();
    Super::EndPlay(EndPlayReason);
}

void ARexaSettlementDirector::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bCombatResponseActive)
    {
        if (UDarkArisenWorldRulesSubsystem* WorldRules =
            GetWorld() ? GetWorld()->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr)
        {
            const int64 CanonicalGameMinute = WorldRules->GetTotalWorldMinutes();
            if (CanonicalGameMinute != LastAppliedGameMinute)
                ApplyGameMinute(CanonicalGameMinute);
        }
        return;
    }

    CombatElapsedSeconds += FMath::Max(0.0f, DeltaSeconds);
    CombatSilenceRemainingSeconds -= FMath::Max(0.0f, DeltaSeconds);
    if (!bChildrenShelteredForPersistentCombat &&
        CombatElapsedSeconds >= ChildSceneRemovalDelaySeconds)
    {
        ShelterChildrenForPersistentCombat();
        bChildrenShelteredForPersistentCombat = true;
    }
    if (CombatSilenceRemainingSeconds <= 0.0f) EndCombatResponse();
}

void ARexaSettlementDirector::ReceiveCombatActivity_Implementation(
    const FVector& WorldLocation)
{
    NotifyCombatActivity(WorldLocation);
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
        if(UNPCLivingWorldSubsystem* Living=World->GetSubsystem<UNPCLivingWorldSubsystem>())
            Living->RegisterNPC(Definition.StableResidentId,TEXT("community.raices"));
    }
    const bool bCompleteRoster =
        SpawnedResidents.Num() == URexaSettlementRoster::RequiredResidentCount;
    if (bCompleteRoster) SetActorTickEnabled(true);
    return bCompleteRoster;
}

void ARexaSettlementDirector::ClearSpawnedSettlement()
{
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if (IsValid(Resident)) Resident->Destroy();
    }
    SpawnedResidents.Reset();
    bCombatResponseActive = false;
    bChildrenShelteredForPersistentCombat = false;
    CombatElapsedSeconds = 0.0f;
    CombatSilenceRemainingSeconds = 0.0f;
    LastAppliedGameMinute = -1;
    SetActorTickEnabled(false);
}

int32 ARexaSettlementDirector::GetSpawnedResidentCount() const
{
    return SpawnedResidents.Num();
}

bool ARexaSettlementDirector::ApplyGameMinute(const int64 GameMinute)
{
    if (GameMinute < 0 || bCombatResponseActive) return false;
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
        if(UNPCLivingWorldSubsystem* Living=GetWorld()->GetSubsystem<UNPCLivingWorldSubsystem>())
            Living->UpdateScheduleAnchor(Command.Resident->GetResidentDefinition().StableResidentId,Command.Anchor->AnchorId);
    }
    const bool bAllResidentsRouted =
        Commands.Num() == URexaSettlementRoster::RequiredResidentCount;
    if (bAllResidentsRouted) LastAppliedGameMinute = GameMinute;
    return bAllResidentsRouted;
}

int32 ARexaSettlementDirector::NotifyCombatActivity(const FVector& CombatLocation)
{
    if (CombatLocation.ContainsNaN()) return 0;
    if (SpawnedResidents.IsEmpty() && !SpawnAuthoredSettlement()) return 0;

    TMap<FName, ARexaSettlementAnchor*> Anchors;
    if (!BuildAnchorRegistry(Anchors)) Anchors.Reset();
    int32 AffectedChildren = 0;
    int32 AffectedResidents = 0;
    UNPCLivingWorldSubsystem* Living=GetWorld()?GetWorld()->GetSubsystem<UNPCLivingWorldSubsystem>():nullptr;
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if(!IsValid(Resident) ||
            FVector::DistSquared(Resident->GetActorLocation(),CombatLocation)>
                FMath::Square(ARexaSettlementResident::ChildCombatFleeRadiusCentimetres))
            continue;
        ++AffectedResidents;
        if(Living) Living->SetMood(Resident->GetResidentDefinition().StableResidentId,ENPCLivingMood::Stressed);
        if(!Resident->GetResidentDefinition().bProtectedChild)
        {
            Resident->EnterCivilianCombatFlee(CombatLocation);
            continue;
        }
        ++AffectedChildren;
        ARexaSettlementAnchor* const* SafetyAnchor =
            Anchors.Find(Resident->GetResidentDefinition().ChildSafetyAnchorId);
        ARexaSettlementAnchor* ValidSafetyAnchor =
            SafetyAnchor && IsValid(*SafetyAnchor) && (*SafetyAnchor)->bChildSafetyDestination
                ? *SafetyAnchor : nullptr;
        if (!ValidSafetyAnchor ||
            !Resident->EnterProtectedChildFlee(CombatLocation, ValidSafetyAnchor))
            Resident->ShelterProtectedChild(ValidSafetyAnchor);
    }

    if (AffectedResidents > 0 || bCombatResponseActive)
    {
        if (!bCombatResponseActive)
        {
            bCombatResponseActive = true;
            bChildrenShelteredForPersistentCombat = false;
            CombatElapsedSeconds = 0.0f;
            SetActorTickEnabled(true);
        }
        CombatSilenceRemainingSeconds = CombatSilenceBeforeReturnSeconds;
    }
    return AffectedChildren;
}

int32 ARexaSettlementDirector::ShelterChildrenForPersistentCombat()
{
    TMap<FName, ARexaSettlementAnchor*> Anchors;
    if (!BuildAnchorRegistry(Anchors)) Anchors.Reset();
    int32 ShelteredChildren = 0;
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if (!IsValid(Resident) ||
            Resident->CurrentSafetyState != ERexaResidentSafetyState::FleeingCombat)
            continue;
        ARexaSettlementAnchor* const* SafetyAnchor =
            Anchors.Find(Resident->GetResidentDefinition().ChildSafetyAnchorId);
        if (Resident->ShelterProtectedChild(
            SafetyAnchor && IsValid(*SafetyAnchor) ? *SafetyAnchor : nullptr))
            ++ShelteredChildren;
    }
    return ShelteredChildren;
}

bool ARexaSettlementDirector::EndCombatResponse()
{
    if (!bCombatResponseActive) return true;
    TMap<FName, ARexaSettlementAnchor*> Anchors;
    if (!BuildAnchorRegistry(Anchors)) return false;

    TArray<TPair<ARexaSettlementResident*, ARexaSettlementAnchor*>> RestoreCommands;
    for (ARexaSettlementResident* Resident : SpawnedResidents)
    {
        if (!IsValid(Resident)) return false;
        if (Resident->CurrentSafetyState == ERexaResidentSafetyState::Routine) continue;
        if(!Resident->GetResidentDefinition().bProtectedChild)
        {
            if(!Resident->RestoreCivilianRoutine()) return false;
            if(UNPCLivingWorldSubsystem* Living=GetWorld()->GetSubsystem<UNPCLivingWorldSubsystem>())
                Living->SetMood(Resident->GetResidentDefinition().StableResidentId,ENPCLivingMood::Content);
            continue;
        }
        ARexaSettlementAnchor* const* SafetyAnchor =
            Anchors.Find(Resident->GetResidentDefinition().ChildSafetyAnchorId);
        if (!SafetyAnchor || !IsValid(*SafetyAnchor) ||
            !(*SafetyAnchor)->bChildSafetyDestination) return false;
        RestoreCommands.Emplace(Resident, *SafetyAnchor);
    }
    for (const TPair<ARexaSettlementResident*, ARexaSettlementAnchor*>& Command :
        RestoreCommands)
    {
        if (!Command.Key->RestoreProtectedChildAfterCombat(Command.Value)) return false;
        if(UNPCLivingWorldSubsystem* Living=GetWorld()->GetSubsystem<UNPCLivingWorldSubsystem>())
            Living->SetMood(Command.Key->GetResidentDefinition().StableResidentId,ENPCLivingMood::Content);
    }

    bCombatResponseActive = false;
    bChildrenShelteredForPersistentCombat = false;
    CombatElapsedSeconds = 0.0f;
    CombatSilenceRemainingSeconds = 0.0f;

    const UDarkArisenWorldRulesSubsystem* WorldRules =
        GetWorld() ? GetWorld()->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    const int64 RestoreGameMinute =
        WorldRules ? WorldRules->GetTotalWorldMinutes() : LastAppliedGameMinute;
    if (RestoreGameMinute >= 0) ApplyGameMinute(RestoreGameMinute);
    return true;
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
