// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LiberationAllianceSubsystem.generated.h"

UENUM(BlueprintType)
enum class ELiberationNetwork : uint8
{
    CulturalWeb,
    RecoveryUnderground,
    MountainPaths,
    PatientCoast,
    DeedJudgers
};

UENUM(BlueprintType)
enum class EAllianceTrustTier : uint8
{
    Watched = 0,
    Tolerated = 1,
    Trusted = 2,
    Kin = 3,
    Blood = 4
};

UENUM(BlueprintType)
enum class EAllianceStrengthAction : uint8
{
    FreeForcedLabourConvoy,
    ReturnSacredItem,
    DeliverWeapons,
    CompleteTrainingQuestChain,
    ExposeAtrocityWithEvidence,
    DefendNetworkCell,
    DestroyEnforcementRecords,
    WinLiberationEvent
};

UENUM(BlueprintType)
enum class EAllianceConnectionAuthorship : uint8
{
    AuthoredQuest,
    FullVisionGap
};

USTRUCT(BlueprintType)
struct FLiberationNetworkState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    ELiberationNetwork Network = ELiberationNetwork::CulturalWeb;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    int32 Strength = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    EAllianceTrustTier Trust = EAllianceTrustTier::Watched;
};

USTRUCT(BlueprintType)
struct FAllianceConnectionDefinition
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ELiberationNetwork A = ELiberationNetwork::CulturalWeb;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ELiberationNetwork B = ELiberationNetwork::RecoveryUnderground;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAllianceConnectionAuthorship Authorship = EAllianceConnectionAuthorship::FullVisionGap;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString GoverningSource;
};

/**
 * Phase-7 Liberation Alliance authority and runtime owner for
 * `thread.archipelago.liberation-connections`.
 *
 * The five networks exist independently. This subsystem records their source-authored strength,
 * trust and connection results; it contains no strategy-menu conquest action and no money path to
 * Liberation Strength. Five of the ten pairwise connection quests are still explicitly Full Vision
 * and therefore cannot be completed through this source owner until approved design exists.
 */
UCLASS()
class DARKARISEN_API ULiberationAllianceSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    static const FName ThreadId;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    FLiberationNetworkState GetNetworkState(ELiberationNetwork Network) const;

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordStrengthAction(ELiberationNetwork Network, EAllianceStrengthAction Action);

    /** Stage-3 counter-attacks author a loss range of 5–10; no hidden default is invented here. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordCounterAttackLoss(ELiberationNetwork Network, int32 AuthoredLoss);

    /** Stage-5 scorched earth has the exact authored -20 network-strength consequence. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordScorchedEarthLoss(ELiberationNetwork Network);

    /** Trust gates differ by network; only an already-authored external act may report a new tier. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordTrustTierReached(ELiberationNetwork Network, EAllianceTrustTier NewTier);

    /** Betrayal resets trust. Strength loss is source-dependent and must be supplied by authored data. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordBetrayal(ELiberationNetwork Network, int32 SourceApprovedStrengthLoss);

    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    TArray<FAllianceConnectionDefinition> GetConnectionDefinitions() const;

    /** Records only one of the five connection quests that the current source actually specifies. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordAuthoredConnectionCompleted(FName ConnectionId);

    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    bool IsConnectionComplete(FName ConnectionId) const;

    /** Size of the largest actually connected network component, 1–5. */
    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    int32 GetConnectedNetworkCount() const;

    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    float GetConnectionMultiplier() const;

    /** Becomes pending the first time one connected component contains all five networks. */
    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    bool IsFirstGatheringPending() const { return bFirstGatheringPending; }

    /** Location/staging remains authored externally because the source offers three possible places. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Alliance")
    bool RecordFirstGatheringCompleted();

    UFUNCTION(BlueprintPure, Category="ColonialWar|Alliance")
    int32 GetUnresolvedFullVisionConnectionCount() const;

private:
    static int32 StartingStrength(ELiberationNetwork Network);
    static int32 StrengthGain(EAllianceStrengthAction Action);
    static bool SameUndirectedPair(const FAllianceConnectionDefinition& Definition,
        ELiberationNetwork A, ELiberationNetwork B);
    void RefreshFirstGatheringState();

    UPROPERTY(SaveGame)
    TMap<ELiberationNetwork, FLiberationNetworkState> Networks;

    UPROPERTY(SaveGame)
    TSet<FName> CompletedConnections;

    UPROPERTY(SaveGame)
    bool bFirstGatheringPending = false;

    UPROPERTY(SaveGame)
    bool bFirstGatheringCompleted = false;
};
