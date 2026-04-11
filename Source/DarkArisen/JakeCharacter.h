// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JakeCharacter.generated.h"

// Forward Declarations — Component-Klassen werden in eigenen Headern definiert,
// sobald das UE5-Projekt im nächsten Jahr aufgesetzt wird.
class UHealthComponent;
class UStaminaComponent;
class UCombatComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
struct FInputActionValue;

/**
 * AJakeCharacter — Spielercharakter Jake Harlow ("Dark Arisen")
 *
 * 17-jähriger Schiffbrüchiger, der nach dem Piratenüberfall auf das
 * Familienschiff an der Küste von Moran angespült wird. Laut Charakter-Bibel
 * kämpft Jake bewusst anders als sein Bruder Ethan: still, geduldig,
 * beobachtend. Er lässt Gegner zu sich kommen, pariert, schlägt präzise
 * zurück — im Gegensatz zu Ethans aggressivem, raumgreifendem Zweihänder-Stil.
 *
 * Diese Klasse ist das Gerüst für den Player Pawn. Das GDD §6 (Combat System)
 * verlangt drei zentrale Ressourcen, die jeweils in eigene Komponenten
 * ausgelagert sind, um das Moveset-Tuning später vom Character-Code zu trennen:
 *
 *   - HealthComponent   -> HP-Pool, keine Regeneration, Heilung nur via Items
 *   - StaminaComponent  -> Kostet bei jeder Aktion, regeneriert im Idle
 *   - CombatComponent   -> Posture (Sekiro-Style), Moveset, Waffen-Switching
 *
 * Hinweis: Das eigentliche UE5-Projekt wird erst 2027 aufgesetzt. Dieser Header
 * dient als Referenz-Vorlage und wird noch nicht kompiliert.
 */
UCLASS()
class DARKARISEN_API AJakeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Konstruktor — erstellt alle Subobjects (Components, Kamera) und setzt Defaults. */
	AJakeCharacter();

protected:
	/** Wird beim Spawn gerufen; bindet Delegates (z. B. HealthComponent::OnDied) und setzt Start-Stats. */
	virtual void BeginPlay() override;

public:
	/** Per-Frame Update. Wird für Stamina-Regen-Ticks und Rache-Meter-Drain benötigt. */
	virtual void Tick(float DeltaSeconds) override;

	/** Verknüpft die Enhanced-Input-Actions (Move, Look, Attack, Parry, Dodge, …) mit den Handlern unten. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// =========================================================================
	//  Components
	// =========================================================================

	/** HP-Pool, Schadensaufnahme, Tod-Event. Besitzt keinen Auto-Regen — siehe GDD §6.1. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	/** Stamina-Verwaltung: Verbrauch bei Attack/Dodge/Sprint/Parry, Regeneration im Idle. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
	TObjectPtr<UStaminaComponent> StaminaComponent;

	/**
	 * Combat-Kern: Moveset-State-Machine, Posture-Bar, aktuell gewählte Waffe
	 * (Cutlass / Flintlock / Bogen / Musket), Hitbox-Aktivierung, Combo-Counter.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
	TObjectPtr<UCombatComponent> CombatComponent;

protected:
	// =========================================================================
	//  Camera Rig
	// =========================================================================

	/** Federarm für die Third-Person-Kamera; kollidiert gegen die Welt, damit die Kamera bei Wänden einzoomt. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow-Kamera am Ende des CameraBoom. Einziger aktiver View des Players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// =========================================================================
	//  Movement Input
	// =========================================================================

	/** WASD / Linker Stick — bewegt Jake relativ zur Kamera-Blickrichtung. */
	void HandleMoveInput(const FInputActionValue& Value);

	/** Maus / Rechter Stick — dreht Yaw/Pitch der Kamera. Respektiert Invert-Y-Settings. */
	void HandleLookInput(const FInputActionValue& Value);

	/** Sprint Start — erhöht MaxWalkSpeed; Stamina-Verbrauch läuft solange bSprinting == true. */
	void HandleSprintStart();

	/** Sprint Stop — zurück auf Normal-Speed, stoppt den Stamina-Drain. */
	void HandleSprintStop();

	/** Jump / Climb — kontextabhängig: ACharacter::Jump() in Luft, ClimbStart an markierten Surfaces. */
	void HandleJumpInput();

	// =========================================================================
	//  Combat Input (GDD §5.1 — PlayStation-Layout als Referenz)
	// =========================================================================

	/** R1 — Light Attack: schneller Cutlass-Combo-Opener, niedriger Stamina-Cost, kettbar. */
	void HandleLightAttack();

	/** R2 — Heavy Attack: langsam, hoher Schaden, ladbar (DualSense Adaptive Trigger). */
	void HandleHeavyAttack();

	/** Square — Parry: timed Block, füllt Gegner-Posture drastisch, perfekt-parry gibt Stamina zurück. */
	void HandleParry();

	/** Circle — Dodge Roll: direktional, i-Frames, Stamina-Cost. Kein Panic-Button. */
	void HandleDodge();

	/** R1 + Square — feuert die aktuell equippte Firearm (Flintlock / Musket / Bogen). */
	void HandleFireWeapon();

	/** L2 Pressed — Hold-to-Aim für Fernwaffen, Tap-to-Lock-On für Melee-Targets. */
	void HandleAimPressed();

	/** L2 Released — bricht Aim ab bzw. gibt Lock-On frei. */
	void HandleAimReleased();

	/** L1 Pressed — öffnet das Weapon Wheel (RDR2-Style) und triggert leichte Zeitlupe. */
	void HandleWeaponWheelOpen();

	/** L1 Released — schließt das Wheel und wendet die Auswahl auf CombatComponent an. */
	void HandleWeaponWheelClose();

	// =========================================================================
	//  Rache — Signature-Fähigkeit (GDD §7)
	// =========================================================================

	/**
	 * L3 + R3 — aktiviert/deaktiviert Rache (Slow-Motion-Combat).
	 *
	 * Narrative Einschränkung: Diese Fähigkeit ist erst ab Chapter 9 verfügbar,
	 * wenn Jake Ethans Verrat auf dem brennenden Deck entdeckt. Der Handler
	 * prüft deshalb zwingend bRacheUnlocked UND einen positiven Meter-Stand.
	 */
	void HandleRacheToggle();

	/** Wird durch den Chapter-9-Story-Trigger auf true gesetzt und persistiert im SaveGame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jake|Rache", SaveGame)
	bool bRacheUnlocked = false;

	// =========================================================================
	//  World Interaction
	// =========================================================================

	/** Triangle — kontextsensitiver Interact-Button: Loot, Dialog, Türen, Rest-Point-Totems, Lagerfeuer. */
	void HandleInteract();

	/** D-Pad Up — konsumiert das aktuell equippte Healing-Item (z. B. Raw Cane Sugar). */
	void HandleQuickHeal();

	/** D-Pad Down — pfeift das Pferd herbei. Funktioniert nur in camp-fähigen Zonen. */
	void HandleWhistleHorse();

public:
	// =========================================================================
	//  Damage & Death Glue
	// =========================================================================

	/** UE4/5-Standard-Damage-Eingang. Leitet den Schaden an den HealthComponent weiter und triggert Hit-Reactions. */
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Callback, der vom HealthComponent::OnDied-Delegate ausgelöst wird.
	 * Verantwortlich für: Doubloons droppen (GDD §6.3), Death-Animation spielen,
	 * GameMode über den Respawn am letzten aktivierten Rest-Point informieren.
	 */
	UFUNCTION()
	void OnCharacterDied();
};
