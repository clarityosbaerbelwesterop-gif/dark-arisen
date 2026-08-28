// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/DarkArisenWorldRulesSubsystem.h"

#include "Stats/Stats.h"

void UDarkArisenWorldRulesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FractionalWorldMinutes = 0.0f;
    bPendingAutosaveRequest = false;
}

void UDarkArisenWorldRulesSubsystem::Tick(float DeltaTime)
{
    if (DeltaTime <= 0.0f)
    {
        return;
    }

    FractionalWorldMinutes += DeltaTime * GameMinutesPerRealSecond;
    const int32 WholeMinutes = FMath::FloorToInt(FractionalWorldMinutes);
    if (WholeMinutes > 0)
    {
        TotalWorldMinutes += WholeMinutes;
        FractionalWorldMinutes -= static_cast<float>(WholeMinutes);
    }
}

TStatId UDarkArisenWorldRulesSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UDarkArisenWorldRulesSubsystem, STATGROUP_Tickables);
}

int32 UDarkArisenWorldRulesSubsystem::GetMinuteOfDay() const
{
    constexpr int64 MinutesPerDay = 24 * 60;
    int64 Minute = TotalWorldMinutes % MinutesPerDay;
    if (Minute < 0)
    {
        Minute += MinutesPerDay;
    }
    return static_cast<int32>(Minute);
}

int32 UDarkArisenWorldRulesSubsystem::GetDayNumber() const
{
    constexpr int64 MinutesPerDay = 24 * 60;
    return static_cast<int32>(TotalWorldMinutes / MinutesPerDay) + 1;
}

bool UDarkArisenWorldRulesSubsystem::SetChapter(int32 NewChapter)
{
    if (NewChapter < 1 || NewChapter == Chapter)
    {
        return false;
    }

    Chapter = NewChapter;
    return true;
}

bool UDarkArisenWorldRulesSubsystem::NotifyChapterBoundary(int32 NewChapter)
{
    if (NewChapter <= Chapter)
    {
        return false;
    }

    Chapter = NewChapter;
    QueueLegalAutosaveRequest();
    return true;
}

bool UDarkArisenWorldRulesSubsystem::NotifyRestCompleted()
{
    QueueLegalAutosaveRequest();
    return true;
}

void UDarkArisenWorldRulesSubsystem::BeginLakeToDockAutosaveSuppression()
{
    bAutosaveSuppressed = true;
    bPendingAutosaveRequest = false;
}

void UDarkArisenWorldRulesSubsystem::EndLakeToDockAutosaveSuppression()
{
    bAutosaveSuppressed = false;
}

bool UDarkArisenWorldRulesSubsystem::ConsumePendingAutosaveRequest()
{
    if (!bPendingAutosaveRequest || bAutosaveSuppressed)
    {
        return false;
    }

    bPendingAutosaveRequest = false;
    return true;
}

void UDarkArisenWorldRulesSubsystem::QueueLegalAutosaveRequest()
{
    if (!bAutosaveSuppressed)
    {
        bPendingAutosaveRequest = true;
    }
}
