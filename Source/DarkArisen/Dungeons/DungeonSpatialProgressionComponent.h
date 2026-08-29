// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DungeonSpatialProgressionComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/** Canonical spatial beats from dungeon_design_philosophy.md Section 4.1. */
UENUM(BlueprintType)
enum class EDungeonSpatialBeat : uint8
{
    Outside,
    Threshold,
    Descent,
    Turn,
    Depth,
    Floor,
    ReturnOpened
};

/**
 * Physical progression authority for the standard authored dungeon shape.
 *
 * It owns no coordinates, room sizes, enemies, puzzle answers or art. Those remain authored level
 * data. It only records that the player physically reached the six canonical beats in order and
 * routes the final inside-opened Return into UAuthoredDungeonSiteComponent.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UDungeonSpatialProgressionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDungeonSpatialProgressionComponent();
    virtual void BeginPlay() override;

    /**
     * Called only by authored physical trigger volumes. The common site must already be entered;
     * this component never bypasses discovery/trust/tide/quest entry gates.
     */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Spatial")
    bool RecordPhysicalBeat(EDungeonSpatialBeat Beat);

    UFUNCTION(BlueprintPure, Category="Dungeon|Spatial")
    EDungeonSpatialBeat GetCurrentBeat() const { return CurrentBeat; }

    UFUNCTION(BlueprintPure, Category="Dungeon|Spatial")
    bool HasReachedFloor() const
    {
        return static_cast<uint8>(CurrentBeat) >= static_cast<uint8>(EDungeonSpatialBeat::Floor);
    }

    UFUNCTION(BlueprintPure, Category="Dungeon|Spatial")
    bool HasOpenedReturnFromInside() const
    {
        return CurrentBeat == EDungeonSpatialBeat::ReturnOpened;
    }

    /** Strictly one-step progression; useful for native tests without a world. */
    static bool IsLegalNextBeat(EDungeonSpatialBeat Current, EDungeonSpatialBeat Proposed);

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(VisibleInstanceOnly, SaveGame, Category="Dungeon|Spatial")
    EDungeonSpatialBeat CurrentBeat = EDungeonSpatialBeat::Outside;
};
