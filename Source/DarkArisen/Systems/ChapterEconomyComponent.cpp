// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Systems/ChapterEconomyComponent.h"

UChapterEconomyComponent::UChapterEconomyComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UChapterEconomyComponent::SetPendingLedger(const FChapterLedger& Ledger)
{
    if (!IsValidLedger(Ledger))
    {
        return false;
    }
    PendingLedger = Ledger;
    return true;
}

bool UChapterEconomyComponent::ResolveChapterLedger(
    const int32 Chapter,
    UProgressionEconomyComponent* Economy)
{
    if (!Economy || Chapter <= LastResolvedChapter || !IsValidLedger(PendingLedger))
    {
        return false;
    }

    const int64 Net = PendingLedger.NetDoubloons();
    const bool bResolved = Net >= 0
        ? (Net == 0 || Economy->CreditCurrency(EDarkArisenCurrency::Doubloons, Net))
        : Economy->SpendCurrency(EDarkArisenCurrency::Doubloons, -Net);

    if (!bResolved)
    {
        return false;
    }

    LastResolvedChapter = Chapter;
    PendingLedger = FChapterLedger();
    return true;
}

bool UChapterEconomyComponent::PayLegendaryTierCost(
    UProgressionEconomyComponent* Economy,
    const EDarkArisenCurrency Currency,
    const int64 Amount,
    const FName WorkId)
{
    if (!Economy || WorkId.IsNone() || Amount <= 0 || PaidLegendaryWork.Contains(WorkId))
    {
        return false;
    }

    if (!Economy->SpendCurrency(Currency, Amount))
    {
        return false;
    }

    PaidLegendaryWork.Add(WorkId);
    return true;
}

bool UChapterEconomyComponent::PayReconstructionSilver(
    UProgressionEconomyComponent* Economy,
    const int64 SilverMarks,
    const FName ProjectId)
{
    if (!Economy
        || ProjectId.IsNone()
        || SilverMarks <= 0
        || PaidReconstructionProjects.Contains(ProjectId))
    {
        return false;
    }

    if (!Economy->SpendCurrency(EDarkArisenCurrency::SilverMarks, SilverMarks))
    {
        return false;
    }

    PaidReconstructionProjects.Add(ProjectId);
    return true;
}

bool UChapterEconomyComponent::PayCrewShare(
    UProgressionEconomyComponent* Economy,
    const int64 Doubloons,
    const int32 Chapter)
{
    if (!Economy || Doubloons <= 0 || Chapter <= 0 || PaidCrewShareChapters.Contains(Chapter))
    {
        return false;
    }

    if (!Economy->SpendCurrency(EDarkArisenCurrency::Doubloons, Doubloons))
    {
        return false;
    }

    PaidCrewShareChapters.Add(Chapter);
    return true;
}

bool UChapterEconomyComponent::IsValidLedger(const FChapterLedger& Ledger)
{
    // Hard engineering bound prevents signed overflow while remaining far above any authored band.
    constexpr int64 EngineeringMagnitudeLimit = 1000000000000LL;
    if (Ledger.HoldingIncome < -EngineeringMagnitudeLimit
        || Ledger.HoldingIncome > EngineeringMagnitudeLimit
        || Ledger.ArmyUpkeep < -EngineeringMagnitudeLimit
        || Ledger.ArmyUpkeep > 0
        || Ledger.GarrisonUpkeep < -EngineeringMagnitudeLimit
        || Ledger.GarrisonUpkeep > 0
        || Ledger.ConstructionDraw < -EngineeringMagnitudeLimit
        || Ledger.ConstructionDraw > 0)
    {
        return false;
    }
    return true;
}
