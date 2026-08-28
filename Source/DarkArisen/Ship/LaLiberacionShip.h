// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship/ShipVoyageComponent.h"
#include "LaLiberacionShip.generated.h"

class USceneComponent;

/**
 * Level-placeable source boundary for La Liberacion.
 *
 * The four scene roots are not final ship geometry. They are stable authored attachment
 * boundaries for a continuous four-deck vessel: Weather, Upper, Mid and Hold. Final hull,
 * stairs/ladders, collision, buoyancy, roll/pitch/heel and streaming evidence remain M3 gates.
 * No deck transition uses a loading screen or teleport API.
 */
UCLASS()
class DARKARISEN_API ALaLiberacionShip : public AActor
{
    GENERATED_BODY()

public:
    ALaLiberacionShip();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Structure")
    TObjectPtr<USceneComponent> ShipRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Structure")
    TObjectPtr<USceneComponent> WeatherDeckRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Structure")
    TObjectPtr<USceneComponent> UpperDeckRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Structure")
    TObjectPtr<USceneComponent> MidDeckRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Structure")
    TObjectPtr<USceneComponent> HoldDeckRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Systems")
    TObjectPtr<UShipVoyageComponent> VoyageComponent;

    UFUNCTION(BlueprintPure, Category="Ship|Structure")
    USceneComponent* GetDeckRoot(EShipDeck Deck) const;

    /**
     * Great-cabin rest enters the canonical M4 rest/autosave path. It performs no disk write.
     * This fails closed unless Jake is already authored as physically inside the great cabin.
     */
    UFUNCTION(BlueprintCallable, Category="Ship|GreatCabin")
    bool CompleteGreatCabinRest();

    UFUNCTION(BlueprintPure, Category="Ship|GreatCabin")
    bool CanRestInGreatCabin() const;

private:
    bool HasCompleteFourDeckStructure() const;
};
