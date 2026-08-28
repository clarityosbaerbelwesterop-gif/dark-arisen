// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Ship/ShipHouseholdComponent.h"

#include "Engine/World.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

UShipHouseholdComponent::UShipHouseholdComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.5f;
}

void UShipHouseholdComponent::BeginPlay()
{
    Super::BeginPlay();

    const UWorld* World = GetWorld();
    const UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    ApplyGameMinute(Rules ? Rules->GetTotalWorldMinutes() : 0);
}

void UShipHouseholdComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const UWorld* World = GetWorld();
    const UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    if (Rules && Rules->GetTotalWorldMinutes() != LastAppliedGameMinute)
    {
        ApplyGameMinute(Rules->GetTotalWorldMinutes());
    }
}

bool UShipHouseholdComponent::ApplyGameMinute(const int64 GameMinute)
{
    if (GameMinute < 0)
    {
        return false;
    }

    constexpr int64 MinutesPerDay = 24 * 60;
    const int32 MinuteOfDay = static_cast<int32>(GameMinute % MinutesPerDay);
    RebuildNamedReads(MinuteOfDay);
    LastAppliedGameMinute = GameMinute;
    return NamedReads.Num() == 5;
}

FCrewHouseholdRead UShipHouseholdComponent::GetNamedCrewRead(const FName CrewId) const
{
    if (const FCrewHouseholdRead* Existing = NamedReads.Find(CrewId))
    {
        return *Existing;
    }

    FCrewHouseholdRead Missing;
    Missing.CrewId = NAME_None;
    Missing.Activity = ECrewHouseholdActivity::OffDuty;
    return Missing;
}

ECrewMoraleRead UShipHouseholdComponent::GetMoraleRead() const
{
    if (!RecentLosses.IsEmpty())
    {
        return ECrewMoraleRead::RecentLoss;
    }
    if (InternalMorale >= 75)
    {
        return ECrewMoraleRead::High;
    }
    if (InternalMorale >= 55 && bCrewShareCurrentlyPaid)
    {
        return ECrewMoraleRead::Fed;
    }
    if (InternalMorale >= 35)
    {
        return ECrewMoraleRead::Strained;
    }
    return ECrewMoraleRead::Bad;
}

ECrewMoraleCue UShipHouseholdComponent::GetVisibleMoraleCue() const
{
    if (!RecentLosses.IsEmpty())
    {
        return ECrewMoraleCue::EmptyHammock;
    }

    const int32 MinuteOfDay = LastAppliedGameMinute >= 0
        ? static_cast<int32>(LastAppliedGameMinute % (24 * 60))
        : 0;

    switch (GetMoraleRead())
    {
    case ECrewMoraleRead::High:
        return (MinuteOfDay / 60) % 2 == 0
            ? ECrewMoraleCue::SingingMidDeck
            : ECrewMoraleCue::SharedMending;
    case ECrewMoraleRead::Fed:
        return ECrewMoraleCue::GalleyFire;
    case ECrewMoraleRead::Strained:
        return ECrewMoraleCue::UnbrokenArgument;
    case ECrewMoraleRead::Bad:
        return IsDusk(MinuteOfDay)
            ? ECrewMoraleCue::EmptyForecastleAtDusk
            : ECrewMoraleCue::UnbrokenArgument;
    case ECrewMoraleRead::RecentLoss:
    default:
        return ECrewMoraleCue::EmptyHammock;
    }
}

void UShipHouseholdComponent::RecordCrewSharePaid(const bool bPaidInFull)
{
    bCrewShareCurrentlyPaid = bPaidInFull;
    InternalMorale += bPaidInFull ? 5 : -12;
    ClampMorale();
}

void UShipHouseholdComponent::RecordCrewLoss(const FName StableCrewId)
{
    if (StableCrewId.IsNone() || RecentLosses.Contains(StableCrewId))
    {
        return;
    }

    RecentLosses.Add(StableCrewId);
    InternalMorale -= 18;
    ClampMorale();
}

void UShipHouseholdComponent::RecordSharedRepairMoment()
{
    InternalMorale += 4;
    ClampMorale();
}

