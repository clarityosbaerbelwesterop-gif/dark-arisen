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
    OnChapterBoundary.Broadcast(Chapter);
    return true;
}

bool UDarkArisenWorldRulesSubsystem::NotifyRestCompleted()
{
    QueueLegalAutosaveRequest();
    return true;
}

bool UDarkArisenWorldRulesSubsystem::CompleteRest(
    const EDarkArisenRestLocation Location,
    const EDarkArisenDaypart TargetDaypart)
{
    if (Location != EDarkArisenRestLocation::GreatCabin
        && Location != EDarkArisenRestLocation::SafeHouse)
    {
        return false;
    }

    constexpr int32 MinutesPerDay = 24 * 60;
    const int32 CurrentMinute = GetMinuteOfDay();
    const int32 TargetMinute = GetDaypartMinute(TargetDaypart);
    int32 DeltaMinutes = TargetMinute - CurrentMinute;
    if (DeltaMinutes <= 0)
    {
        DeltaMinutes += MinutesPerDay;
    }

    TotalWorldMinutes += DeltaMinutes;
    FractionalWorldMinutes = 0.0f;
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

int32 UDarkArisenWorldRulesSubsystem::GetDaypartMinute(const EDarkArisenDaypart TargetDaypart)
{
    switch (TargetDaypart)
    {
    case EDarkArisenDaypart::Dawn:
        return 6 * 60;
    case EDarkArisenDaypart::Midday:
        return 12 * 60;
    case EDarkArisenDaypart::Dusk:
        return 18 * 60;
    case EDarkArisenDaypart::Night:
        return 22 * 60;
    default:
        return 6 * 60;
    }
}


FDarkArisenWorldRulesSnapshot UDarkArisenWorldRulesSubsystem::CaptureSnapshot() const
{
    FDarkArisenWorldRulesSnapshot Snapshot;
    Snapshot.bValid=true;
    Snapshot.TotalWorldMinutes=TotalWorldMinutes;
    Snapshot.Chapter=Chapter;
    Snapshot.bAutosaveSuppressed=bAutosaveSuppressed;
    return Snapshot;
}

bool UDarkArisenWorldRulesSubsystem::RestoreSnapshot(const FDarkArisenWorldRulesSnapshot& Snapshot)
{
    if(!Snapshot.bValid || Snapshot.TotalWorldMinutes<0 || Snapshot.Chapter<1 || Snapshot.Chapter>10) return false;
    TotalWorldMinutes=Snapshot.TotalWorldMinutes;
    Chapter=Snapshot.Chapter;
    bAutosaveSuppressed=Snapshot.bAutosaveSuppressed;
    FractionalWorldMinutes=0.f;
    bPendingAutosaveRequest=false;
    return true;
}
