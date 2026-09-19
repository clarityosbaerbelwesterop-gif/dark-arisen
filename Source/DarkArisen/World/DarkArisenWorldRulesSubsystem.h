// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DarkArisenWorldRulesSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDarkArisenRestLocation : uint8
{
    GreatCabin,
    SafeHouse
};

UENUM(BlueprintType)
enum class EDarkArisenDaypart : uint8
{
    Dawn,
    Midday,
    Dusk,
    Night
};

USTRUCT(BlueprintType)
struct FDarkArisenWorldRulesSnapshot
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) bool bValid=false;
    UPROPERTY(SaveGame) int64 TotalWorldMinutes=0;
    UPROPERTY(SaveGame) int32 Chapter=1;
    UPROPERTY(SaveGame) bool bAutosaveSuppressed=false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnDarkArisenChapterBoundary,
    int32,
    NewChapter);

/**
 * Canonical M4 world-time/save owner.
 *
 * One in-game hour = 150 real seconds. This subsystem owns the clock so settlements, tides,
 * ship watches and later war simulation never invent competing clocks. It performs no disk IO.
 *
 * Autosave has exactly two legal request sources: chapter boundaries and completed rest.
 * Manual save remains unrestricted. The lake-to-dock window suppresses autosaves only.
 */
UCLASS()
class DARKARISEN_API UDarkArisenWorldRulesSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    UPROPERTY(BlueprintAssignable, Category="World|Chapter")
    FOnDarkArisenChapterBoundary OnChapterBoundary;

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

    /** Compatibility path for an already-completed authored rest. */
    UFUNCTION(BlueprintCallable, Category="World|Save")
    bool NotifyRestCompleted();

    /**
     * Canonical rest entry: only great cabin or safe house, and only one of four dayparts.
     * No arbitrary wait-until-hour or bedroll path exists.
     */
    UFUNCTION(BlueprintCallable, Category="World|Rest")
    bool CompleteRest(EDarkArisenRestLocation Location, EDarkArisenDaypart TargetDaypart);

    UFUNCTION(BlueprintCallable, Category="World|Save")
    void BeginLakeToDockAutosaveSuppression();

    UFUNCTION(BlueprintCallable, Category="World|Save")
    void EndLakeToDockAutosaveSuppression();

    UFUNCTION(BlueprintPure, Category="World|Save")
    bool IsAutosaveSuppressed() const { return bAutosaveSuppressed; }

    UFUNCTION(BlueprintCallable, Category="World|Persistence")
    FDarkArisenWorldRulesSnapshot CaptureSnapshot() const;
    UFUNCTION(BlueprintCallable, Category="World|Persistence")
    bool RestoreSnapshot(const FDarkArisenWorldRulesSnapshot& Snapshot);

    UFUNCTION(BlueprintPure, Category="World|Save")
    bool CanManualSave() const { return true; }

    /** Save authority consumes this request and performs the actual write. */
    UFUNCTION(BlueprintCallable, Category="World|Save")
    bool ConsumePendingAutosaveRequest();

private:
    void QueueLegalAutosaveRequest();
    static int32 GetDaypartMinute(EDarkArisenDaypart TargetDaypart);

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
