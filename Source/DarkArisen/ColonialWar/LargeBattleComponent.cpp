// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/LargeBattleComponent.h"

ULargeBattleComponent::ULargeBattleComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool ULargeBattleComponent::BeginBattle(const FName InBattleId)
{
    if (InBattleId.IsNone() || bBattleActive)
    {
        return false;
    }

    BattleId = InBattleId;
    bBattleActive = true;
    bJakeFallen = false;
    BattleOutcome = ELargeBattleOutcome::Unresolved;
    return true;
}

bool ULargeBattleComponent::RecordJakeFallen()
{
    if (!bBattleActive || bJakeFallen)
    {
        return false;
    }

    bJakeFallen = true;
    return true;
}

bool ULargeBattleComponent::ResolveBattle(const ELargeBattleOutcome Outcome)
{
    if (!bBattleActive || Outcome == ELargeBattleOutcome::Unresolved)
    {
        return false;
    }

    BattleOutcome = Outcome;
    bBattleActive = false;
    return true;
}
