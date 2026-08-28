// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ship/ShipVoyageComponent.h"
#include "ShipHouseholdComponent.generated.h"

UENUM(BlueprintType)
enum class ECrewHouseholdActivity : uint8
{
    Watch,
    Navigation,
    Forge,
    Surgery,
    Quiet,
    Meal,
    OffDuty
};

UENUM(BlueprintType)
enum class ECrewMoraleRead : uint8
{
    High,
    Fed,
    Strained,
    Bad,
    RecentLoss
};

UENUM(BlueprintType)
enum class ECrewMoraleCue : uint8
{
    SingingMidDeck,
    SharedMending,
    GalleyFire,
    UnbrokenArgument,
    EmptyForecastleAtDusk,
    EmptyHammock
};

USTRUCT(BlueprintType)
struct FCrewHouseholdRead
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName CrewId;

    UPROPERTY(BlueprintReadOnly)
    EShipDeck Deck = EShipDeck::Weather;

    UPROPERTY(BlueprintReadOnly)
    ECrewHouseholdActivity Activity = ECrewHouseholdActivity::Watch;
};

/**
 * M3 household authority for La Liberacion.
 *
 * The crew are never a party/loadout. This component converts the canonical world clock into
 * deck activity, meals and watches and exposes morale only through authored world-readable cues.
 * The numeric morale value is intentionally private and has no Blueprint getter.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UShipHouseholdComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShipHouseholdComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    bool ApplyGameMinute(int64 GameMinute);

    UFUNCTION(BlueprintPure, Category="Ship|Household")
    FCrewHouseholdRead GetNamedCrewRead(FName CrewId) const;

    UFUNCTION(BlueprintPure, Category="Ship|Household")
    ECrewMoraleRead GetMoraleRead() const;

    UFUNCTION(BlueprintPure, Category="Ship|Household")
    ECrewMoraleCue GetVisibleMoraleCue() const;

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    void RecordCrewSharePaid(bool bPaidInFull);

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    void RecordCrewLoss(FName StableCrewId);

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    void RecordSharedRepairMoment();

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    void RecordStrainingChoice();

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    bool SetSpecialistBerthsOccupied(int32 Count);

    UFUNCTION(BlueprintCallable, Category="Ship|Household")
    bool SetSemiNamedHands(int32 Count);

    UFUNCTION(BlueprintPure, Category="Ship|Household")
    int32 GetSpecialistBerthsOccupied() const { return SpecialistBerthsOccupied; }

    UFUNCTION(BlueprintPure, Category="Ship|Household")
    int32 GetSemiNamedHands() const { return SemiNamedHands; }

private:
    void RebuildNamedReads(int32 MinuteOfDay);
    bool IsMealWindow(int32 MinuteOfDay) const;
    bool IsDusk(int32 MinuteOfDay) const;
    void ClampMorale();

    UPROPERTY(Transient)
    TMap<FName, FCrewHouseholdRead> NamedReads;

    UPROPERTY(SaveGame)
    int32 SpecialistBerthsOccupied = 0;

    UPROPERTY(SaveGame)
    int32 SemiNamedHands = 12;

    UPROPERTY(SaveGame)
    TSet<FName> RecentLosses;

    /** Internal world-state only. Never display this value or expose it as an affinity meter. */
    UPROPERTY(SaveGame)
    int32 InternalMorale = 55;

    UPROPERTY(SaveGame)
    bool bCrewShareCurrentlyPaid = true;

    UPROPERTY(SaveGame)
    int64 LastAppliedGameMinute = -1;
};
