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
    for (const TWeakObjectPtr<AActor>& Responder : Responders)
    {
        ICombatProximityResponder::Execute_ReceiveCombatActivity(
            Responder.Get(), WorldLocation);
    }
}
