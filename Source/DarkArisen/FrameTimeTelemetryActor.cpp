// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "FrameTimeTelemetryActor.h"

#include "DarkArisen.h"
#include "DesignLaws.h"
#include "HAL/PlatformTime.h"

AFrameTimeTelemetryActor::AFrameTimeTelemetryActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_LastDemotable;
    SetActorHiddenInGame(true);
    SetCanBeDamaged(false);
    FrameTimesMilliseconds.Reserve(720);
}

void AFrameTimeTelemetryActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const double Now = FPlatformTime::Seconds();
    if (PreviousPlatformSeconds <= 0.0)
    {
        PreviousPlatformSeconds = Now;
        return;
    }

    const double FrameMilliseconds = (Now - PreviousPlatformSeconds) * 1000.0;
    PreviousPlatformSeconds = Now;
    if (FrameMilliseconds <= 0.0 || FrameMilliseconds > 1000.0)
    {
        return;
    }

    FrameTimesMilliseconds.Add(FrameMilliseconds);
    WindowSeconds += FrameMilliseconds / 1000.0;
    if (WindowSeconds >= 10.0)
    {
        FlushWindow();
    }
}

void AFrameTimeTelemetryActor::FlushWindow()
{
    if (FrameTimesMilliseconds.IsEmpty())
    {
        WindowSeconds = 0.0;
        return;
    }

    FrameTimesMilliseconds.Sort();
    const int32 Count = FrameTimesMilliseconds.Num();
    const int32 P50Index = FMath::Clamp(FMath::CeilToInt(Count * 0.50) - 1, 0, Count - 1);
    const int32 P95Index = FMath::Clamp(FMath::CeilToInt(Count * 0.95) - 1, 0, Count - 1);
    const double BudgetMilliseconds =
        1000.0 / static_cast<double>(DarkArisen::DesignLaws::TargetFramesPerSecond);

    int32 OverBudgetCount = 0;
    for (const double FrameTime : FrameTimesMilliseconds)
    {
        OverBudgetCount += FrameTime > BudgetMilliseconds ? 1 : 0;
    }

    const double OverBudgetPercent = static_cast<double>(OverBudgetCount) * 100.0 / Count;
    UE_LOG(LogDarkArisenTelemetry, Display,
        TEXT("{\"event\":\"frame_time\",\"samples\":%d,\"p50_ms\":%.3f,\"p95_ms\":%.3f,\"max_ms\":%.3f,\"budget_ms\":%.3f,\"over_budget_pct\":%.2f}"),
        Count,
        FrameTimesMilliseconds[P50Index],
        FrameTimesMilliseconds[P95Index],
        FrameTimesMilliseconds.Last(),
        BudgetMilliseconds,
        OverBudgetPercent);

    FrameTimesMilliseconds.Reset(720);
    WindowSeconds = 0.0;
}

