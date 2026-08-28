// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ColonialWarStateSubsystem.generated.h"

UENUM(BlueprintType)
enum class EColonialFaction : uint8
{
    Imperial,
    Albion,
    Liberation,
    Crimson
};

UENUM(BlueprintType)
enum class EWarMomentumPhase : uint8
{
    Alpha,
    Beta,
    Gamma
};

UENUM(BlueprintType)
enum class ERegionalWarOutcome : uint8
{
    Contested,
    Crisis,
    ImperialControlled,
    AlbionControlled,
    Liberated,
    Vacuum,
    CrimsonOccupied
};

UENUM(BlueprintType)
enum class EWarActionVerb : uint8
{
    Support,
    Break,
    Liberate
};

USTRUCT(BlueprintType)
struct FRegionalWarSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ERegionalWarOutcome Outcome = ERegionalWarOutcome::Contested;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EColonialFaction DominantFaction = EColonialFaction::Imperial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCrisis = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFallAssaultCompleted = false;
};

/** Authored non-player pressure for one chapter. Rates are data, not invented engine constants. */
USTRUCT(BlueprintType)
struct FRegionalAutonomousWarTick
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ImperialDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 AlbionDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 LiberationDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 CrimsonDelta = 0;
};

USTRUCT()
struct FColonialRegionState
{
    GENERATED_BODY()

    UPROPERTY(SaveGame)
    FName RegionId;

    UPROPERTY(SaveGame)
    int32 ImperialControl = 0;

    UPROPERTY(SaveGame)
    int32 AlbionControl = 0;

    UPROPERTY(SaveGame)
    int32 LiberationStrength = 0;

    UPROPERTY(SaveGame)
    int32 CrimsonThreat = 0;

    UPROPERTY(SaveGame)
    ERegionalWarOutcome Outcome = ERegionalWarOutcome::Contested;

    UPROPERTY(SaveGame)
    bool bFallAssaultCompleted = false;
};

/**
 * Hidden M5 strategic authority. Numeric control values exist only as simulation state; callers
 * receive qualitative world outcomes rather than a player-facing meter. Physical missions report
 * their resolved consequences here. This subsystem never performs a mission, teleport or instant
 * conquest from UI input.
 */
UCLASS()
class DARKARISEN_API UColonialWarStateSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Authoring")
    bool RegisterRegion(FName RegionId, EColonialFaction InitialController);

    /** Report an already completed physical Support/Break/Liberate action. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Actions")
    bool RecordResolvedWarAction(FName RegionId, EWarActionVerb Verb, EColonialFaction TargetFaction,
        int32 ControlDelta, int32 LiberationDelta, int32 CrimsonDelta);

    /** The war also moves without Jake; one authored autonomous tick per region/chapter. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Simulation")
    bool RecordAutonomousChapterTick(FName RegionId, const FRegionalAutonomousWarTick& Tick);

    /** A Fall cannot resolve without this authored assault completion. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Siege")
    bool RecordFallAssaultCompleted(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Chapter")
    bool AdvanceChapter(int32 Chapter);

    UFUNCTION(BlueprintPure, Category="ColonialWar|Chapter")
    EWarMomentumPhase GetMomentumPhase() const { return MomentumPhase; }

    UFUNCTION(BlueprintPure, Category="ColonialWar|WorldRead")
    FRegionalWarSnapshot GetRegionSnapshot(FName RegionId) const;

    UFUNCTION(BlueprintPure, Category="ColonialWar|WorldRead")
    bool IsRegionLiberated(FName RegionId) const;

    /** Internal deterministic test/authoring access; never expose this as player UI. */
    int32 GetHiddenControlForTests(FName RegionId, EColonialFaction Faction) const;

private:
    static int32 ClampControl(int32 Value);
    static EWarMomentumPhase ResolveMomentumPhase(int32 Chapter);
    static bool IsValidAutonomousDelta(int32 Delta);
    void ReevaluateRegion(FColonialRegionState& Region);
    int32& ResolveFactionControl(FColonialRegionState& Region, EColonialFaction Faction);
    const int32& ResolveFactionControl(const FColonialRegionState& Region, EColonialFaction Faction) const;

    UPROPERTY(SaveGame)
    TMap<FName, FColonialRegionState> Regions;

    UPROPERTY(SaveGame)
    TMap<FName, int32> LastAutonomousTickChapter;

    UPROPERTY(SaveGame)
    int32 CurrentChapter = 4;

    UPROPERTY(SaveGame)
    EWarMomentumPhase MomentumPhase = EWarMomentumPhase::Alpha;
};
