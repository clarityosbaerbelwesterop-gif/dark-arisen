// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CenoteFirstMotherComponent.generated.h"

/**
 * Saveable authoring state for the Cenote's required water, image, boss and Return beats.
 * Discovery, progress and completion deliberately emit no map, HUD, sound or camera event.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UCenoteFirstMotherComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCenoteFirstMotherComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Cenote|Progress")
    bool MarkEnteredAtSinkholeLip();

    UFUNCTION(BlueprintCallable, Category = "Cenote|Progress")
    bool MarkWaterRoutingSolved();

    UFUNCTION(BlueprintCallable, Category = "Cenote|Image")
    void SetPlayerInImageChamber(bool bInChamber);

    /** The world-time owner supplies the catalog's exact one-hour sunlight window. */
    UFUNCTION(BlueprintCallable, Category = "Cenote|Image")
    void SetGreenGoldSunlightWindowActive(bool bActive);

    UFUNCTION(BlueprintCallable, Category = "Cenote|Progress")
    bool ResolveKeeperBelow(FName OutcomeId);

    UFUNCTION(BlueprintCallable, Category = "Cenote|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintPure, Category = "Cenote|Progress")
    bool IsDungeonComplete() const;

    static float EvaluateGreenGoldWitnessSeconds(
        float CurrentSeconds,
        float DeltaSeconds,
        bool bInImageChamber,
        bool bSunlightWindowActive,
        float RequiredSeconds);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    bool bEntered = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    bool bWaterRoutingSolved = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    bool bGreenGoldImageWitnessed = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    bool bKeeperEncounterResolved = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    FName KeeperOutcomeId;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Cenote|Progress")
    bool bReturnShortcutOpened = false;

    /** DESIGN-GAP: the catalog locks a one-hour world window, not gaze duration. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cenote|Tuning",
        meta = (ClampMin = "0.1"))
    float RequiredImageWitnessSeconds = 5.0f;

private:
    float GreenGoldWitnessSeconds = 0.0f;
    bool bPlayerInImageChamber = false;
    bool bGreenGoldSunlightWindowActive = false;
    void RefreshTickState();
};
