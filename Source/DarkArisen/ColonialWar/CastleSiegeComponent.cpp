// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/CastleSiegeComponent.h"

#include "ContentScale/AuthoredRewardCatalog.h"

UCastleSiegeComponent::UCastleSiegeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UCastleSiegeComponent::ConfigureCastle(const FName InCastleId, const bool bInCoastal)
{
    if (InCastleId.IsNone() || !CastleId.IsNone())
    {
        return false;
    }
    CastleId = InCastleId;
    bCoastal = bInCoastal;
    return true;
}

bool UCastleSiegeComponent::BeginSiege(const bool bDefensiveSiege)
{
    if (CastleId.IsNone() || Phase != ESiegePhase::None)
    {
        return false;
    }
    bDefensive = bDefensiveSiege;
    Phase = ESiegePhase::Approach;
    return true;
}

bool UCastleSiegeComponent::RecordApproachIntel(const FName IntelId)
{
    if (Phase != ESiegePhase::Approach || IntelId.IsNone() || ApproachIntel.Contains(IntelId))
    {
        return false;
    }
    ApproachIntel.Add(IntelId);
    return true;
}

bool UCastleSiegeComponent::RecordInvestmentAction(const FName ActionId)
{
    if ((Phase != ESiegePhase::Approach && Phase != ESiegePhase::Investment)
        || ActionId.IsNone()
        || InvestmentActions.Contains(ActionId))
    {
        return false;
    }
    Phase = ESiegePhase::Investment;
    InvestmentActions.Add(ActionId);
    return true;
}

bool UCastleSiegeComponent::CommitBreachDoor(const ESiegeDoor InDoor)
{
    if ((Phase != ESiegePhase::Approach && Phase != ESiegePhase::Investment)
        || bDoorCommitted)
    {
        return false;
    }

    // Blind assault is legal; it simply arrives here with no Approach intel or Investment history.
    Door = InDoor;
    bDoorCommitted = true;
    Phase = ESiegePhase::Breach;
    return true;
}

bool UCastleSiegeComponent::RecordBreachCompleted(const bool bStructuralDamageInflicted)
{
    if (Phase != ESiegePhase::Breach || !bDoorCommitted || bBreachCompleted)
    {
        return false;
    }

    bBreachCompleted = true;
    bPersistentBreachDamage = bPersistentBreachDamage || bStructuralDamageInflicted;
    Phase = ESiegePhase::Assault;
    return true;
}

bool UCastleSiegeComponent::RecordCommanderResolved(const FName CommanderId)
{
    if (Phase != ESiegePhase::Assault || CommanderId.IsNone() || bCommanderResolved)
    {
        return false;
    }
    bCommanderResolved = true;
    return true;
}

bool UCastleSiegeComponent::RecordStateTreasureRecovered(const FName TreasureId)
{
    if (Phase != ESiegePhase::Assault || TreasureId.IsNone() || RecoveredStateTreasures.Contains(TreasureId))
    {
        return false;
    }

    const TArray<FAuthoredRewardBinding> StateTreasures =
        FAuthoredRewardCatalog::BuildStateTreasureSlots();
    const FAuthoredRewardBinding* Binding = StateTreasures.FindByPredicate(
        [TreasureId](const FAuthoredRewardBinding& Entry)
        {
            return Entry.SourceContentId == TreasureId || Entry.StableId == TreasureId;
        });
    if (!Binding || Binding->OriginStableId != CastleId)
    {
        return false;
    }

    RecoveredStateTreasures.Add(TreasureId);
    return true;
}

bool UCastleSiegeComponent::ResolveSiege(const ESiegeResolution InResolution)
{
    if (Phase != ESiegePhase::Assault
        || !bBreachCompleted
        || !bCommanderResolved
        || InResolution == ESiegeResolution::None)
    {
        return false;
    }

    if (bPersistentBreachDamage && InResolution == ESiegeResolution::HeldIntact)
    {
        return false;
    }

    if (bDefensive && InResolution != ESiegeResolution::Defended && InResolution != ESiegeResolution::Lost)
    {
        return false;
    }

    if (!bDefensive && (InResolution == ESiegeResolution::Defended || InResolution == ESiegeResolution::Lost))
    {
        return false;
    }

    Resolution = InResolution;
    Phase = ESiegePhase::Resolved;
    return true;
}
