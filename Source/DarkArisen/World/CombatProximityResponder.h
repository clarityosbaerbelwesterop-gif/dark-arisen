// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatProximityResponder.generated.h"

UINTERFACE()
class DARKARISEN_API UCombatProximityResponder : public UInterface
{
    GENERATED_BODY()
};

/** World actors implement this to react to combat without coupling combat code to a region. */
class DARKARISEN_API ICombatProximityResponder
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "World|Combat")
    void ReceiveCombatActivity(const FVector& WorldLocation);
};
