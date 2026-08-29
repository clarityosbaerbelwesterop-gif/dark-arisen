// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/CombatProximitySubsystem.h"

#include "World/CombatProximityResponder.h"

void UCombatProximitySubsystem::RegisterResponder(AActor* Responder)
{
    if (!IsValid(Responder) ||
        !Responder->GetClass()->ImplementsInterface(UCombatProximityResponder::StaticClass()))
        return;
    Responders.AddUnique(TWeakObjectPtr<AActor>(Responder));
}

void UCombatProximitySubsystem::UnregisterResponder(AActor* Responder)
{
    Responders.Remove(TWeakObjectPtr<AActor>(Responder));
}

void UCombatProximitySubsystem::BroadcastCombatActivity(const FVector& WorldLocation)
{
    if (WorldLocation.ContainsNaN()) return;
    Responders.RemoveAll([](const TWeakObjectPtr<AActor>& Responder)
    {
        return !Responder.IsValid();
    });
    // Iterate a copy: a responder may destroy actors or unregister itself while reacting.
    const TArray<TWeakObjectPtr<AActor>> PendingResponders = Responders;
    for (const TWeakObjectPtr<AActor>& Responder : PendingResponders)
    {
        if (!Responder.IsValid()) continue;
        ICombatProximityResponder::Execute_ReceiveCombatActivity(
            Responder.Get(), WorldLocation);
    }
}