void UShipHouseholdComponent::RecordStrainingChoice()
{
    InternalMorale -= 8;
    ClampMorale();
}

bool UShipHouseholdComponent::SetSpecialistBerthsOccupied(const int32 Count)
{
    if (Count < 0 || Count > 8)
    {
        return false;
    }
    SpecialistBerthsOccupied = Count;
    return true;
}

bool UShipHouseholdComponent::SetSemiNamedHands(const int32 Count)
{
    if (Count < 0 || Count > 12)
    {
        return false;
    }

    const UShipVoyageComponent* Voyage = GetOwner()
        ? GetOwner()->FindComponentByClass<UShipVoyageComponent>()
        : nullptr;
    if (Voyage && Count > Voyage->GetActiveHands())
    {
        return false;
    }

    SemiNamedHands = Count;
    return true;
}

void UShipHouseholdComponent::RebuildNamedReads(const int32 MinuteOfDay)
{
    NamedReads.Reset();

    auto AddRead = [this](
        const TCHAR* CrewId,
        const EShipDeck Deck,
        const ECrewHouseholdActivity Activity)
    {
        FCrewHouseholdRead Read;
        Read.CrewId = FName(CrewId);
        Read.Deck = Deck;
        Read.Activity = Activity;
        NamedReads.Add(Read.CrewId, Read);
    };

    if (IsMealWindow(MinuteOfDay))
    {
        AddRead(TEXT("crew.mira"), EShipDeck::Mid, ECrewHouseholdActivity::Meal);
        AddRead(TEXT("crew.big_tom"), EShipDeck::Mid, ECrewHouseholdActivity::Meal);
        AddRead(TEXT("crew.esteban"), EShipDeck::Mid, ECrewHouseholdActivity::Meal);
        AddRead(TEXT("crew.ines"), EShipDeck::Mid, ECrewHouseholdActivity::Meal);
        AddRead(TEXT("crew.father_salvio"), EShipDeck::Mid, ECrewHouseholdActivity::Meal);
        return;
    }

    const bool bDawn = MinuteOfDay >= 300 && MinuteOfDay < 480;
    const bool bWorkingDay = MinuteOfDay >= 480 && MinuteOfDay < 1080;
    const bool bNight = MinuteOfDay >= 1200 || MinuteOfDay < 360;

    AddRead(
        TEXT("crew.mira"),
        EShipDeck::Weather,
        bDawn ? ECrewHouseholdActivity::Navigation : ECrewHouseholdActivity::Watch);
    AddRead(
        TEXT("crew.big_tom"),
        EShipDeck::Mid,
        bWorkingDay ? ECrewHouseholdActivity::Forge : ECrewHouseholdActivity::OffDuty);
    AddRead(
        TEXT("crew.esteban"),
        bNight ? EShipDeck::Weather : EShipDeck::Upper,
        bNight ? ECrewHouseholdActivity::Navigation : ECrewHouseholdActivity::OffDuty);
    AddRead(
        TEXT("crew.ines"),
        EShipDeck::Hold,
        bWorkingDay ? ECrewHouseholdActivity::Surgery : ECrewHouseholdActivity::OffDuty);
    AddRead(
        TEXT("crew.father_salvio"),
        IsDusk(MinuteOfDay) ? EShipDeck::Weather : EShipDeck::Upper,
        IsDusk(MinuteOfDay) ? ECrewHouseholdActivity::Quiet : ECrewHouseholdActivity::OffDuty);
}

bool UShipHouseholdComponent::IsMealWindow(const int32 MinuteOfDay) const
{
    const bool bMorningMeal = MinuteOfDay >= 420 && MinuteOfDay < 480;
    const bool bEveningMeal = MinuteOfDay >= 1140 && MinuteOfDay < 1200;
    return bMorningMeal || bEveningMeal;
}

bool UShipHouseholdComponent::IsDusk(const int32 MinuteOfDay) const
{
    return MinuteOfDay >= 1020 && MinuteOfDay < 1200;
}

void UShipHouseholdComponent::ClampMorale()
{
    InternalMorale = FMath::Clamp(InternalMorale, 0, 100);
}
