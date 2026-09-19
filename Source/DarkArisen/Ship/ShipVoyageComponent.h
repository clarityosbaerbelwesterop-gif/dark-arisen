// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipVoyageComponent.generated.h"

UENUM(BlueprintType)
enum class EShipDeck : uint8
{
    Weather,
    Upper,
    Mid,
    Hold
};

UENUM(BlueprintType)
enum class EPointOfSail : uint8
{
    InIrons,
    CloseHauled,
    CloseReach,
    BeamReach,
    BroadReach,
    Running
};

UENUM(BlueprintType)
enum class ECrewWatch : uint8
{
    Dawn,
    Midday,
    Dusk,
    Night
};

USTRUCT(BlueprintType)
struct FShipVoyageSnapshot
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) bool bValid=false;
    UPROPERTY(SaveGame) FTransform WorldTransform;
    UPROPERTY(SaveGame) bool bOwnedAndUnlocked=false;
    UPROPERTY(SaveGame) float Throttle=0.f;
    UPROPERTY(SaveGame) float HullIntegrity=1000.f;
    UPROPERTY(SaveGame) float HeadingDegrees=0.f;
    UPROPERTY(SaveGame) float CommandedHeadingDegrees=0.f;
    UPROPERTY(SaveGame) float ForwardSpeedMetresPerSecond=0.f;
    UPROPERTY(SaveGame) float WindDirectionDegrees=90.f;
    UPROPERTY(SaveGame) float WindStrengthMetresPerSecond=6.f;
    UPROPERTY(SaveGame) bool bJakeInGreatCabin=false;
    UPROPERTY(SaveGame) int32 ActiveHands=40;
    UPROPERTY(SaveGame) TArray<FNamedCrewMemberState> NamedCrew;
    UPROPERTY(SaveGame) TArray<FPhysicalChartRecord> PhysicalCharts;
};

USTRUCT(BlueprintType)
struct FNamedCrewMemberState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Role;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECrewWatch PrimaryWatch = ECrewWatch::Dawn;

    UPROPERTY(SaveGame)
    bool bAlive = true;

    UPROPERTY(SaveGame)
    bool bAboard = true;
};

USTRUCT(BlueprintType)
struct FPhysicalChartRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ChartId;

    UPROPERTY(SaveGame, BlueprintReadOnly)
    bool bAcquired = false;

    UPROPERTY(SaveGame, BlueprintReadOnly)
    bool bJakeHasAnnotated = false;
};

