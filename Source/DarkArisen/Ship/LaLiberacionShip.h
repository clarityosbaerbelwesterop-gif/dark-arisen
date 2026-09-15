// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship/ShipVoyageComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"
#include "LaLiberacionShip.generated.h"

class APhysicalMapActor;
class USceneComponent;
class USeaPassageComponent;
class UShipHouseholdComponent;
class UNavalCombatComponent;

/** One continuous, physically traversable La Liberacion with native voyage and combat authority. */
UCLASS()
class DARKARISEN_API ALaLiberacionShip : public AActor
{
    GENERATED_BODY()
public:
    ALaLiberacionShip();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Structure") TObjectPtr<USceneComponent> ShipRoot;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Structure") TObjectPtr<USceneComponent> WeatherDeckRoot;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Structure") TObjectPtr<USceneComponent> UpperDeckRoot;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Structure") TObjectPtr<USceneComponent> MidDeckRoot;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Structure") TObjectPtr<USceneComponent> HoldDeckRoot;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Systems") TObjectPtr<UShipVoyageComponent> VoyageComponent;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Systems") TObjectPtr<UShipHouseholdComponent> HouseholdComponent;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Systems") TObjectPtr<USeaPassageComponent> SeaPassageComponent;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ship|Systems") TObjectPtr<UNavalCombatComponent> NavalCombatComponent;
    UPROPERTY(EditDefaultsOnly,Category="Ship|Map") TSubclassOf<APhysicalMapActor> PhysicalMapClass;

    UFUNCTION(BlueprintPure,Category="Ship|Structure") USceneComponent* GetDeckRoot(EShipDeck Deck) const;
    UFUNCTION(BlueprintPure,Category="Ship|Map") APhysicalMapActor* GetPhysicalMap() const{return PhysicalMap;}
    UFUNCTION(BlueprintCallable,Category="Ship|GreatCabin") bool CompleteGreatCabinRest();
    UFUNCTION(BlueprintCallable,Category="Ship|GreatCabin") bool RestGreatCabinToDaypart(EDarkArisenDaypart TargetDaypart);
    UFUNCTION(BlueprintPure,Category="Ship|GreatCabin") bool CanRestInGreatCabin() const;

private:
    bool HasCompleteFourDeckStructure() const;
    bool SpawnPhysicalMap();
    UPROPERTY(Transient) TObjectPtr<APhysicalMapActor> PhysicalMap;
};
