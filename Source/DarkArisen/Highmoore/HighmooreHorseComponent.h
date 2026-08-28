// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighmooreHorseComponent.generated.h"

UENUM(BlueprintType)
enum class EHorseBondLevel : uint8
{
    Unknown = 0,
    Familiar = 1,
    Trusted = 2,
    Bonded = 3
};

UENUM(BlueprintType)
enum class EHorseGait : uint8
{
    Walk,
    Trot,
    Canter,
    Gallop
};

UENUM(BlueprintType)
enum class EHorseMoodRead : uint8
{
    Calm,
    Content,
    Tired,
    Frightened,
    Blown
};

UENUM(BlueprintType)
enum class EAuthoredRideSequence : uint8
{
    None,
    Lake,
    Arion,
    Belos,
    ReturnWalk
};

/**
 * M6 Highmoore horse. Bond changes handling, not a player-facing stat sheet. No Highmoore fast
 * travel lives here. The lake and revenge rides are authored real-time traversal contracts, and
 * permanent death/Arion-Belos exhaustion persist.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UHighmooreHorseComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHighmooreHorseComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool SetBondLevel(EHorseBondLevel NewBond);

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    EHorseBondLevel GetBondLevel() const { return BondLevel; }

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool SetGait(EHorseGait NewGait);

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    EHorseGait GetGait() const { return Gait; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    EHorseMoodRead GetMoodRead() const;

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool BeginLakeRide();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool BeginArionRide();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool ContinueArionRideToBelos();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool BeginReturnWalk();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool CompleteAuthoredRide();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Horse")
    bool RecordPermanentDeath();

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    bool IsAlive() const { return bAlive; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    bool HasLastingRevengeRideDamage() const { return bLastingRevengeRideDamage; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    EAuthoredRideSequence GetActiveRide() const { return ActiveRide; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Horse")
    float GetRideElapsedSeconds() const { return RideElapsedSeconds; }

    static constexpr float LakeRideRequiredSeconds = 9.0f * 60.0f;
    static constexpr float ArionRideRequiredSeconds = 4.0f * 60.0f;
    static constexpr float BelosRideRequiredSeconds = 6.0f * 60.0f;
    static constexpr float ReturnWalkRequiredSeconds = 40.0f * 60.0f;

private:
    bool BeginRide(EAuthoredRideSequence Ride, EHorseGait RequiredGait, float RequiredSeconds,
        bool bSuspendStamina);
    float GetRequiredRideSeconds() const;

    UPROPERTY(SaveGame)
    EHorseBondLevel BondLevel = EHorseBondLevel::Unknown;

    UPROPERTY(SaveGame)
    EHorseGait Gait = EHorseGait::Walk;

    UPROPERTY(SaveGame)
    bool bAlive = true;

    UPROPERTY(SaveGame)
    bool bLastingRevengeRideDamage = false;

    UPROPERTY(SaveGame)
    float InternalStamina = 100.0f;

    UPROPERTY(Transient)
    EAuthoredRideSequence ActiveRide = EAuthoredRideSequence::None;

    UPROPERTY(Transient)
    float RideElapsedSeconds = 0.0f;

    UPROPERTY(Transient)
    bool bStaminaSuspendedForAuthoredRide = false;

    UPROPERTY(Transient)
    float ActiveRideRequiredSeconds = 0.0f;
};
