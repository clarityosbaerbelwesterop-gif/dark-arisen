// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/PrincessQuestStateComponent.h"

#include "Engine/World.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

UPrincessQuestStateComponent::UPrincessQuestStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UPrincessQuestStateComponent::RecordFalseLetterHandedOver()
{
    if (Movement != EPrincessQuestMovement::NotStarted || bAutosaveWindowStarted)
    {
        return false;
    }

    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    if (!Rules)
    {
        return false;
    }

    Rules->BeginLakeToDockAutosaveSuppression();
    if (!Rules->IsAutosaveSuppressed())
    {
        return false;
    }

    bAutosaveWindowStarted = true;
    Movement = EPrincessQuestMovement::FalseLetter;
    return true;
}

bool UPrincessQuestStateComponent::RecordReturnedToArion()
{
    if (Movement != EPrincessQuestMovement::FalseLetter)
    {
        return false;
    }
    Movement = EPrincessQuestMovement::ReturnedToArion;
    return true;
}

bool UPrincessQuestStateComponent::RecordLakeReached()
{
    if (Movement != EPrincessQuestMovement::ReturnedToArion)
    {
        return false;
    }
    Movement = EPrincessQuestMovement::Lake;
    return true;
}

bool UPrincessQuestStateComponent::RecordArrowMoment()
{
    if (Movement != EPrincessQuestMovement::Lake)
    {
        return false;
    }
    Movement = EPrincessQuestMovement::ArrowAftermath;
    return true;
}

bool UPrincessQuestStateComponent::RecordArionFalls(const bool bInReadRealLetter)
{
    if (Movement != EPrincessQuestMovement::ArrowAftermath)
    {
        return false;
    }

    bReadRealLetter = bInReadRealLetter;
    Movement = EPrincessQuestMovement::ArionFalls;
    return true;
}

bool UPrincessQuestStateComponent::RecordTurnedWest()
{
    if (Movement != EPrincessQuestMovement::ArionFalls)
    {
        return false;
    }

    BelosResolution = EBelosPathResolution::TurnedAway;
    Movement = EPrincessQuestMovement::TurnedWest;
    return true;
}

bool UPrincessQuestStateComponent::BeginBelosAssault()
{
    if (Movement != EPrincessQuestMovement::ArionFalls)
    {
        return false;
    }

    Movement = EPrincessQuestMovement::Belos;
    return true;
}

bool UPrincessQuestStateComponent::ResolveBelosAssault(const bool bHasCrystalKatana)
{
    if (Movement != EPrincessQuestMovement::Belos || BelosResolution != EBelosPathResolution::None)
    {
        return false;
    }

    BelosResolution = bHasCrystalKatana
        ? EBelosPathResolution::KatanaAssaultInterrupted
        : EBelosPathResolution::OrdinaryWeaponOverwhelmed;
    return true;
}

bool UPrincessQuestStateComponent::RecordWentToUndercroftStair()
{
    if (Movement != EPrincessQuestMovement::Belos
        || BelosResolution == EBelosPathResolution::None
        || bWentToUndercroftStair)
    {
        return false;
    }

    bWentToUndercroftStair = true;
    return true;
}

bool UPrincessQuestStateComponent::BeginReturnJourney()
{
    if ((Movement != EPrincessQuestMovement::Belos
            || BelosResolution == EBelosPathResolution::None)
        && Movement != EPrincessQuestMovement::TurnedWest)
    {
        return false;
    }

    Movement = EPrincessQuestMovement::ReturnJourney;
    return true;
}

bool UPrincessQuestStateComponent::RecordDockReached()
{
    if (Movement != EPrincessQuestMovement::ReturnJourney || !bAutosaveWindowStarted)
    {
        return false;
    }

    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    if (!Rules || !Rules->IsAutosaveSuppressed())
    {
        return false;
    }

    Rules->EndLakeToDockAutosaveSuppression();
    if (Rules->IsAutosaveSuppressed())
    {
        return false;
    }

    bAutosaveWindowStarted = false;
    Movement = EPrincessQuestMovement::DockReached;
    return true;
}

bool UPrincessQuestStateComponent::CanDamageArionActor(
    const bool bIsCombatant,
    const bool bIsChild) const
{
    return bIsCombatant && !bIsChild;
}
