// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

// Forward Declarations
class UStaminaComponent;
class UHealthComponent;

/**
 * Waffen-Slots im Weapon Wheel (GDD §8.1 — 8 Slots total).
 *
 * Der Starting-Loadout aus §6.2 belegt Cutlass + Flintlock; die anderen Slots
 * werden im Spielverlauf freigeschaltet. None entspricht einem leeren Slot.
 */
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None      UMETA(DisplayName = "Empty"),
	Cutlass   UMETA(DisplayName = "Cutlass"),
	Flintlock UMETA(DisplayName = "Flintlock"),
	Musket    UMETA(DisplayName = "Musket"),
	Bow       UMETA(DisplayName = "Bow"),
	Throwable UMETA(DisplayName = "Throwable")
};

/**
 * Moveset-State-Machine. Der Character darf nur dann neue Combat-Inputs
 * annehmen, wenn der State Idle ist — sonst werden Combos durch Mashing
 * zerrissen und Animationen brechen mitten im Hit-Frame ab.
 */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Idle,
	LightAttacking,
	HeavyCharging,
	HeavyReleasing,
	Parrying,
	Dodging,
	Staggered,
	Dead
};

/** Events für Animation-Notifies, VFX-Trigger, HUD-Updates, SFX. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, ECombatState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, EWeaponSlot, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostureBroken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRacheMeterChanged, float, NewValue, float, MaxValue);

/**
 * UCombatComponent — Moveset, Posture, Waffen-Handling und Rache-Kontrolle.
 *
 * Vereint die Combat-Mechaniken aus GDD §6.1 (Posture/Parry/Dodge/Poise),
 * §6.2 (Starting Loadout: Broken Cutlass, Rusty Flintlock), §7 (Rache-System)
 * und §5.1 (Input-Bindings, die hier als High-Level-Aktionen ankommen).
 *
 * Verantwortlich für:
 *  - Die ECombatState-Maschine — Input-Gate gegen Mashing
 *  - Posture-Bar (Sekiro-inspiriert: Break = Critical-Hit-Opportunity)
 *  - Waffen-Wheel-State und aktuelle Primary-/Secondary-Waffe
 *  - Rache-Meter: Drain während aktiv, Fill via Cigar/Rum/Kill/Hit
 *  - Koordination mit UStaminaComponent (Kostenprüfung vor jedem Move)
 *    und UHealthComponent (Parry-Window nullt Eingangsschaden)
 *
 * Laut Jakes Charakter-Bibel kämpft er "still, geduldig, beobachtet zuerst"
 * und "lässt Gegner zu sich kommen, pariert, schlägt zurück" — das
 * Balancing-Ziel für diesen Component ist deshalb, Parry und Dodge
 * mindestens so belohnend zu machen wie Offensiv-Combos.
 *
 * Hit-Detection-Traces und Animation-Montages werden in der .cpp aufgesetzt;
 * dieser Header definiert nur das öffentliche Gerüst.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Konstruktor — aktiviert Tick (für Parry-Window und Rache-Drain). */
	UCombatComponent();

protected:
	/** Cached Handles auf die Schwester-Komponenten (Stamina, Health) am Owner. */
	virtual void BeginPlay() override;