/**
 * M3 source contract for La Liberacion.
 *
 * Canon boundaries:
 * - one singular, continuously walkable four-deck ship;
 * - point-of-sail, first-mate availability and hands drive movement;
 * - navigation is coast/chart/sky/guide based, never compass or player-dot based;
 * - charts are physical objects and may be annotated;
 * - the ship continues sailing when Jake leaves the helm;
 * - `InternalMorale` belongs exclusively to UShipHouseholdComponent and has no numeric UI getter;
 * - no water fast-travel API exists here.
 *
 * Geometry, buoyancy, authored crew animation, audio and runtime acceptance remain separate M3 gates.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UShipVoyageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShipVoyageComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Ship|Helm")
    void SetJakeAtHelm(bool bAtHelm);

    UFUNCTION(BlueprintCallable, Category="Ship|Ownership")
    bool SetOwnedAndUnlocked(bool bOwned);
    UFUNCTION(BlueprintPure, Category="Ship|Ownership")
    bool IsOwnedAndUnlocked() const { return bOwnedAndUnlocked; }
    UFUNCTION(BlueprintCallable, Category="Ship|Access")
    bool Embark(AActor* Character);
    UFUNCTION(BlueprintCallable, Category="Ship|Access")
    bool Disembark(AActor* Character);
    UFUNCTION(BlueprintCallable, Category="Ship|Helm")
    bool SetThrottle(float NewThrottle);
    UFUNCTION(BlueprintCallable, Category="Ship|Damage")
    void ApplyHullDamage(float Damage);
    UFUNCTION(BlueprintPure, Category="Ship|Damage")
    float GetHullIntegrity() const { return HullIntegrity; }

    UFUNCTION(BlueprintCallable, Category="Ship|Helm")
    void SetHelmCommandDegrees(float NewHeadingDegrees);

    UFUNCTION(BlueprintCallable, Category="Ship|Wind")
    void SetWind(float DirectionDegrees, float StrengthMetresPerSecond);

    UFUNCTION(BlueprintPure, Category="Ship|Wind")
    EPointOfSail GetPointOfSail() const { return PointOfSail; }

    UFUNCTION(BlueprintPure, Category="Ship|Movement")
    float GetForwardSpeedMetresPerSecond() const { return ForwardSpeedMetresPerSecond; }

    UFUNCTION(BlueprintPure, Category="Ship|Movement")
    float GetHeadingDegrees() const { return HeadingDegrees; }

    UFUNCTION(BlueprintPure, Category="Ship|Helm")
    bool IsJakeAtHelm() const { return bJakeAtHelm; }

    UFUNCTION(BlueprintPure, Category="Ship|Crew")
    TArray<FNamedCrewMemberState> GetNamedCrew() const { return NamedCrew; }

    UFUNCTION(BlueprintCallable, Category="Ship|Crew")
    void SetCrewMemberAboard(FName StableId, bool bAboard);

    UFUNCTION(BlueprintCallable, Category="Ship|Crew")
    void SetCrewMemberAlive(FName StableId, bool bAlive);

    UFUNCTION(BlueprintCallable, Category="Ship|Crew")
    void SetActiveHands(int32 NewHands);

    UFUNCTION(BlueprintPure, Category="Ship|Crew")
    int32 GetActiveHands() const { return ActiveHands; }

    UFUNCTION(BlueprintCallable, Category="Ship|Chart")
    bool AcquirePhysicalChart(FName ChartId);

    UFUNCTION(BlueprintCallable, Category="Ship|Chart")
    bool AnnotatePhysicalChart(FName ChartId);

    UFUNCTION(BlueprintPure, Category="Ship|Chart")
    bool HasPhysicalChart(FName ChartId) const;

    UFUNCTION(BlueprintCallable, Category="Ship|Cabin")
    void EnterGreatCabin() { bJakeInGreatCabin = true; }

    UFUNCTION(BlueprintCallable, Category="Ship|Cabin")
    void LeaveGreatCabin() { bJakeInGreatCabin = false; }

    UFUNCTION(BlueprintPure, Category="Ship|Cabin")
    bool IsJakeInGreatCabin() const { return bJakeInGreatCabin; }

    UFUNCTION(BlueprintCallable, Category="Ship|Persistence")
    FShipVoyageSnapshot CaptureSnapshot() const;
    UFUNCTION(BlueprintCallable, Category="Ship|Persistence")
    bool RestoreSnapshot(const FShipVoyageSnapshot& Snapshot);

    UFUNCTION(BlueprintPure, Category="Ship|Structure")
    static int32 GetRequiredDeckCount() { return 4; }

private:
    static float NormalizeHeading(float Degrees);
    void RecalculatePointOfSail();
    float GetSailEfficiency() const;
    float GetCrewHandlingFactor() const;
    void BuildCanonicalCrew();

    UPROPERTY(SaveGame)
    bool bOwnedAndUnlocked = false;
    UPROPERTY(SaveGame)
    float Throttle = 0.0f;
    UPROPERTY(SaveGame)
    float HullIntegrity = 1000.0f;
    UPROPERTY()
    TSet<TObjectPtr<AActor>> EmbarkedCharacters;

    UPROPERTY(SaveGame)
    float HeadingDegrees = 0.0f;

    UPROPERTY(SaveGame)
    float CommandedHeadingDegrees = 0.0f;

    UPROPERTY(SaveGame)
    float ForwardSpeedMetresPerSecond = 0.0f;

    UPROPERTY(SaveGame)
    float WindDirectionDegrees = 90.0f;

    UPROPERTY(SaveGame)
    float WindStrengthMetresPerSecond = 6.0f;

    UPROPERTY(SaveGame)
    bool bJakeAtHelm = false;

    UPROPERTY(SaveGame)
    bool bJakeInGreatCabin = false;

    UPROPERTY(SaveGame)
    EPointOfSail PointOfSail = EPointOfSail::BeamReach;

    UPROPERTY(SaveGame)
    TArray<FNamedCrewMemberState> NamedCrew;

    UPROPERTY(SaveGame)
    int32 ActiveHands = 40;

    UPROPERTY(SaveGame)
    TArray<FPhysicalChartRecord> PhysicalCharts;

    /** DESIGN-GAP: final hull-speed tuning requires authored vessel/runtime evidence. */
    UPROPERTY(EditDefaultsOnly, Category="Ship|Tuning", meta=(ClampMin="0.0"))
    float ProvisionalMaximumSpeedMetresPerSecond = 8.0f;

    /** DESIGN-GAP: helm response is provisional until the M3 sea-passage playtest. */
    UPROPERTY(EditDefaultsOnly, Category="Ship|Tuning", meta=(ClampMin="0.1"))
    float ProvisionalHeadingResponseDegreesPerSecond = 8.0f;
};
