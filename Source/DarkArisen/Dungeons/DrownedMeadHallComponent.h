// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrownedMeadHallComponent.generated.h"

class UAuthoredDungeonSiteComponent;
class UDarkArisenWorldRulesSubsystem;

/**
 * Unique authored mechanics for the Drowned Mead Hall.
 *
 * Common entered/image/bottom/Return state remains owned by UAuthoredDungeonSiteComponent.
 * This component owns only the twice-daily forty-minute dry window and the drinking-horn reward.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UDrownedMeadHallComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDrownedMeadHallComponent();
    virtual void BeginPlay() override;

    /**
     * DESIGN-GAP: the Bible locks two dry windows per day and forty minutes of dry access, but not
     * the first window's clock time. -1 keeps the tide gate fail-closed until level authoring sets it.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drowned Mead Hall|Tide",
        meta=(ClampMin="-1", ClampMax="1439"))
    int32 AuthoredFirstDryWindowStartMinute = -1;

    UFUNCTION(BlueprintPure, Category="Drowned Mead Hall|Tide")
    bool IsDryWindowActive() const;

    UFUNCTION(BlueprintCallable, Category="Drowned Mead Hall|Progress")
    bool MarkEnteredAtLowTide();

    UFUNCTION(BlueprintCallable, Category="Drowned Mead Hall|Progress")
    bool MarkDrainedHallImageWitnessed();

    UFUNCTION(BlueprintCallable, Category="Drowned Mead Hall|Progress")
    bool ResolveLastGuest(FName OutcomeId);

    UFUNCTION(BlueprintCallable, Category="Drowned Mead Hall|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintCallable, Category="Drowned Mead Hall|Reward")
    bool RecoverDrinkingHorn();

    UFUNCTION(BlueprintPure, Category="Drowned Mead Hall|Progress")
    bool IsDungeonStructurallyComplete() const;

    UFUNCTION(BlueprintPure, Category="Drowned Mead Hall|Reward")
    bool HasDrinkingHorn() const { return bDrinkingHornRecovered; }

    static bool IsDryWindowMinute(int32 MinuteOfDay, int32 FirstWindowStartMinute);

    static constexpr int32 DryWindowDurationWorldMinutes = 40;
    static constexpr int32 DryWindowSeparationWorldMinutes = 12 * 60;
    static constexpr int32 WorldMinutesPerDay = 24 * 60;

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(SaveGame)
    bool bDrinkingHornRecovered = false;
};
