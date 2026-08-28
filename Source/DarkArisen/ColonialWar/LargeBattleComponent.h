// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColonialWar/ArmyCampaignComponent.h"
#include "Components/ActorComponent.h"
#include "LargeBattleComponent.generated.h"

UENUM(BlueprintType)
enum class ELargeBattleType : uint8
{
    FieldAction,
    Assault,
    Defence,
    Rising
};

UENUM(BlueprintType)
enum class ELargeBattleOutcome : uint8
{
    Unresolved,
    Victory,
    Defeat,
    Withdrawal
};

USTRUCT(BlueprintType)
struct FBattleSegmentDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName SegmentId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1.0"))
    float FrontMetres = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bOfficerPresent = false;
};

USTRUCT()
struct FBattleSegmentRuntimeState
{
    GENERATED_BODY()

    UPROPERTY(SaveGame)
    FName SegmentId;

    /** Hidden by design; no Blueprint getter exposes the number. */
    UPROPERTY(SaveGame)
    int32 Cohesion = 100;

    UPROPERTY(SaveGame)
    EArmyOrder CurrentOrder = EArmyOrder::Hold;

    UPROPERTY(SaveGame)
    float FrontMetres = 40.0f;

    UPROPERTY(SaveGame)
    bool bJakePresent = false;

    UPROPERTY(SaveGame)
    bool bOfficerPresent = false;

    UPROPERTY(SaveGame)
    bool bResolved = false;

    UPROPERTY(SaveGame)
    bool bBroken = false;
};

/**
 * M5 large-battle source boundary. A battle is a line of 3–6 hidden-cohesion segments, not an RTS.
 * Jake occupies at most one segment and cannot prevent other segments from breaking. Rising battles
 * reject Jake's direct orders. Jake falling removes him from direct control but does not end the
 * battle; authored force/world simulation resolves the final outcome after every segment resolves.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API ULargeBattleComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULargeBattleComponent();

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool BeginBattle(FName InBattleId, ELargeBattleType InBattleType);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool RegisterSegment(const FBattleSegmentDefinition& Definition);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool ApplySegmentCohesionDelta(FName SegmentId, int32 Delta);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool SetJakeSegment(FName SegmentId);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool SetOfficerPresent(FName SegmentId, bool bPresent);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool IssueSegmentOrder(FName SegmentId, EArmyOrder Order);

    UFUNCTION(BlueprintCallable, Category="Battle|Line")
    bool RecordSegmentResolved(FName SegmentId, bool bBroken);

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool RecordJakeFallen();

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool ResolveBattle(ELargeBattleOutcome Outcome);

    UFUNCTION(BlueprintPure, Category="Battle")
    bool IsBattleActive() const { return bBattleActive; }

    UFUNCTION(BlueprintPure, Category="Battle")
    bool IsJakeActiveInBattle() const { return bBattleActive && !bJakeFallen; }

    UFUNCTION(BlueprintPure, Category="Battle")
    ELargeBattleOutcome GetOutcome() const { return BattleOutcome; }

    UFUNCTION(BlueprintPure, Category="Battle|WorldRead")
    int32 GetResolvedSegmentCount() const;

    int32 GetHiddenCohesionForTests(FName SegmentId) const;

private:
    bool AreAllSegmentsResolved() const;

    UPROPERTY(SaveGame)
    FName BattleId;

    UPROPERTY(SaveGame)
    ELargeBattleType BattleType = ELargeBattleType::FieldAction;

    UPROPERTY(SaveGame)
    bool bBattleActive = false;

    UPROPERTY(SaveGame)
    bool bJakeFallen = false;

    UPROPERTY(SaveGame)
    ELargeBattleOutcome BattleOutcome = ELargeBattleOutcome::Unresolved;

    UPROPERTY(SaveGame)
    TMap<FName, FBattleSegmentRuntimeState> Segments;

    static constexpr int32 MinimumSegments = 3;
    static constexpr int32 MaximumSegments = 6;
};
