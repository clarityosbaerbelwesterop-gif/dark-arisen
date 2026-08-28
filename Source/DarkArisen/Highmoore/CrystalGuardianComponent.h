// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrystalGuardianComponent.generated.h"

UENUM(BlueprintType)
enum class ECrystalGuardianPhase : uint8
{
    Dormant,
    Threshold,
    Columns,
    Span,
    LastStand,
    Settling,
    Defeated,
    Bypassed
};

/**
 * M6 Crystal Guardian source authority. It guards a threshold, does not attack first, does not
 * pursue beyond it and consumes explicit light stimuli rather than body/player targets. The boss
 * may be bypassed; that path deliberately yields no Crystal Katana.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UCrystalGuardianComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCrystalGuardianComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Highmoore|Guardian")
    bool EngageGuardian();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Guardian")
    bool RecordLightStimulus(FVector WorldLocation, float Intensity);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Guardian")
    bool ApplyGuardianHealthPercent(float RemainingPercent);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Guardian")
    bool RecordGuardianBypassed();

    /** Starts the authored four-second settle/hold. No slow motion or sting is owned here. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Guardian")
    bool BeginDefeatSettle();

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    ECrystalGuardianPhase GetPhase() const { return Phase; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    bool HasBeenDefeated() const { return Phase == ECrystalGuardianPhase::Defeated; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    bool WasBypassed() const { return Phase == ECrystalGuardianPhase::Bypassed; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    bool CanYieldCrystalKatana() const { return Phase == ECrystalGuardianPhase::Defeated; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    FVector GetLastLightStimulusLocation() const { return LastLightStimulusLocation; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Guardian")
    bool HasActiveLightStimulus() const { return bHasActiveLightStimulus; }

    static constexpr float DefeatHoldSeconds = 4.0f;

private:
    static ECrystalGuardianPhase ResolveCombatPhase(float RemainingPercent);

    UPROPERTY(SaveGame)
    ECrystalGuardianPhase Phase = ECrystalGuardianPhase::Dormant;

    UPROPERTY(SaveGame)
    float HealthPercent = 100.0f;

    UPROPERTY(Transient)
    FVector LastLightStimulusLocation = FVector::ZeroVector;

    UPROPERTY(Transient)
    bool bHasActiveLightStimulus = false;

    UPROPERTY(Transient)
    float SettleElapsedSeconds = 0.0f;
};
