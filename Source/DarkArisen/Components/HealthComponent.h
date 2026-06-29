// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/** Multicast, damit mehrere Systeme (UI, Audio, AI-Aggro-Reset, GameMode) auf den Tod reagieren können. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiedSignature, AActor*, KilledBy);

/** Für HUD-HP-Bar, Damage-Vignette, DualSense-Haptik. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature,
	float, NewHealth, float, MaxHealth, float, Delta);

/**
 * Klassifiziert einen Heilvorgang.
 *
 * GDD §11.1 trennt Instant-Heals (Cane Sugar, Bandage, Rum, Medical Tincture)
 * von einem einzigen Heal-Over-Time-Effekt: Cooked Meal regeneriert 50 % HP
 * über 30 Sekunden. Damit reicht dieses 2-Werte-Enum.
 */
UENUM(BlueprintType)
enum class EHealType : uint8
{
	Instant  UMETA(DisplayName = "Instant"),
	OverTime UMETA(DisplayName = "Over Time")
};

/**
 * UHealthComponent — HP-Pool und Schadens-Pipeline für Jake Harlow.
 *
 * Umsetzt GDD §6.1 Health-Regel: "No regen. Healing via Sugar, Rum, Bandages,
 * Tinctures. Animation commitment." sowie §11.1 Heilitem-Werte.
 *
 * Designregeln:
 *  - Kein Passiv-Regen. CurrentHealth sinkt, bis der Spieler ein Heilitem
 *    bewusst konsumiert — das erzwingt den Souls-Like-Risk-Management-Loop.
 *  - Cooked Meal startet einen Over-Time-Ticker (PendingRegen / RegenRemaining).
 *  - Poise/Stagger-Modulation passiert im CombatComponent, nicht hier. Dieser
 *    Component kennt nur noch finale Schadenszahlen.
 *  - Beim Tod feuert OnDied genau einmal; weitere Damage-/Heal-Calls sind no-ops.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Konstruktor — aktiviert Tick (nur für Over-Time-Heals) und setzt Defaults. */
	UHealthComponent();

protected:
	/** Klammert CurrentHealth auf MaxHealth beim ersten Spawn. */
	virtual void BeginPlay() override;

public:
	/** Tick — rechnet laufende Over-Time-Heals (Cooked Meal) herunter. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// =========================================================================
	//  Stats
	// =========================================================================

	/** Maximale HP. Start: 100. Wird durch Endgame-Upgrades erhöht. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health|Stats")
	float MaxHealth = 100.f;

	/** Laufende HP. Invariante: 0 <= CurrentHealth <= MaxHealth. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
	float CurrentHealth = 100.f;

	/** true sobald CurrentHealth <= 0 — sperrt weitere Damage-/Heal-Calls. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
	bool bIsDead = false;

	// =========================================================================
	//  Damage & Heal API
	// =========================================================================

	/**
	 * Rohschaden-Eingang. Clamped gegen 0, feuert OnHealthChanged und bei
	 * HP == 0 OnDied. Wird von AJakeCharacter::TakeDamage() weitergereicht,
	 * nachdem CombatComponent die Poise-Modulation angewendet hat.
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount, AActor* DamageCauser);

	/**
	 * Heilt Jake. Instant addiert sofort (Sugar/Bandage/Rum/Tincture),
	 * OverTime startet einen Ticker über Duration Sekunden (Cooked Meal).
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyHeal(float Amount, EHealType HealType, float Duration = 0.f);

	/** HP / MaxHP — für HUD-Binding und DualSense-Heartbeat-Intensität. */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	/** Convenience-Abfrage, damit Input-Handler und AI den Totenstatus sauber prüfen. */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	// =========================================================================
	//  Delegates
	// =========================================================================

	/** Abonniert von AJakeCharacter::OnCharacterDied() und von AI-/UI-Systemen. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDiedSignature OnDied;

	/** UI-HP-Bar, Damage-Vignette, Audio-Heartbeat etc. hängen sich hier ein. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

private:
	/** Noch zu verteilende HP-Menge aus dem aktuell laufenden Over-Time-Heal. */
	float PendingRegen = 0.f;

	/** Sekunden, die der Over-Time-Heal noch läuft. 0 = kein aktiver Regen. */
	float RegenRemaining = 0.f;
};
