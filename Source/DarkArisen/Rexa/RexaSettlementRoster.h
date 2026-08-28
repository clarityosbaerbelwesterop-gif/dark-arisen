// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RexaSettlementRoster.generated.h"

UENUM(BlueprintType)
enum class ERexaResidentHeritage : uint8
{
    IndigenousRexan,
    MixedRexan,
    ImperialColonist,
    SailorTrader
};

UENUM(BlueprintType)
enum class ERexaResidentAgeBand : uint8
{
    Child,
    Adolescent,
    Adult,
    Elder
};

USTRUCT(BlueprintType)
struct FRexaResidentDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Identity")
    FName StableResidentId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Identity")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Identity")
    ERexaResidentHeritage Heritage = ERexaResidentHeritage::IndigenousRexan;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Identity")
    ERexaResidentAgeBand AgeBand = ERexaResidentAgeBand::Adult;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Identity")
    FName OccupationId;

    /** Authored placement relative to the settlement director; never a random crowd point. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Placement")
    FVector HomeOffsetCentimetres = FVector::ZeroVector;

    /** Purpose-driven schedule anchors. Tropical midday always routes to shade or home. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Schedule")
    FName DawnAnchorId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Schedule")
    FName MiddayAnchorId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Schedule")
    FName EveningAnchorId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Schedule")
    FName NightAnchorId;

    /** Connects specific locals to the fallible direction variants they actually know. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Dialogue")
    TArray<FName> KnowledgeIds;

    /** Must be true for every child and false for every other age band. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Safety")
    bool bProtectedChild = false;

    /** Authored building/adult destination used only by protected-child combat flight. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Safety")
    FName ChildSafetyAnchorId;

    FName GetPurposeAnchorAtGameMinute(int64 GameMinute) const;
};

/** Exact forty-person authored Las Raíces roster for the M2 proof settlement. */
UCLASS()
class DARKARISEN_API URexaSettlementRoster : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static constexpr int32 RequiredResidentCount = 40;
    static constexpr int32 RequiredIndigenousCount = 16;
    static constexpr int32 RequiredMixedCount = 12;
    static constexpr int32 RequiredImperialCount = 8;
    static constexpr int32 RequiredSailorTraderCount = 4;

    UFUNCTION(BlueprintPure, Category = "Rexa|Settlement")
    static TArray<FRexaResidentDefinition> GetAuthoredResidents();

    static bool IsRosterValid(const TArray<FRexaResidentDefinition>& Residents);
};
