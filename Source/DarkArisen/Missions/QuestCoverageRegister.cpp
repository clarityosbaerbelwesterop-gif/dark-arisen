// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/QuestCoverageRegister.h"

namespace
{
FQuestCoverageSlot AuthoredSlot(
    const EQuestCoverageKind Kind,
    const TCHAR* CoverageId,
    const TCHAR* BucketId,
    const TCHAR* QuestId,
    const TCHAR* Source)
{
    FQuestCoverageSlot Slot;
    Slot.CoverageId = CoverageId;
    Slot.Kind = Kind;
    Slot.State = EQuestCoverageState::Authored;
    Slot.BucketId = BucketId;
    Slot.AuthoredQuestId = QuestId;
    Slot.GoverningSource = Source;
    return Slot;
}

FQuestCoverageSlot GapSlot(
    const EQuestCoverageKind Kind,
    const FString& Prefix,
    const FName BucketId,
    const int32 Index,
    const TCHAR* Source)
{
    FQuestCoverageSlot Slot;
    Slot.CoverageId = FName(*FString::Printf(TEXT("%s.%03d"), *Prefix, Index));
    Slot.Kind = Kind;
    Slot.State = EQuestCoverageState::ReservedUnauthored;
    Slot.BucketId = BucketId;
    Slot.GoverningSource = Source;
    return Slot;
}

FString StandingBucketToken(const EM7StandingMissionType Type)
{
    switch (Type)
    {
    case EM7StandingMissionType::Escort: return TEXT("escort");
    case EM7StandingMissionType::ConvoyRaid: return TEXT("convoy-raid");
    case EM7StandingMissionType::Recovery: return TEXT("recovery");
    case EM7StandingMissionType::Champion: return TEXT("champion");
    case EM7StandingMissionType::Transport: return TEXT("transport");
    case EM7StandingMissionType::PrivateerCommission: return TEXT("privateer-commission");
    case EM7StandingMissionType::RoadWork: return TEXT("road-work");
    case EM7StandingMissionType::Hunt: return TEXT("hunt");
    case EM7StandingMissionType::Salvage: return TEXT("salvage");
    default: return TEXT("invalid");
    }
}
}

