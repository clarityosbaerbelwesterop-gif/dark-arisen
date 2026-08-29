// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Bosses/Tier1CommanderEncounterComponent.h"

UTier1CommanderEncounterComponent::UTier1CommanderEncounterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTier1CommanderEncounterComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeDefinition();
}

bool UTier1CommanderEncounterComponent::InitializeDefinition()
{
    bDefinitionValid = FTier1CommanderCatalog::TryGet(CommanderStableId, Definition);
    CurrentPhaseIndex = 0;
    if (!bDefinitionValid)
    {
        UE_LOG(LogTemp, Error, TEXT("Tier-1 commander component has unknown stable id '%s'."), *CommanderStableId.ToString());
    }
    return bDefinitionValid;
}

bool UTier1CommanderEncounterComponent::BeginEncounter()
{
    if ((!bDefinitionValid && !InitializeDefinition())
        || bEncounterBegun
        || Resolution != ETier1CommanderResolution::Unresolved)
    {
        return false;
    }
    bEncounterBegun = true;
    CurrentPhaseIndex = 0;
    return true;
}

bool UTier1CommanderEncounterComponent::UpdateHealthFraction(const float HealthFraction)
{
    if (!bEncounterBegun
        || Resolution != ETier1CommanderResolution::Unresolved
        || Definition.PhaseHealthFractions.Num() != 2
        || Definition.PhaseIds.Num() != 3)
    {
        return false;
    }

    const float Clamped = FMath::Clamp(HealthFraction, 0.0f, 1.0f);
    int32 NewPhase = 0;
    if (Clamped <= Definition.PhaseHealthFractions[1])
    {
        NewPhase = 2;
    }
    else if (Clamped <= Definition.PhaseHealthFractions[0])
    {
        NewPhase = 1;
    }

    if (NewPhase <= CurrentPhaseIndex)
    {
        return false;
    }
    CurrentPhaseIndex = NewPhase;
    return true;
}

bool UTier1CommanderEncounterComponent::AdvanceScriptedStage(const FName StageId)
{
    if (!bEncounterBegun
        || Resolution != ETier1CommanderResolution::Unresolved
        || !Definition.PhaseHealthFractions.IsEmpty()
        || StageId.IsNone()
        || Definition.PhaseIds.IsEmpty())
    {
        return false;
    }

    const int32 RequestedIndex = Definition.PhaseIds.IndexOfByKey(StageId);
    if (RequestedIndex == INDEX_NONE || RequestedIndex != CurrentPhaseIndex + 1)
    {
        return false;
    }
    CurrentPhaseIndex = RequestedIndex;
    return true;
}

bool UTier1CommanderEncounterComponent::ResolveInternal(
    const ETier1CommanderResolution NewResolution,
    const FName RouteId)
{
    if ((!bDefinitionValid && !InitializeDefinition())
        || Resolution != ETier1CommanderResolution::Unresolved
        || NewResolution == ETier1CommanderResolution::Unresolved)
    {
        return false;
    }

    if (NewResolution == ETier1CommanderResolution::Avoided && RouteId.IsNone())
    {
        return false;
    }

    Resolution = NewResolution;
    AvoidedRouteId = NewResolution == ETier1CommanderResolution::Avoided ? RouteId : NAME_None;
    bEncounterBegun = false;
    return true;
}

bool UTier1CommanderEncounterComponent::ResolveKilled()
{
    if (!bEncounterBegun)
    {
        return false;
    }
    return ResolveInternal(ETier1CommanderResolution::Killed);
}

bool UTier1CommanderEncounterComponent::TryResolveSpared()
{
    if (!bEncounterBegun || Resolution != ETier1CommanderResolution::Unresolved)
    {
        return false;
    }

    if (Definition.ResolutionRule == ETier1CommanderResolutionRule::SpareRefusedLeaveOnly)
    {
        bSpareRefusedObserved = true;
        return false;
    }
    return ResolveInternal(ETier1CommanderResolution::Spared);
}

bool UTier1CommanderEncounterComponent::ResolveAvoided(const FName RouteId)
{
    // Avoidance may resolve before the direct encounter begins: uprising, purchase, recall, supply cut,
    // alliance, standoff, heist, dispatch exposure and education are all authored pre-combat paths.
    return ResolveInternal(ETier1CommanderResolution::Avoided, RouteId);
}

FName UTier1CommanderEncounterComponent::GetCurrentPhaseId() const
{
    if (!bDefinitionValid || !Definition.PhaseIds.IsValidIndex(CurrentPhaseIndex))
    {
        return NAME_None;
    }
    return Definition.PhaseIds[CurrentPhaseIndex];
}
