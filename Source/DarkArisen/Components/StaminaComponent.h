// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

/** HUD-Bar, Audio ("keuchen"), VFX (Schweißfilter, Kameraschwingen bei niedrigem Wert). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature,
	float, NewStamina, float, MaxStamina);

/** Triggert den Exhausted-Stagger und sperrt Offensiv-Moves bis CurrentStamina > 0. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepletedSignature);

/**
 * UStaminaComponent — Ressourcen-Management für alle Combat-Aktionen.
 *
 * Umsetzt GDD §6.1 Stamina-Regel: "All actions cost stamina. Management is
 * the core skill. Regenerates when idle." + Parry-Regel: "Perfect parry
 * restores stamina." + §11.2 Sea Legs Brew Tinktur: "Stamina regen +50 %".
 *
 * Designregeln:
 *  - Nach jedem Verbrauch startet eine RegenDelay-Sperre, bevor die
 *    Regeneration wieder einsetzt. Das verhindert endloses Button-Mashing
 *    und erzwingt den Souls-typischen "atmen-dann-angreifen"-Rhythmus.
 *  - Sprint zieht kontinuierlich, solange bSprinting aktiv ist.
 *  - Ein Perfect Parry umgeht den Delay und restored zusätzlich eine feste Menge.
 *  - Bei 0 feuert OnStaminaDepleted genau einmal; der CombatComponent erzwingt
 *    daraufhin einen kurzen Exhausted-Stagger-State.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Konstruktor — aktiviert Tick und setzt Default-Werte. */
	UStaminaComponent();

protected:
	/** Setzt CurrentStamina = MaxStamina beim ersten Spawn. */
	virtual void BeginPlay() override;

public:
	/** Tick läuft permanent — regelt Regen-Delay-Timer und Sprint-Drain. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// =========================================================================
	//  Stats
	// =========================================================================

	/** Maximale Stamina. Start: 100, wächst durch Progression-Upgrades. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Stats")
	float MaxStamina = 100.f;

	/** Laufende Stamina. Invariante: 0 <= CurrentStamina <= MaxStamina. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stamina|Stats")
	float CurrentStamina = 100.f;

	/** Basis-Regen in Einheiten pro Sekunde. Wird mit RegenMultiplier skaliert. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Regen")
	float BaseRegenRate = 25.f;

	/** Sekunden Sperre nach jedem Verbrauch, bevor Regeneration wieder startet. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Regen")
	float RegenDelay = 1.0f;

	/**
	 * Multiplikator auf BaseRegenRate. 1.0 = Standard. Sea Legs Brew (§11.2)
	 * hebt den Wert für 120 Sekunden auf 1.5 an. Andere Effekte können
	 * kurzfristig debuffen (z. B. verwundet, durchnässt).
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stamina|Regen")
	float RegenMultiplier = 1.f;

	/** Verbrauch pro Sekunde beim Sprinten — kontinuierlicher Drain. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Sprint")
	float SprintDrainPerSecond = 15.f;

	// =========================================================================
	//  API
	// =========================================================================

	/** Prüft ob ein Move bezahlbar ist, ohne zu verbrauchen. Für Input-Gate im Character. */
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool CanAfford(float Cost) const;

	/**
	 * Verbraucht Cost. Gibt false zurück, wenn zu wenig Stamina vorhanden ist —
	 * der CombatComponent bricht die Aktion dann ab bevor die Animation startet.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float Cost);

	/** Perfect-Parry-Reward: voller Bypass des Regen-Delays + feste Restore-Menge. */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RestoreFromPerfectParry(float Amount);

	/** Schaltet den kontinuierlichen Sprint-Drain an/aus. Vom AJakeCharacter gerufen. */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetSprinting(bool bSprintActive);

	/** Für Tinkturen-Buffs. 1.5 = +50 % Regeneration (Sea Legs Brew). */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetRegenMultiplier(float Multiplier);

	// =========================================================================
	//  Delegates
	// =========================================================================

	/** Pro Change-Event — UI-Bar, Audio, VFX hängen sich hier ein. */
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChangedSignature OnStaminaChanged;

	/** Feuert genau einmal pro Depletion, nicht per Tick. */
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaDepletedSignature OnStaminaDepleted;

private:
	/** Sekunden, bis die Regeneration wieder einsetzen darf (zählt runter auf 0). */
	float RegenLockTimer = 0.f;

	/** Cache für den Sprint-Status, damit Tick den kontinuierlichen Drain fahren kann. */
	bool bSprinting = false;
};
