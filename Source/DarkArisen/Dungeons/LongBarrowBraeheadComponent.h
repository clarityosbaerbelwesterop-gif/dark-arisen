// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LongBarrowBraeheadComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/**
 * Highmoore — The Long Barrow at Braehead.
 *
 * House Brae access is relational. The old script is illegible outside and legible inside. There is
 * no boss. The source does not lock a reward, so this owner deliberately exposes none.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API ULongBarrowBraeheadComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULongBarrowBraeheadComponent();
    virtual void BeginPlay() override;

    /** The real House-Brae relationship/quest authority supplies this access fact. */
    UFUNCTION(BlueprintCallable, Category="Long Barrow|Progress")
    bool MarkEnteredWithBraePermission(bool bHouseBraeAccessGranted);

    /** Physical arrival inside makes the records legible and resolves the no-boss bottom. */
    UFUNCTION(BlueprintCallable, Category="Long Barrow|Progress")
    bool ReachLegibleInteriorRecords();

    UFUNCTION(BlueprintCallable, Category="Long Barrow|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintPure, Category="Long Barrow|Law")
    bool HasBoss() const { return false; }

    UFUNCTION(BlueprintPure, Category="Long Barrow|Law")
    bool HasAuthoredReward() const { return false; }

    UFUNCTION(BlueprintPure, Category="Long Barrow|Progress")
    bool IsDungeonComplete() const;

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(SaveGame)
    bool bInteriorRecordsReached = false;
};
