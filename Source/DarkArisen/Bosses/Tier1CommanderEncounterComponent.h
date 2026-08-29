// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bosses/Tier1CommanderCatalog.h"
#include "Tier1CommanderEncounterComponent.generated.h"

UENUM(BlueprintType)
enum class ETier1CommanderResolution : uint8
{
    Unresolved,
    Killed,
    Spared,
    Avoided
};

/**
 * Shared resolution/phase state for the Nine Who Hold. Combat, siege, ship, heat, ice, fire and war
 * consequences remain owned by their existing systems; this component only enforces the authored
 * encounter contract and special-case resolution laws.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UTier1CommanderEncounterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTier1CommanderEncounterComponent();
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tier1|Definition")
    FName CommanderStableId;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Tier1|State")
    bool bEncounterBegun = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Tier1|State")
    int32 CurrentPhaseIndex = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Tier1|State")
    ETier1CommanderResolution Resolution = ETier1CommanderResolution::Unresolved;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Tier1|State")
    FName AvoidedRouteId;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Tier1|State")
    bool bSpareRefusedObserved = false;

    bool InitializeDefinition();

    UFUNCTION(BlueprintCallable, Category="Tier1|State")
    bool BeginEncounter();

    /** Health-band bosses only. Health fraction is clamped to 0..1. */
    UFUNCTION(BlueprintCallable, Category="Tier1|State")
    bool UpdateHealthFraction(float HealthFraction);

    /** Negotiation/pursuit/naval encounters advance only through their authored ordered stage IDs. */
    UFUNCTION(BlueprintCallable, Category="Tier1|State")
    bool AdvanceScriptedStage(FName StageId);

    UFUNCTION(BlueprintCallable, Category="Tier1|Resolution")
    bool ResolveKilled();

    UFUNCTION(BlueprintCallable, Category="Tier1|Resolution")
    bool TryResolveSpared();

    UFUNCTION(BlueprintCallable, Category="Tier1|Resolution")
    bool ResolveAvoided(FName RouteId);

    UFUNCTION(BlueprintPure, Category="Tier1|Definition")
    FName GetCurrentPhaseId() const;

    UFUNCTION(BlueprintPure, Category="Tier1|Definition")
    bool IsDirectCombatant() const { return bDefinitionValid && Definition.bDirectCombatant; }

    UFUNCTION(BlueprintPure, Category="Tier1|Definition")
    bool IsDefinitionValid() const { return bDefinitionValid; }

private:
    bool bDefinitionValid = false;
    FTier1CommanderDefinition Definition;

    bool ResolveInternal(ETier1CommanderResolution NewResolution, FName RouteId = NAME_None);
};
