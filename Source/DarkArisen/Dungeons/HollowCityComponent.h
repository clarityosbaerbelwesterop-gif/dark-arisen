// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HollowCityComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/**
 * Unique authored state for the Hollow City Tier-C complex.
 *
 * The Bible locks four sub-areas, two bosses, interlocking shortcuts, the concealed eleven-square-
 * kilometre city image and an ancient map reward. It does not name the four sub-areas or a required
 * shortcut count, so this component uses numeric authored slots and stable shortcut IDs rather than
 * inventing names or a count.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UHollowCityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHollowCityComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Hollow City|Progress")
    bool MarkEnteredFromCulturalWebRoute();

    /** AuthoredIndex is one of the four source-locked sub-area slots: 0..3. */
    UFUNCTION(BlueprintCallable, Category="Hollow City|Progress")
    bool MarkSubAreaEntered(int32 AuthoredIndex);

    UFUNCTION(BlueprintPure, Category="Hollow City|Progress")
    bool HasVisitedAllFourSubAreas() const;

    UFUNCTION(BlueprintCallable, Category="Hollow City|Image")
    bool MarkCanopyHiddenCityImageWitnessed();

    UFUNCTION(BlueprintCallable, Category="Hollow City|Boss")
    bool ResolveWatcherAtGate();

    UFUNCTION(BlueprintCallable, Category="Hollow City|Boss")
    bool ResolveThingInReservoir();

    /** Count is intentionally not locked; each physical shortcut supplies a stable authored ID. */
    UFUNCTION(BlueprintCallable, Category="Hollow City|Traversal")
    bool OpenInterlockingShortcut(FName ShortcutId);

    UFUNCTION(BlueprintCallable, Category="Hollow City|Traversal")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintCallable, Category="Hollow City|Reward")
    bool RecoverAncientTierEMap();

    UFUNCTION(BlueprintPure, Category="Hollow City|Progress")
    bool IsDungeonStructurallyComplete() const;

    UFUNCTION(BlueprintPure, Category="Hollow City|Progress")
    bool IsContentChainComplete() const;

    static bool HasAllSubAreaBits(uint8 Mask);
    static constexpr int32 RequiredSubAreaCount = 4;
    static constexpr uint8 RequiredSubAreaMask = 0x0F;

private:
    bool BindSiteAuthority();
    void TryResolveBottomAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(SaveGame)
    uint8 VisitedSubAreaMask = 0;

    UPROPERTY(SaveGame)
    bool bWatcherAtGateResolved = false;

    UPROPERTY(SaveGame)
    bool bThingInReservoirResolved = false;

    UPROPERTY(SaveGame)
    TArray<FName> OpenedInterlockingShortcutIds;

    UPROPERTY(SaveGame)
    bool bAncientTierEMapRecovered = false;
};
