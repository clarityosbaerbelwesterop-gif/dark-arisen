#!/usr/bin/env python3
"""Deterministic source contract for the first M1 core-loop tranche."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


REQUIRED_FILES = (
    "Source/DarkArisen/CoreLoopTuning.h",
    "Source/DarkArisen/Components/WoundStateComponent.h",
    "Source/DarkArisen/Components/WoundStateComponent.cpp",
    "Source/DarkArisen/Components/CameraStateComponent.h",
    "Source/DarkArisen/Components/CameraStateComponent.cpp",
    "Source/DarkArisen/Components/InteractionComponent.h",
    "Source/DarkArisen/Components/InteractionComponent.cpp",
    "Source/DarkArisen/Interaction/DarkArisenInteractable.h",
    "Source/DarkArisen/Interaction/DarkArisenInteractable.cpp",
    "Source/DarkArisen/PostureOnlyHUD.h",
    "Source/DarkArisen/PostureOnlyHUD.cpp",
    "Source/DarkArisen/DuelingEnemyCharacter.h",
    "Source/DarkArisen/DuelingEnemyCharacter.cpp",
    "Source/DarkArisen/Components/LockOnComponent.h",
    "Source/DarkArisen/Components/LockOnComponent.cpp",
    "Source/DarkArisen/Interaction/PhysicalDoorActor.h",
    "Source/DarkArisen/Interaction/PhysicalDoorActor.cpp",
    "Source/DarkArisen/Interaction/PhysicalPickupActor.h",
    "Source/DarkArisen/Interaction/PhysicalPickupActor.cpp",
    "Source/DarkArisen/Interaction/ExamineDocumentActor.h",
    "Source/DarkArisen/Interaction/ExamineDocumentActor.cpp",
    "Source/DarkArisen/Interaction/InteractionPersistence.h",
    "Source/DarkArisen/Interaction/InteractionPersistence.cpp",
    "Source/DarkArisen/Animation/AnimNotify_ResolveCombatHit.h",
    "Source/DarkArisen/Animation/AnimNotify_ResolveCombatHit.cpp",
    "Source/DarkArisen/Animation/DarkArisenAnimInstance.h",
    "Source/DarkArisen/Animation/DarkArisenAnimInstance.cpp",
    "Docs/M1_CORE_LOOP.md",
)


def _require_fragments(path: Path, fragments: tuple[str, ...], errors: list[str]) -> None:
    if not path.is_file():
        return
    text = path.read_text(encoding="utf-8")
    for fragment in fragments:
        if fragment not in text:
            errors.append(f"{path.name} requirement missing: {fragment}")


def validate(root: Path) -> list[str]:
    errors: list[str] = []
    for relative in REQUIRED_FILES:
        if not (root / relative).is_file():
            errors.append(f"missing M1 file: {relative}")

    _require_fragments(root / "Source/DarkArisen/CoreLoopTuning.h", (
        "InteractionPromptSeconds = 4.0f",
        "DefaultTakeSeconds = 0.6f",
        "WindedStaminaThreshold = 0.30f",
        "HurtHealthThreshold = 0.60f",
        "BadHealthThreshold = 0.30f",
        "FailingHealthThreshold = 0.12f",
        "PostureBreakVulnerabilitySeconds = 1.4f",
        "MediumStartupFrames = 13",
        "MediumRecoveryFrames = 20",
        "GreatStartupFrames = 26",
        "GreatRecoveryFrames = 44",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/WoundStateComponent.cpp", (
        "Health < FailingHealthThreshold",
        "Health < BadHealthThreshold",
        "Health < HurtHealthThreshold",
        "Stamina < WindedStaminaThreshold",
        "!bKatanaEquipped",
        "Profile.bLimp = true",
        "Profile.bStaggerRun = true",
        "Profile.bWeaponDrag = true",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/CameraStateComponent.h", (
        "Free,",
        "Anchored,",
        "Authored",
        "CurrentMode == EPlayerCameraMode::Free",
        "CurrentMode != EPlayerCameraMode::Authored",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/InteractionComponent.cpp", (
        "InteractionPromptRangeMetres",
        "LineTraceSingleByChannel",
        "InteractionPromptSeconds",
        "Execute_BeginInteraction",
        "Execute_CompleteInteraction",
        "Execute_CancelInteraction",
        "EnterAnchoredUntilReleased",
        "OnExaminePresentationChanged.Broadcast",
        "bExaminePresentationActive && !ActiveExamineTarget.IsValid()",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/CombatComponent.cpp", (
        "CurrentState != ECombatState::Idle",
        "ActionCommitmentRemaining > 0.0f",
        "BackstepStaminaCost = 10.0f",
        "DodgeStaminaCost = 25.0f",
        "DesignLaws::DeflectionWindowSeconds",
        "PostureBreakVulnerabilitySeconds",
        "PostureVisualState == EPostureVisualState::Broken",
        "SweepMultiByChannel",
        "ResolveHitAgainst",
        "RecoverRally(RecoveryAction)",
        "TargetCombat->IsDeflectionWindowOpen()",
        "ResolveQueuedMeleeHitFromAnimation",
        "RouteQueuedMeleeHitToAnimationNotify",
        "bPendingHitUsesFrameFallback",
        "ClearQueuedMeleeHit()",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/HealthComponent.cpp", (
        "RallyWindowRemaining = RallyWindowSeconds",
        "ERallyDamageClass::StandardEnemy: return 0.70f",
        "ERallyDamageClass::Poison: return 0.0f",
        "ERallyRecoveryAction::LightHit: return 0.15f",
        "ERallyRecoveryAction::Critical: return 1.0f",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/PostureOnlyHUD.cpp", (
        "PermittedCombatHudElements == 1",
        "GetPostureRemainingFraction",
        "IsPromptVisible",
        "IsExamineVisible",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/JakeCharacter.cpp", (
        "HealthComponent->bRallyEnabled = true",
        "MaximumWoundCameraDriftCentimetres * Instability",
        "CameraBoom->SocketOffset = BaseCameraSocketOffset + LowFrequencyDrift",
        "WoundStateComponent->GetPresentationProfile()",
        "CameraStateComponent->EnterDeathHold()",
        "Gamepad_RightThumbstick",
        "Gamepad_LeftThumbstick",
        "CombatComponent->StartRache()",
        "TryPlayActionMontage(LightAttackMontage)",
        "TryPlayActionMontage(HeavyAttackMontage)",
        "RouteQueuedMeleeHitToAnimationNotify()",
        "Direction.IsNearlyZero() ? BackstepMontage : DodgeMontage",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/DuelingEnemyCharacter.cpp", (
        "AwarenessRangeCentimetres",
        "AttackRangeCentimetres",
        "CombatComponent->PerformLightAttack()",
        "OnDied.AddDynamic",
        "DisableMovement",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/GreyboxGameMode.cpp", (
        "SpawnActor<ADuelingEnemyCharacter>",
        "SpawnActor<APhysicalDoorActor>",
        "SpawnActor<APhysicalPickupActor>",
        "SpawnActor<AExamineDocumentActor>",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Interaction/PhysicalDoorActor.cpp", (
        "FInterpConstantTo",
        "OpenStateBit",
        "BrokenStateBit",
        "SetSimulatePhysics(true)",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Interaction/PhysicalPickupActor.cpp", (
        "DefaultTakeSeconds",
        "VInterpTo",
        "bTaken",
        "ApplyTakenState",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Interaction/ExamineDocumentActor.cpp", (
        "AttachToActor",
        "KeepWorldTransform",
        "GetExamineBody_Implementation",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Interaction/InteractionPersistence.cpp", (
        "CapturePersistentState",
        "RestorePersistentState",
        "StateByPersistentId",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Animation/AnimNotify_ResolveCombatHit.cpp", (
        "ResolveQueuedMeleeHitFromAnimation(HitKind)",
        "Resolve Heavy Hit",
        "Resolve Parry Strike",
        "Resolve Critical Hit",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Animation/DarkArisenAnimInstance.cpp", (
        "GroundSpeedCentimetresPerSecond",
        "Movement->IsFalling()",
        "Movement->GetCurrentAcceleration()",
        "Combat->CurrentState",
        "Combat->CurrentWeightClass",
        "Combat->PostureVisualState",
        "Wounds->GetWoundLayer()",
        "Wounds->GetPresentationProfile()",
        "Wounds->ShouldUseWoundedAnimationSet(bKatanaEquipped)",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/LockOnComponent.cpp", (
        "TActorIterator<AActor>",
        "LineTraceSingleByChannel",
        "Health->IsDead()",
        "!Combat->IsCombatTargetable()",
        "Movement->bOrientRotationToMovement = !bLocked",
        "MaximumAcquisitionRangeCentimetres",
        "MaximumRetentionRangeCentimetres",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/LockOnComponent.h", (
        "creates no marker, outline, widget or HUD state",
    ), errors)
    _require_fragments(root / "Config/DefaultInput.ini", (
        'ActionName="LightAttack"',
        'ActionName="HeavyAttack"',
        'ActionName="Parry"',
        'ActionName="Dodge"',
        'ActionName="Interact"',
        'ActionName="LockOn"',
        'ActionName="Rache"',
    ), errors)

    source_root = root / "Source/DarkArisen"
    source_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in sorted(source_root.rglob("*"))
        if path.is_file() and path.suffix.lower() in {".h", ".cpp"}
    ) if source_root.is_dir() else ""
    for forbidden in (
        "CancelIntoDodge",
        "CancelIntoBlock",
        "HighlightNearbyLoot",
        "TakeAllLoot",
    ):
        if forbidden in source_text:
            errors.append(f"superseded M1 mechanic is forbidden: {forbidden}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    errors = validate(args.root.resolve())
    if errors:
        print("M1 validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    print("M1 core-loop validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
