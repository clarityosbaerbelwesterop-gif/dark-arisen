// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrameTimeTelemetryActor.generated.h"

/** Emits compact server-side frame-time windows against technical_targets.md section 3. */
UCLASS(NotBlueprintable, Transient)
class DARKARISEN_API AFrameTimeTelemetryActor : public AActor
{
    GENERATED_BODY()

public:
    AFrameTimeTelemetryActor();
    virtual void Tick(float DeltaSeconds) override;

private:
    TArray<double> FrameTimesMilliseconds;
    double PreviousPlatformSeconds = 0.0;
    double WindowSeconds = 0.0;

    void FlushWindow();
};

