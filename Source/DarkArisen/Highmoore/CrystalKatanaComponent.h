// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrystalKatanaComponent.generated.h"

UENUM(BlueprintType)
enum class ECrystalKatanaLightRead : uint8
{
    Resting,
    Moving
};

/**
 * M6 Crystal Katana contract. No durability, repair, sharpening or upgrade state exists. Attack
 * stamina cost is deliberately condition-independent: callers provide the authored base cost and
 * receive that exact cost regardless of Jake's wounds, health, stamina or posture.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UCrystalKatanaComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCrystalKatanaComponent();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Katana")
    bool AcquireFromDefeatedGuardian(bool bGuardianDefeated);

    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    bool IsAcquired() const { return bAcquired; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    float GetBladeLengthCentimetres() const { return BladeLengthCentimetres; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    float GetWeightKilograms() const { return WeightKilograms; }

    /** Flat by law: condition parameters are intentionally absent. */
    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    float ResolveFlatAttackStaminaCost(float AuthoredBaseCost) const;

    UFUNCTION(BlueprintCallable, Category="Highmoore|Katana")
    void SetWeaponInMotion(bool bInMotion);

    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    ECrystalKatanaLightRead GetLightRead() const { return LightRead; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Katana")
    bool IgnoresConventionalArmour() const { return bAcquired; }

private:
    UPROPERTY(SaveGame)
    bool bAcquired = false;

    UPROPERTY(SaveGame)
    ECrystalKatanaLightRead LightRead = ECrystalKatanaLightRead::Resting;

    static constexpr float BladeLengthCentimetres = 74.0f;
    static constexpr float WeightKilograms = 1.1f;
};
