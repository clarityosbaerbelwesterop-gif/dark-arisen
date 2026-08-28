// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatProximitySubsystem.generated.h"

class AActor;

/** Sparse registry: combat signals visit responders, never every actor in the world. */
UCLASS()
class DARKARISEN_API UCombatProximitySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void RegisterResponder(AActor* Responder);
    void UnregisterResponder(AActor* Responder);
    void BroadcastCombatActivity(const FVector& WorldLocation);

private:
    TArray<TWeakObjectPtr<AActor>> Responders;
};
