// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StoneFieldUnderChamberComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/**
 * Fjordlund — The Stone Field's Under-Chamber.
 *
 * There is no boss. The thousands of sea-taken names are both the unforgettable image and the
 * authored bottom state. If Bjorn is already dead, his name is present; this component consumes
 * that fact from the real death-state authority and creates no marker or notification.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UStoneFieldUnderChamberComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStoneFieldUnderChamberComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Stone Field|Progress")
    bool MarkEnteredUnderStandingStones();

    /** Physical arrival at the names wall resolves image + no-boss bottom together. */
    UFUNCTION(BlueprintCallable, Category="Stone Field|Progress")
    bool ReachNamesWall(bool bBjornDeadAccordingToAuthority);

    UFUNCTION(BlueprintCallable, Category="Stone Field|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintPure, Category="Stone Field|State")
    bool WasBjornsNamePresentAtWitness() const { return bBjornsNamePresentAtWitness; }

    UFUNCTION(BlueprintPure, Category="Stone Field|Law")
    bool HasBoss() const { return false; }

    UFUNCTION(BlueprintPure, Category="Stone Field|Progress")
    bool IsDungeonComplete() const;

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(SaveGame)
    bool bNamesWallReached = false;

    UPROPERTY(SaveGame)
    bool bBjornsNamePresentAtWitness = false;
};
