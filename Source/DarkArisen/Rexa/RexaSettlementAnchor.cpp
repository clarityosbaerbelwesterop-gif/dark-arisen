// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementAnchor.h"

#include "Components/SceneComponent.h"

ARexaSettlementAnchor::ARexaSettlementAnchor()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    SetActorEnableCollision(false);
}

bool ARexaSettlementAnchor::InitializeAuthoredAnchor(
    const FName InSettlementId,
    const FName InAnchorId,
    const bool bInShelteredFromMiddayHeat,
    const bool bInChildSafetyDestination)
{
    if (InSettlementId.IsNone() || InAnchorId.IsNone()) return false;
    SettlementId = InSettlementId;
    AnchorId = InAnchorId;
    bShelteredFromMiddayHeat = bInShelteredFromMiddayHeat;
    bChildSafetyDestination = bInChildSafetyDestination;
    return true;
}

bool ARexaSettlementAnchor::IsAuthoredAnchorValid() const
{
    return !SettlementId.IsNone() && !AnchorId.IsNone() && !GetActorLocation().ContainsNaN();
}
