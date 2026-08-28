// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColonialWar/ColonialWarStateSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "RetaliationSubsystem.generated.h"

UENUM(BlueprintType)
enum class ERetaliationStage : uint8
{
    Unnoticed = 0,
    Letter = 1,
    Bounty = 2,
    CounterAttack = 3,
    Hostages = 4,
    ScorchedEarth = 5
};

USTRUCT(BlueprintType)
struct FHostageCandidate
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PersonId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RegionId;

    /** Hidden investment/attachment score. Never surfaced as relationship UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 AttachmentScore = 0;

    /** Ripple is authored connectivity/quest impact, not a player-visible number. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 RippleWeight = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bChild = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bNarrativeProtected = false;

    /** Newer Phase-12 law: adult crew may be selected; this field is informational only. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCrew = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bAlive = true;
};

/**
 * M5 retaliation authority. Heat is hidden, separate for Imperial and Albion, stage-capped by
 * chapter momentum. Children are absolutely excluded. Adult crew remain eligible under the newer
 * Phase-12 crew law unless an individual is explicitly narrative-protected by authored data.
 */
UCLASS()
class DARKARISEN_API URetaliationSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Retaliation")
    bool RecordFactionDamage(EColonialFaction Faction, int32 HeatAmount);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Retaliation")
    bool RecordDeEscalation(EColonialFaction Faction, int32 HeatReduction);

    /** Apply slow decay only if the faction was not attacked in the chapter that just ended. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Retaliation")
    void AdvanceChapter(int32 NewChapter);

    UFUNCTION(BlueprintPure, Category="ColonialWar|Retaliation")
    ERetaliationStage GetCurrentStage(EColonialFaction Faction) const;

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Hostages")
    bool RegisterHostageCandidate(const FHostageCandidate& Candidate);

    /** Deterministic weighted selection for authored mission setup; returns no children. */
    UFUNCTION(BlueprintPure, Category="ColonialWar|Hostages")
    TArray<FName> SelectHostages(EColonialFaction Faction, FName ReachableRegionId, int32 MaxCount) const;

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Hostages")
    bool ResolveHostageLoss(FName PersonId);

    bool IsCandidateEligibleForTests(FName PersonId) const;
    int32 GetHiddenHeatForTests(EColonialFaction Faction) const;

private:
    static bool IsRetaliatingEmpire(EColonialFaction Faction);
    static ERetaliationStage StageFromHeat(int32 Heat);
    static ERetaliationStage CapStageForChapter(ERetaliationStage Stage, int32 Chapter);
    int32& ResolveHeatMutable(EColonialFaction Faction);
    const int32& ResolveHeat(EColonialFaction Faction) const;
    bool WasAttackedThisChapter(EColonialFaction Faction) const;

    UPROPERTY(SaveGame)
    int32 ImperialHeat = 0;

    UPROPERTY(SaveGame)
    int32 AlbionHeat = 0;

    UPROPERTY(SaveGame)
    int32 CurrentChapter = 4;

    UPROPERTY(SaveGame)
    bool bImperialAttackedThisChapter = false;

    UPROPERTY(SaveGame)
    bool bAlbionAttackedThisChapter = false;

    UPROPERTY(SaveGame)
    TMap<FName, FHostageCandidate> HostageCandidates;

    static constexpr int32 PassiveChapterDecay = 5;
};
