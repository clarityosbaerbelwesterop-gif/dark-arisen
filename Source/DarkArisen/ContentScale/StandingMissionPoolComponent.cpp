// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/StandingMissionPoolComponent.h"

namespace
{
constexpr float PayCurveTolerance = 0.001f;

TMap<EM7StandingMissionType, int32> RequiredTypeCounts()
{
    return {
        {EM7StandingMissionType::Escort, 21},
        {EM7StandingMissionType::ConvoyRaid, 18},
        {EM7StandingMissionType::Recovery, 24},
        {EM7StandingMissionType::Champion, 12},
        {EM7StandingMissionType::Transport, 16},
        {EM7StandingMissionType::PrivateerCommission, 14},
        {EM7StandingMissionType::RoadWork, 15},
        {EM7StandingMissionType::Hunt, 13},
        {EM7StandingMissionType::Salvage, 14}};
}
}

UStandingMissionPoolComponent::UStandingMissionPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FName UStandingMissionPoolComponent::MakeRegionTypeKey(const FName RegionId, const EM7StandingMissionType Type)
{
    return FName(*FString::Printf(TEXT("%s|%d"), *RegionId.ToString(), static_cast<int32>(Type)));
}

const FStandingMissionVariantDefinition* UStandingMissionPoolComponent::FindDefinition(const FName StableId) const
{
    return AuthoredVariants.FindByPredicate([StableId](const FStandingMissionVariantDefinition& Definition)
    {
        return Definition.StableId == StableId;
    });
}

bool UStandingMissionPoolComponent::IsPayCurveValid() const
{
    if (SameRegionTypePayMultipliers.Num() != 6)
    {
        return false;
    }
    if (!FMath::IsNearlyEqual(SameRegionTypePayMultipliers[0], 1.0f, PayCurveTolerance)
        || !FMath::IsNearlyEqual(SameRegionTypePayMultipliers[5], 0.60f, PayCurveTolerance))
    {
        return false;
    }
    float Previous = 1.0f + PayCurveTolerance;
    for (const float Multiplier : SameRegionTypePayMultipliers)
    {
        if (!FMath::IsFinite(Multiplier)
            || Multiplier < 0.60f - PayCurveTolerance
            || Multiplier > 1.0f + PayCurveTolerance
            || Multiplier > Previous + PayCurveTolerance)
        {
            return false;
        }
        Previous = Multiplier;
    }
    return true;
}

bool UStandingMissionPoolComponent::ValidateAuthoredPool(TArray<FString>& OutErrors) const
{
    OutErrors.Reset();

    if (AuthoredVariants.Num() != UContentScaleManifestDataAsset::RequiredStandingVariants)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Standing pool requires exactly %d authored variants; found %d"),
            UContentScaleManifestDataAsset::RequiredStandingVariants,
            AuthoredVariants.Num()));
    }

    if (!IsPayCurveValid())
    {
        OutErrors.Add(TEXT("Standing pay curve must have six monotonic authored values, run 1 = 1.00 and run 6 = 0.60"));
    }

    TSet<FName> SeenIds;
    TMap<EM7StandingMissionType, int32> Counts;
    for (const FStandingMissionVariantDefinition& Definition : AuthoredVariants)
    {
        if (Definition.StableId.IsNone())
        {
            OutErrors.Add(TEXT("Standing variant has no stable id"));
        }
        else if (SeenIds.Contains(Definition.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("duplicate Standing variant id: %s"), *Definition.StableId.ToString()));
        }
        else
        {
            SeenIds.Add(Definition.StableId);
        }

        if (Definition.RegionId.IsNone())
        {
            OutErrors.Add(FString::Printf(TEXT("Standing variant %s has no region"), *Definition.StableId.ToString()));
        }
        if (Definition.Type == EM7StandingMissionType::None)
        {
            OutErrors.Add(FString::Printf(TEXT("Standing variant %s has no authored mission type"), *Definition.StableId.ToString()));
        }
        else
        {
            Counts.FindOrAdd(Definition.Type)++;
        }
        if (Definition.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Standing variant %s has no governing source"), *Definition.StableId.ToString()));
        }
        if (Definition.FullPay < 0 || Definition.StandingGain < 0)
        {
            OutErrors.Add(FString::Printf(TEXT("Standing variant %s has invalid negative reward data"), *Definition.StableId.ToString()));
        }
    }

    for (const TPair<EM7StandingMissionType, int32>& Requirement : RequiredTypeCounts())
    {
        const int32 Actual = Counts.FindRef(Requirement.Key);
        if (Actual != Requirement.Value)
        {
            OutErrors.Add(FString::Printf(
                TEXT("Standing type %d requires exactly %d authored variants; found %d"),
                static_cast<int32>(Requirement.Key),
                Requirement.Value,
                Actual));
        }
    }

    for (const FName CompletedId : CompletedVariantIds)
    {
        if (!SeenIds.Contains(CompletedId))
        {
            OutErrors.Add(FString::Printf(TEXT("save state references unknown Standing variant: %s"), *CompletedId.ToString()));
        }
    }

    return OutErrors.IsEmpty();
}