public:
	/** Tick — rechnet Parry-Window runter und drainiert den Rache-Meter bei Aktiv-State. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// =========================================================================
	//  Posture (Sekiro-Style, GDD §6.1)
	// =========================================================================

	/** Obergrenze der Posture-Bar, bevor Jake in den Posture-Break stürzt. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Posture")
	float MaxPosture = 100.f;

	/** Laufender Posture-Wert. Baut sich im Idle automatisch ab (Tick). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Posture")
	float CurrentPosture = 0.f;

	/**
	 * Poise-Multiplikator auf eingehenden Posture-Damage.
	 * 1.0 = Standard, 0.7 entspricht +30 % Poise (Ironhide Tincture, §11.2).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Posture")
	float PoiseMultiplier = 1.f;

	/** Addiert Posture-Schaden, prüft Break-Bedingung; vom Hit-Handler gerufen. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Posture")
	void AddPostureDamage(float Amount);

	// =========================================================================
	//  Core Moveset
	// =========================================================================

	/**
	 * R1 — Light Attack. Konsumiert Stamina, startet die Combo-Animation und
	 * öffnet ein Combo-Window für die nächste Light-Attack innerhalb X Sekunden.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
	void PerformLightAttack();

	/**
	 * R2 — Heavy Attack. Wenn bCharged true ist, wurde der Adaptive Trigger
	 * voll gehalten und der finale Schaden wird entsprechend skaliert
	 * (siehe §5.1 DualSense "Adaptive trigger resistance").
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
	void PerformHeavyAttack(bool bCharged);

	/**
	 * Square — Parry. Öffnet ein kurzes Parry-Window. Während des Windows
	 * eingehender Schaden wird auf 0 gesetzt, dem Angreifer massiv Posture
	 * zugefügt. Innerhalb eines kleineren Perfect-Windows zusätzlich
	 * Stamina-Restore via UStaminaComponent::RestoreFromPerfectParry().
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
	void PerformParry();

	/**
	 * Circle — direktionaler Dodge Roll mit i-Frames. Der Direction-Vector
	 * wird aus dem aktuellen Movement-Input in AJakeCharacter abgeleitet.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
	void PerformDodge(const FVector& Direction);

	/** R1 + Square — feuert die aktuell equippte Ranged-Waffe (Flintlock/Musket/Bow). */
	UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
	void FireEquippedRanged();

	// =========================================================================
	//  State Machine
	// =========================================================================

	/** Aktueller Moveset-State — gated weitere Combat-Inputs vom Character. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|State")
	ECombatState CurrentState = ECombatState::Idle;

	/**
	 * Vom Animation-Notify am Ende einer Action gerufen; setzt den State
	 * zurück auf Idle und öffnet damit den Input-Gate für den nächsten Move.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|State")
	void FinishAction();

	// =========================================================================
	//  Weapon Wheel / Loadout (GDD §8.1)
	// =========================================================================

	/** Aktuell gezogene Primary-Melee-Waffe. Start: Cutlass. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
	EWeaponSlot CurrentMelee = EWeaponSlot::Cutlass;

	/** Aktuell equippte Ranged-Waffe. Start: Flintlock. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
	EWeaponSlot CurrentRanged = EWeaponSlot::Flintlock;

	/** Wird vom Weapon-Wheel-Select aufgerufen, wenn der Player einen Slot bestätigt. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Loadout")
	void EquipWeapon(EWeaponSlot Slot);

	// =========================================================================
	//  Rache — Signature-Fähigkeit (GDD §7)
	// =========================================================================

	/** Meter-Maximum. Volle Bar = ~8 Sekunden Slow-Motion laut §7.2. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Rache")
	float MaxRache = 100.f;

	/** Laufender Meter. Start 0, bis Chapter-9-Story-Trigger bRacheUnlocked setzt. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
	float CurrentRache = 0.f;

	/** Drain pro Sekunde im aktiven Zustand. 12.5 × 8 s = volle Bar → ca. 8 s Dauer. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Rache")
	float RacheDrainPerSecond = 12.5f;

	/** true, solange Jake sich im Slow-Motion-Zustand befindet. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
	bool bRacheActive = false;

	/**
	 * Aktiviert Slow-Motion: setzt Global Time Dilation, schaltet den
	 * desaturierten Post-Process-Stack ein, highlightet Gegner in Crimson,
	 * triggert den Heartbeat-Audio-Loop. Prüft vorher bRacheActive == false
	 * und CurrentRache > 0.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
	void StartRache();

	/** Stoppt Slow-Motion sauber, stellt Time Dilation und Post-Process zurück. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
	void StopRache();

	/**
	 * Füllt den Rache-Meter um Percent Prozent von MaxRache.
	 * Cigar (§7.3) ruft das mit 50, Rum (§7.4) mit 25 auf;
	 * Kills und Treffer-Reaktionen rufen es mit kleineren Werten auf.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
	void AddRacheFuel(float Percent);

	// =========================================================================
	//  Delegates
	// =========================================================================

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnCombatStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnWeaponChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnPostureBroken OnPostureBroken;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnRacheMeterChanged OnRacheMeterChanged;

private:
	/** Cached Referenz auf Jakes Stamina-Component, in BeginPlay aufgelöst. */
	UPROPERTY()
	TObjectPtr<UStaminaComponent> CachedStamina;

	/** Cached Referenz auf Jakes Health-Component für Parry-Damage-Null-Setzen. */
	UPROPERTY()
	TObjectPtr<UHealthComponent> CachedHealth;

	/** Sekunden, die das aktuelle Parry-Window noch offen ist. 0 = kein Parry aktiv. */
	float ParryWindowRemaining = 0.f;

	/** Wechselt CurrentState und feuert OnStateChanged. Einziger Schreibpfad auf den State. */
	void SetState(ECombatState NewState);
};
