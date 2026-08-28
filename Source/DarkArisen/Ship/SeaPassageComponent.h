// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SeaPassageComponent.generated.h"

/**
 * M3 proof boundary for real sea passages.
 *
 * A passage can complete only after both authored world-time and physical-distance floors are
 * met. Completion changes state only; it never moves the vessel, loads another level, or
 * teleports the player. The ship must physically sail the distance in the current world.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API USeaPassageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USeaPassageComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Ship|Passage")
    bool BeginSeaPassage(FName RouteId, int32 MinimumGameMinutes, float MinimumDistanceMetres);

    UFUNCTION(BlueprintPure, Category="Ship|Passage")
    bool CanCompleteSeaPassage() const;

    UFUNCTION(BlueprintCallable, Category="Ship|Passage")
    bool CompleteSeaPassage();

    UFUNCTION(BlueprintPure, Category="Ship|Passage")
    bool IsSeaPassageActive() const { return bPassageActive; }

    UFUNCTION(BlueprintPure, Category="Ship|Passage")
    FName GetActiveRouteId() const { return ActiveRouteId; }

    UFUNCTION(BlueprintPure, Category="Ship|Passage")
    float GetSailedDistanceMetres() const { return SailedDistanceMetres; }

private:
    int64 GetCurrentWorldMinute() const;

    UPROPERTY(SaveGame)
    bool bPassageActive = false;

    UPROPERTY(SaveGame)
    FName ActiveRouteId;

    UPROPERTY(SaveGame)
    int64 PassageStartWorldMinute = 0;

    UPROPERTY(SaveGame)
    int32 RequiredGameMinutes = 0;

    UPROPERTY(SaveGame)
    float RequiredDistanceMetres = 0.0f;

    UPROPERTY(SaveGame)
    float SailedDistanceMetres = 0.0f;

    UPROPERTY(Transient)
    FVector LastObservedLocation = FVector::ZeroVector;

    UPROPERTY(Transient)
    bool bHasObservedLocation = false;
};
