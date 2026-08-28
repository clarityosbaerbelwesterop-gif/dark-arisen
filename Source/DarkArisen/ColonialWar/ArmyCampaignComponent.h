// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArmyCampaignComponent.generated.h"

UENUM(BlueprintType)
enum class ECampaignForceType : uint8
{
    Hired,
    SettlementMilitia,
    Alliance,
    FactionLevy,
    Crew
};

UENUM(BlueprintType)
enum class EArmyOrder : uint8
{
    Hold,
    Press,
    BreakOff
};

UENUM(BlueprintType)
enum class EForceMoraleRead : uint8
{
    Steady,
    Quiet,
    Fraying,
    Departed
};

USTRUCT(BlueprintType)
struct FCampaignForceDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ForceId;

    /** Named/cultural source; there is deliberately no generic Infantry unit type. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName SourceId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECampaignForceType Type = ECampaignForceType::Hired;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1"))
    int32 Men = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCavalry = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bSpecialist = false;

    /** Company marines stop immediately when unpaid; other hired troops desert after two chapters. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCompanyMarines = false;
};

/**
 * M5 army state. No unit cards, formations, facing controls or selection boxes exist here.
 * Recruitment arrives from authored scenes as named/cultural forces. Jake has exactly three
 * command verbs and cannot command Alliance forces. Crew are representable but never counted as
 * an army.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UArmyCampaignComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UArmyCampaignComponent();

    UFUNCTION(BlueprintCallable, Category="Army|Recruitment")
    bool RegisterForce(const FCampaignForceDefinition& Definition);

    UFUNCTION(BlueprintCallable, Category="Army|Officers")
    bool SetNamedOfficerCount(int32 Count);

    UFUNCTION(BlueprintCallable, Category="Army|Pay")
    bool RecordForcePaid(FName ForceId, bool bPaid);

    UFUNCTION(BlueprintCallable, Category="Army|Chapter")
    void AdvanceChapter();

    UFUNCTION(BlueprintCallable, Category="Army|Command")
    bool IssueOrder(FName ForceId, EArmyOrder Order);

    UFUNCTION(BlueprintPure, Category="Army|WorldRead")
    EForceMoraleRead GetForceMoraleRead(FName ForceId) const;

    UFUNCTION(BlueprintPure, Category="Army|WorldRead")
    int32 GetCommandCapacity() const;

    UFUNCTION(BlueprintPure, Category="Army|WorldRead")
    bool IsOverCommandCapacity() const;

    UFUNCTION(BlueprintPure, Category="Army|Economy")
    int64 CalculateChapterDoubloonUpkeep() const;

    int32 GetArmyMenForTests() const;
    bool IsForceActiveForTests(FName ForceId) const;

private:
    UPROPERTY(SaveGame)
    TMap<FName, FCampaignForceDefinition> Forces;

    UPROPERTY(SaveGame)
    TMap<FName, int32> UnpaidChapters;

    UPROPERTY(SaveGame)
    TSet<FName> PaidThisChapter;

    UPROPERTY(SaveGame)
    TSet<FName> DepartedForces;

    UPROPERTY(SaveGame)
    TMap<FName, EArmyOrder> LastOrders;

    UPROPERTY(SaveGame)
    int32 NamedOfficerCount = 0;

    static constexpr int32 JakeBaseCommandCapacity = 200;
    static constexpr int32 CapacityPerNamedOfficer = 150;
    static constexpr int32 HiredUpkeepPerMan = 14;
    static constexpr int32 MilitiaUpkeepPerMan = 4;
};
