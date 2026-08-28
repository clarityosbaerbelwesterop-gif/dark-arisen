// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrystalCavesPassageComponent.generated.h"

UENUM(BlueprintType)
enum class ECrystalCavesPassageState : uint8
{
    Outside,
    FirstPassage,
    HighmooreOpen,
    RepeatPassage
};

/**
 * M6 physical Crystal-Caves route. The first passage and every repeat passage require elapsed
 * real traversal time plus observed physical movement. This component owns no OpenLevel,
 * ServerTravel, teleport or map-click transition path.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UCrystalCavesPassageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCrystalCavesPassageComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Highmoore|Caves")
    bool BeginFirstPassage(float AuthoredMinimumDistanceMetres);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Caves")
    bool CompleteFirstPassage();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Caves")
    bool BeginRepeatPassage(float AuthoredMinimumDistanceMetres, float AuthoredRepeatRealSeconds);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Caves")
    bool CompleteRepeatPassage();

    UFUNCTION(BlueprintPure, Category="Highmoore|Caves")
    bool IsHighmoorePermanentlyOpen() const { return bHighmoorePermanentlyOpen; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Caves")
    ECrystalCavesPassageState GetPassageState() const { return State; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Caves")
    float GetObservedDistanceMetres() const { return ObservedDistanceMetres; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Caves")
    float GetElapsedPassageSeconds() const { return ElapsedPassageSeconds; }

    static constexpr float FirstPassageMinimumRealSeconds = 90.0f * 60.0f;
    static constexpr float RepeatPassageMinimumRealSeconds = 4.0f * 60.0f;
    static constexpr float RepeatPassageMaximumAuthoredRealSeconds = 6.0f * 60.0f;

private:
    bool CanCompleteActivePassage() const;
    void ResetObservedTraversal();

    UPROPERTY(SaveGame)
    bool bHighmoorePermanentlyOpen = false;

    UPROPERTY(SaveGame)
    ECrystalCavesPassageState State = ECrystalCavesPassageState::Outside;

    UPROPERTY(Transient)
    float ElapsedPassageSeconds = 0.0f;

    UPROPERTY(Transient)
    float ObservedDistanceMetres = 0.0f;

    UPROPERTY(Transient)
    float RequiredRealSeconds = 0.0f;

    UPROPERTY(Transient)
    float RequiredDistanceMetres = 0.0f;

    UPROPERTY(Transient)
    FVector LastObservedLocation = FVector::ZeroVector;

    UPROPERTY(Transient)
    bool bHasObservedLocation = false;
};
