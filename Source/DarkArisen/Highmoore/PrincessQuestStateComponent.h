// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PrincessQuestStateComponent.generated.h"

UENUM(BlueprintType)
enum class EPrincessQuestMovement : uint8
{
    NotStarted,
    FalseLetter,
    ReturnedToArion,
    Lake,
    ArrowAftermath,
    ArionFalls,
    TurnedWest,
    Belos,
    ReturnJourney,
    DockReached
};

UENUM(BlueprintType)
enum class EBelosPathResolution : uint8
{
    None,
    TurnedAway,
    KatanaAssaultInterrupted,
    OrdinaryWeaponOverwhelmed
};

/**
 * M6 Princess-quest source law boundary. It owns no romance system. The false letter begins the
 * canonical M4 autosave-suppression window and only the dock ends it. The arrow moment exposes
 * explicit negative capabilities so slow motion, cue changes, camera moves, telegraphs and revive
 * prompts cannot be silently added by callers.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UPrincessQuestStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPrincessQuestStateComponent();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordFalseLetterHandedOver();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordReturnedToArion();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordLakeReached();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordArrowMoment();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordArionFalls(bool bReadRealLetter);

    /** West ends the revenge branch without a prompt/marker-owned redirect. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordTurnedWest();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool BeginBelosAssault();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool ResolveBelosAssault(bool bHasCrystalKatana);

    /**
     * Records the protected playable choice to clear/go to the blocked undercroft stair.
     * The state records only that Jake chose it; the game still never shows what is below.
     */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordWentToUndercroftStair();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool BeginReturnJourney();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Princess")
    bool RecordDockReached();

    /** Engine-level social/combat guard for Arion: children and all noncombatants are invalid targets. */
    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|Safety")
    bool CanDamageArionActor(bool bIsCombatant, bool bIsChild) const;

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool AllowsArrowSlowMotion() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool AllowsArrowMusicCueChange() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool AllowsArrowCameraMove() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool AllowsArrowTelegraph() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool AllowsArrowRevivePrompt() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess|ArrowLaw")
    bool RetainsPlayerControlAtArrow() const { return true; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess")
    EPrincessQuestMovement GetMovement() const { return Movement; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess")
    EBelosPathResolution GetBelosResolution() const { return BelosResolution; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess")
    bool DidReadRealLetter() const { return bReadRealLetter; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Princess")
    bool DidGoToUndercroftStair() const { return bWentToUndercroftStair; }

private:
    UPROPERTY(SaveGame)
    EPrincessQuestMovement Movement = EPrincessQuestMovement::NotStarted;

    UPROPERTY(SaveGame)
    EBelosPathResolution BelosResolution = EBelosPathResolution::None;

    UPROPERTY(SaveGame)
    bool bReadRealLetter = false;

    UPROPERTY(SaveGame)
    bool bWentToUndercroftStair = false;

    UPROPERTY(SaveGame)
    bool bAutosaveWindowStarted = false;
};
