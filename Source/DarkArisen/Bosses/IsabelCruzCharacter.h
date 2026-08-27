// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IsabelCruzCharacter.generated.h"

class UCombatComponent;
class UHealthComponent;
class UStaticMeshComponent;
class UStaminaComponent;

UENUM(BlueprintType)
enum class EIsabelCruzPhase : uint8
{
    Duty,
    List,
    EighteenFortySix UMETA(DisplayName = "1846"),
    Resolved
};

UENUM(BlueprintType)
enum class EBossEncounterResolution : uint8
{
    Unresolved,
    Killed,
    Spared,
    Avoided
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnIsabelCruzPhaseChanged,
    EIsabelCruzPhase,
    PreviousPhase,
    EIsabelCruzPhase,
    NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnBossEncounterResolved,
    EBossEncounterResolution,
    Resolution);

/**
 * Native M2 proof-boss state. Authored dialogue, animations, arena geometry and rewards remain
 * separate acceptance gates and are deliberately not inferred from this actor.
 */
UCLASS()
class DARKARISEN_API AIsabelCruzCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AIsabelCruzCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Isabel Cruz|Encounter")
    bool BeginEncounter(ACharacter* PlayerCharacter);

    /** Mercy is available only during the authored posture-break vulnerability. */
    UFUNCTION(BlueprintCallable, Category = "Isabel Cruz|Encounter")
    bool TrySpare();

    /** External fort-state proof may resolve the route before combat begins. */
    UFUNCTION(BlueprintCallable, Category = "Isabel Cruz|Encounter")
    bool ResolveAvoided(bool bFortSurrenderedWithoutCombat);

    UFUNCTION(BlueprintPure, Category = "Isabel Cruz|Encounter")
    bool CanBeSpared() const;

    static constexpr float ListPhaseHealthFraction = 0.65f;
    static constexpr float EighteenFortySixPhaseHealthFraction = 0.30f;
    static EIsabelCruzPhase EvaluatePhase(float HealthFraction);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Isabel Cruz|Components")
    TObjectPtr<UHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Isabel Cruz|Components")
    TObjectPtr<UStaminaComponent> StaminaComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Isabel Cruz|Components")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Isabel Cruz|Components")
    TObjectPtr<UStaticMeshComponent> GreyboxBody;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame,
        Category = "Isabel Cruz|Encounter")
    EIsabelCruzPhase CurrentPhase = EIsabelCruzPhase::Duty;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame,
        Category = "Isabel Cruz|Encounter")
    EBossEncounterResolution Resolution = EBossEncounterResolution::Unresolved;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame,
        Category = "Isabel Cruz|Encounter")
    bool bEncounterActive = false;

    /** DESIGN-GAP: encounter-start radius awaits the authored fort layout. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Isabel Cruz|Tuning",
        meta = (ClampMin = "100.0"))
    float AwarenessRangeCentimetres = 1200.0f;

    /** DESIGN-GAP: weapon reach awaits final animation traces. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Isabel Cruz|Tuning",
        meta = (ClampMin = "100.0"))
    float AttackRangeCentimetres = 220.0f;

    /** DESIGN-GAP: cadence awaits the no-cancel controller feel pass. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Isabel Cruz|Tuning",
        meta = (ClampMin = "0.0"))
    float AttackCooldownSeconds = 1.1f;

    UPROPERTY(BlueprintAssignable, Category = "Isabel Cruz|Events")
    FOnIsabelCruzPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Isabel Cruz|Events")
    FOnBossEncounterResolved OnEncounterResolved;

private:
    UPROPERTY()
    TObjectPtr<ACharacter> TargetCharacter;

    float AttackCooldownRemaining = 0.0f;

    UFUNCTION()
    void OnHealthChanged(float NewHealth, float MaximumHealth, float Delta);

    UFUNCTION()
    void OnCruzDied(AActor* DamageCauser);

    void SetPhase(EIsabelCruzPhase NewPhase);
    void ResolveEncounter(EBossEncounterResolution NewResolution);
};
