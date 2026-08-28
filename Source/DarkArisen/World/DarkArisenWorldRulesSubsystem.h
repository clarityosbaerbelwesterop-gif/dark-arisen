// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DarkArisenWorldRulesSubsystem.generated.h"

/**
 * Canonical M4 world-time/save owner.
 *
 * The world clock advances at 1 in-game hour per 150 real seconds. This subsystem owns
 * the clock so settlements, tides, ship watches and future war simulation do not invent
 * competing time sources. It deliberately performs no disk IO.
 *
 * Autosave has exactly two legal request sources: chapter boundaries and completed rest.
 * Manual save remains unrestricted. The lake-to-dock authored window suppresses all
 * autosave requests without disabling manual save.
 */
UCLASS()
class DARKARISEN_API UDarkArisenWorldRulesSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    UFUNCTION(BlueprintPure, Category="World|Time")
    int64 GetTotalWorldMinutes() const { return TotalWorldMinutes; }

    UFUNCTION(BlueprintPure, Category="World|Time")
    int32 GetMinuteOfDay() const;

    UFUNCTION(BlueprintPure, Category="World|Time")
    int32 GetDayNumber() const;

    UFUNCTION(BlueprintPure, Category="World|Time")
    int32 GetChapter() const { return Chapter; }

    UFUNCTION(BlueprintCallable, Category="World|Time")
    bool SetChapter(int32 NewChapter);

    /** Chapter boundary is one of exactly two legal autosave request sources. */
    UFUNCTION(BlueprintCallable, Category="World|Save")
    bool NotifyChapterBoundary(int32 NewChapter);

    /** Rest completion is one of exactly two legal autosave request sources. */
    UFUNCTION(BlueprintCallable, Category="World|Save")
    bool NotifyRestCompleted();

    UFUNCTION(BlueprintCallable, Category="World|Save")
    void BeginLakeToDockAutosaveSuppression();

    UFUNCTION(BlueprintCallable, Category="World|Save")
    void EndLakeToDockAutosaveSuppression();

    UFUNCTION(BlueprintPure, Category="World|Save")
    bool IsAutosaveSuppressed() const { return bAutosaveSuppressed; }

    UFUNCTION(BlueprintPure, Category="World|Save")
    bool CanManualSave() const { return true; }

    /**
     * Save authority calls this and performs the actual write. A suppressed request is
     * never queued, and consuming the request cannot itself write to disk.
     */
    UFUNCTION(BlueprintCallable, Category="World|Save")
    bool ConsumePendingAutosaveRequest();

private:
    void QueueLegalAutosaveRequest();

    UPROPERTY(SaveGame)
    int64 TotalWorldMinutes = 0;

    UPROPERTY(SaveGame)
    int32 Chapter = 4;

    UPROPERTY(SaveGame)
    bool bAutosaveSuppressed = false;

    UPROPERTY(Transient)
    bool bPendingAutosaveRequest = false;

    UPROPERTY(Transient)
    float FractionalWorldMinutes = 0.0f;

    static constexpr float RealSecondsPerGameHour = 150.0f;
    static constexpr float GameMinutesPerRealSecond = 60.0f / RealSecondsPerGameHour;
};