TArray<FQuestCoverageSlot> FQuestCoverageRegister::BuildCoverage()
{
    TArray<FQuestCoverageSlot> Result;
    Result.Reserve(RequiredTurns + RequiredStanding);

    Result.Add(AuthoredSlot(
        EQuestCoverageKind::Turn,
        TEXT("turn-authored.rexa-moran.001"),
        TEXT("region.rexa-moran"),
        TEXT("Rexa.Turn.EmptyHammock"),
        TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp")));
    Result.Add(AuthoredSlot(
        EQuestCoverageKind::Turn,
        TEXT("turn-authored.rexa-moran.002"),
        TEXT("region.rexa-moran"),
        TEXT("Rexa.Turn.ThreeCutsInStone"),
        TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp")));
    Result.Add(AuthoredSlot(
        EQuestCoverageKind::Turn,
        TEXT("turn-authored.rexa-moran.003"),
        TEXT("region.rexa-moran"),
        TEXT("Rexa.Turn.SaltLedger"),
        TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp")));

    for (const FTurnProductionRequirement& Requirement : FMissionScaleRequirements::BuildTurnRequirements())
    {
        const int32 MissingInBucket = Requirement.RequiredCount - Requirement.AuthoredIdentityCount;
        const FString Prefix = FString::Printf(TEXT("turn-gap.%s"), *Requirement.RegionId.ToString());
        for (int32 Index = 1; Index <= MissingInBucket; ++Index)
        {
            Result.Add(GapSlot(
                EQuestCoverageKind::Turn,
                Prefix,
                Requirement.RegionId,
                Index,
                TEXT("side quest catalog.md Section 4")));
        }
    }

    Result.Add(AuthoredSlot(
        EQuestCoverageKind::Standing,
        TEXT("standing-authored.salvage.001"),
        TEXT("standing.salvage"),
        TEXT("Rexa.Standing.Salvage.SanTelmoBell"),
        TEXT("Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp")));

    for (const FStandingTypeProductionRequirement& Requirement : FMissionScaleRequirements::BuildStandingTypeRequirements())
    {
        const int32 MissingInBucket = Requirement.RequiredCount - Requirement.AuthoredIdentityCount;
        const FString Token = StandingBucketToken(Requirement.Type);
        const FName BucketId(*FString::Printf(TEXT("standing.%s"), *Token));
        const FString Prefix = FString::Printf(TEXT("standing-gap.%s"), *Token);
        for (int32 Index = 1; Index <= MissingInBucket; ++Index)
        {
            Result.Add(GapSlot(
                EQuestCoverageKind::Standing,
                Prefix,
                BucketId,
                Index,
                TEXT("mission types.md Sections 1-10")));
        }
    }

    return Result;
}

TArray<FQuestCoverageSlot> FQuestCoverageRegister::BuildMissingCoverage()
{
    TArray<FQuestCoverageSlot> Missing;
    for (const FQuestCoverageSlot& Slot : BuildCoverage())
    {
        if (Slot.State == EQuestCoverageState::ReservedUnauthored)
        {
            Missing.Add(Slot);
        }
    }
    return Missing;
}

bool FQuestCoverageRegister::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FQuestCoverageSlot> Coverage = BuildCoverage();
    if (Coverage.Num() != RequiredTurns + RequiredStanding)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Quest coverage must contain exactly %d finite slots; found %d."),
            RequiredTurns + RequiredStanding,
            Coverage.Num()));
    }

    TSet<FName> CoverageIds;
    TSet<FName> AuthoredQuestIds;
    int32 TurnCount = 0;
    int32 StandingCount = 0;
    int32 AuthoredTurnCount = 0;
    int32 AuthoredStandingCount = 0;
    int32 MissingCount = 0;

    for (const FQuestCoverageSlot& Slot : Coverage)
    {
        if (Slot.CoverageId.IsNone() || Slot.BucketId.IsNone() || Slot.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every quest coverage slot requires coverage id, bucket id and governing source."));
            continue;
        }
        if (CoverageIds.Contains(Slot.CoverageId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate quest coverage id: %s"), *Slot.CoverageId.ToString()));
        }
        CoverageIds.Add(Slot.CoverageId);

        if (Slot.Kind == EQuestCoverageKind::Turn)
        {
            ++TurnCount;
        }
        else
        {
            ++StandingCount;
        }

        if (Slot.State == EQuestCoverageState::Authored)
        {
            if (Slot.AuthoredQuestId.IsNone())
            {
                OutErrors.Add(FString::Printf(TEXT("Authored coverage slot %s has no authored quest id."), *Slot.CoverageId.ToString()));
            }
            else if (AuthoredQuestIds.Contains(Slot.AuthoredQuestId))
            {
                OutErrors.Add(FString::Printf(TEXT("Duplicate authored quest identity: %s"), *Slot.AuthoredQuestId.ToString()));
            }
            else
            {
                AuthoredQuestIds.Add(Slot.AuthoredQuestId);
            }

            if (Slot.Kind == EQuestCoverageKind::Turn)
            {
                ++AuthoredTurnCount;
            }
            else
            {
                ++AuthoredStandingCount;
            }
        }
        else
        {
            ++MissingCount;
            if (!Slot.AuthoredQuestId.IsNone())
            {
                OutErrors.Add(FString::Printf(
                    TEXT("Reserved quest slot %s must not carry an invented authored identity."),
                    *Slot.CoverageId.ToString()));
            }
        }
    }

    if (TurnCount != RequiredTurns || AuthoredTurnCount != AuthoredTurns)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Turn coverage must remain %d total / %d authored; found %d / %d."),
            RequiredTurns,
            AuthoredTurns,
            TurnCount,
            AuthoredTurnCount));
    }
    if (StandingCount != RequiredStanding || AuthoredStandingCount != AuthoredStanding)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Standing coverage must remain %d total / %d authored; found %d / %d."),
            RequiredStanding,
            AuthoredStanding,
            StandingCount,
            AuthoredStandingCount));
    }
    if (MissingCount != TotalMissingQuestIdentities)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Quest coverage must expose exactly %d unauthored identities; found %d."),
            TotalMissingQuestIdentities,
            MissingCount));
    }

    return OutErrors.IsEmpty();
}