const FStandingMissionVariantDefinition* UStandingMissionPoolComponent::FindNextAvailableVariant(
    const FName RegionId,
    const EM7StandingMissionType Type) const
{
    if (RegionId.IsNone() || Type == EM7StandingMissionType::None)
    {
        return nullptr;
    }

    return AuthoredVariants.FindByPredicate([this, RegionId, Type](const FStandingMissionVariantDefinition& Definition)
    {
        return Definition.RegionId == RegionId
            && Definition.Type == Type
            && !CompletedVariantIds.Contains(Definition.StableId);
    });
}

bool UStandingMissionPoolComponent::IsPoolExhausted(const FName RegionId, const EM7StandingMissionType Type) const
{
    return FindNextAvailableVariant(RegionId, Type) == nullptr;
}

int32 UStandingMissionPoolComponent::GetCompletedCount(const FName RegionId, const EM7StandingMissionType Type) const
{
    return CompletedCountByRegionType.FindRef(MakeRegionTypeKey(RegionId, Type));
}

float UStandingMissionPoolComponent::GetPayMultiplierForNextRun(
    const FName RegionId,
    const EM7StandingMissionType Type) const
{
    if (!IsPayCurveValid() || RegionId.IsNone() || Type == EM7StandingMissionType::None)
    {
        return 0.0f;
    }

    const int32 CompletedCount = GetCompletedCount(RegionId, Type);
    const int32 CurveIndex = FMath::Clamp(CompletedCount, 0, SameRegionTypePayMultipliers.Num() - 1);
    return SameRegionTypePayMultipliers[CurveIndex];
}

bool UStandingMissionPoolComponent::CompleteVariant(
    const FName StableId,
    FStandingMissionCompletionResult& OutResult)
{
    OutResult = FStandingMissionCompletionResult{};

    const FStandingMissionVariantDefinition* Definition = FindDefinition(StableId);
    if (!Definition
        || CompletedVariantIds.Contains(StableId)
        || Definition->RegionId.IsNone()
        || Definition->Type == EM7StandingMissionType::None
        || !IsPayCurveValid())
    {
        return false;
    }

    const FName PoolKey = MakeRegionTypeKey(Definition->RegionId, Definition->Type);
    const int32 PreviousRuns = CompletedCountByRegionType.FindRef(PoolKey);
    const int32 CurveIndex = FMath::Clamp(PreviousRuns, 0, SameRegionTypePayMultipliers.Num() - 1);
    const float Multiplier = SameRegionTypePayMultipliers[CurveIndex];

    CompletedVariantIds.Add(StableId);
    CompletedCountByRegionType.FindOrAdd(PoolKey) = PreviousRuns + 1;

    OutResult.bCompleted = true;
    OutResult.AppliedPayMultiplier = Multiplier;
    OutResult.PaidAmount = FMath::Max(0, FMath::RoundToInt(static_cast<float>(Definition->FullPay) * Multiplier));
    OutResult.StandingGain = Definition->StandingGain;
    OutResult.SameRegionTypeRunNumber = PreviousRuns + 1;
    return true;
}
