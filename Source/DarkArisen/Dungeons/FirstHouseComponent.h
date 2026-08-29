// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FirstHouseComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/**
 * Region 06 — The First House.
 *
 * Canon is intentionally sparse: open door, wrong Fjordlund tide grammar, no hazard, half-eaten
 * unspoiled meal for six, no boss, no reward, no explanation. This component keeps it that way.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UFirstHouseComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFirstHouseComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="First House|Progress")
    bool MarkEnteredThroughOpenDoor();

    UFUNCTION(BlueprintCallable, Category="First House|Image")
    bool MarkHalfEatenMealWitnessed();

    /** Resolves the deliberately empty bottom state; there is no fight. */
    UFUNCTION(BlueprintCallable, Category="First House|Progress")
    bool ResolveEmptyBottom();

    UFUNCTION(BlueprintCallable, Category="First House|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintPure, Category="First House|Law")
    bool HasHazard() const { return false; }

    UFUNCTION(BlueprintPure, Category="First House|Law")
    bool HasBoss() const { return false; }

    UFUNCTION(BlueprintPure, Category="First House|Law")
    bool HasReward() const { return false; }

    UFUNCTION(BlueprintPure, Category="First House|Progress")
    bool IsDungeonComplete() const;

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;
};
