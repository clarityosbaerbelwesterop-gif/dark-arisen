// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Ship/ShipVoyageComponent.h"

#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"

UShipVoyageComponent::UShipVoyageComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UShipVoyageComponent::BeginPlay()
{
    Super::BeginPlay();

    if (NamedCrew.Num() == 0)
    {
        BuildCanonicalCrew();
    }

    ActiveHands = FMath::Clamp(ActiveHands, 0, 90);
    HeadingDegrees = NormalizeHeading(HeadingDegrees);
    CommandedHeadingDegrees = NormalizeHeading(CommandedHeadingDegrees);
    WindDirectionDegrees = NormalizeHeading(WindDirectionDegrees);
    RecalculatePointOfSail();
}

void UShipVoyageComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeltaTime <= 0.0f)
    {
        return;
    }

    // Jake leaving the wheel does not stop the vessel. The first mate or a hand stepping up
    // holds the last commanded course; fewer hands reduce response instead of creating a menu.
    const float HandlingFactor = GetCrewHandlingFactor();
    const float HeadingDelta = FMath::FindDeltaAngleDegrees(HeadingDegrees, CommandedHeadingDegrees);
    const float MaximumHeadingStep = ProvisionalHeadingResponseDegreesPerSecond * HandlingFactor * DeltaTime;
    HeadingDegrees = NormalizeHeading(
        HeadingDegrees + FMath::Clamp(HeadingDelta, -MaximumHeadingStep, MaximumHeadingStep));

    RecalculatePointOfSail();

    const float WindDrive = FMath::Max(0.0f, WindStrengthMetresPerSecond) * GetSailEfficiency();
    const float DesiredSpeed = FMath::Min(ProvisionalMaximumSpeedMetresPerSecond, WindDrive) * HandlingFactor;
    ForwardSpeedMetresPerSecond = FMath::FInterpTo(
        ForwardSpeedMetresPerSecond,
        DesiredSpeed,
        DeltaTime,
        0.75f);

    // Sailing is physical world movement. This is intentionally not a route completion,
    // teleport or level transition; collision may stop the vessel and the passage gate measures
    // only distance the actor actually travelled.
    if (AActor* Owner = GetOwner())
    {
        const FRotator HeadingRotation(0.0f, HeadingDegrees, 0.0f);
        Owner->SetActorRotation(HeadingRotation);
        const FVector Forward = FRotationMatrix(HeadingRotation).GetUnitAxis(EAxis::X);
        const FVector DeltaCentimetres =
            Forward * ForwardSpeedMetresPerSecond * 100.0f * DeltaTime;
        Owner->AddActorWorldOffset(DeltaCentimetres, true);
    }
}

void UShipVoyageComponent::SetJakeAtHelm(bool bAtHelm)
{
    bJakeAtHelm = bAtHelm;
}

void UShipVoyageComponent::SetHelmCommandDegrees(float NewHeadingDegrees)
{
    CommandedHeadingDegrees = NormalizeHeading(NewHeadingDegrees);
}

void UShipVoyageComponent::SetWind(float DirectionDegrees, float StrengthMetresPerSecond)
{
    WindDirectionDegrees = NormalizeHeading(DirectionDegrees);
    WindStrengthMetresPerSecond = FMath::Max(0.0f, StrengthMetresPerSecond);
    RecalculatePointOfSail();
}

void UShipVoyageComponent::SetCrewMemberAboard(FName StableId, bool bAboard)
{
    if (FNamedCrewMemberState* CrewMember = NamedCrew.FindByPredicate(
        [StableId](const FNamedCrewMemberState& Entry) { return Entry.StableId == StableId; }))
    {
        CrewMember->bAboard = bAboard && CrewMember->bAlive;
    }
}

void UShipVoyageComponent::SetCrewMemberAlive(FName StableId, bool bAlive)
{
    if (FNamedCrewMemberState* CrewMember = NamedCrew.FindByPredicate(
        [StableId](const FNamedCrewMemberState& Entry) { return Entry.StableId == StableId; }))
    {
        CrewMember->bAlive = bAlive;
        if (!bAlive)
        {
            CrewMember->bAboard = false;
        }
    }
}

void UShipVoyageComponent::SetActiveHands(int32 NewHands)
{
    // Canon range is 40-90 once the mature ship household exists. Zero remains legal
    // for fail-closed tests or authored catastrophic states, but values above 90 do not.
    ActiveHands = FMath::Clamp(NewHands, 0, 90);
}

bool UShipVoyageComponent::AcquirePhysicalChart(FName ChartId)
{
    if (ChartId.IsNone())
    {
        return false;
    }

    if (FPhysicalChartRecord* Existing = PhysicalCharts.FindByPredicate(
        [ChartId](const FPhysicalChartRecord& Entry) { return Entry.ChartId == ChartId; }))
    {
        Existing->bAcquired = true;
        return true;
    }

    FPhysicalChartRecord Record;
    Record.ChartId = ChartId;
    Record.bAcquired = true;
    PhysicalCharts.Add(Record);
    return true;
}

bool UShipVoyageComponent::AnnotatePhysicalChart(FName ChartId)
{
    if (FPhysicalChartRecord* Existing = PhysicalCharts.FindByPredicate(
        [ChartId](const FPhysicalChartRecord& Entry) { return Entry.ChartId == ChartId; }))
    {
        if (!Existing->bAcquired)
        {
            return false;
        }

        Existing->bJakeHasAnnotated = true;
        return true;
    }

    return false;
}

bool UShipVoyageComponent::HasPhysicalChart(FName ChartId) const
{
    const FPhysicalChartRecord* Existing = PhysicalCharts.FindByPredicate(
        [ChartId](const FPhysicalChartRecord& Entry) { return Entry.ChartId == ChartId; });
    return Existing != nullptr && Existing->bAcquired;
}

float UShipVoyageComponent::NormalizeHeading(float Degrees)
{
    float Result = FMath::Fmod(Degrees, 360.0f);
    if (Result < 0.0f)
    {
        Result += 360.0f;
    }
    return Result;
}

void UShipVoyageComponent::RecalculatePointOfSail()
{
    const float RelativeWind = FMath::Abs(
        FMath::FindDeltaAngleDegrees(HeadingDegrees, WindDirectionDegrees));

    if (RelativeWind < 35.0f)
    {
        PointOfSail = EPointOfSail::InIrons;
    }
    else if (RelativeWind < 55.0f)
    {
        PointOfSail = EPointOfSail::CloseHauled;
    }
    else if (RelativeWind < 80.0f)
    {
        PointOfSail = EPointOfSail::CloseReach;
    }
    else if (RelativeWind < 110.0f)
    {
        PointOfSail = EPointOfSail::BeamReach;
    }
    else if (RelativeWind < 150.0f)
    {
        PointOfSail = EPointOfSail::BroadReach;
    }
    else
    {
        PointOfSail = EPointOfSail::Running;
    }
}

float UShipVoyageComponent::GetSailEfficiency() const
{
    switch (PointOfSail)
    {
    case EPointOfSail::InIrons:
        return 0.0f;
    case EPointOfSail::CloseHauled:
        return 0.35f;
    case EPointOfSail::CloseReach:
        return 0.62f;
    case EPointOfSail::BeamReach:
        return 0.88f;
    case EPointOfSail::BroadReach:
        return 1.0f;
    case EPointOfSail::Running:
        return 0.82f;
    default:
        return 0.0f;
    }
}

float UShipVoyageComponent::GetCrewHandlingFactor() const
{
    const FNamedCrewMemberState* Mira = NamedCrew.FindByPredicate(
        [](const FNamedCrewMemberState& CrewMember)
        {
            return CrewMember.StableId == FName(TEXT("crew.mira"));
        });

    // crew_system.md: first mate affects manoeuvre response. If Mira is lost, a hand steps up
    // badly so the mechanical role returns while the person does not. Other named crew — most
    // importantly Father Salvio — do not leak into handling quality.
    const float FirstMateFactor = Mira && Mira->bAlive && Mira->bAboard ? 1.0f : 0.6f;
    const float HandsFactor = FMath::Clamp(
        static_cast<float>(ActiveHands) / 40.0f, 0.15f, 1.0f);
    return FMath::Clamp((FirstMateFactor * 0.35f) + (HandsFactor * 0.65f), 0.15f, 1.0f);
}

void UShipVoyageComponent::BuildCanonicalCrew()
{
    NamedCrew.Reset(5);

    auto AddCrew = [this](
        const TCHAR* StableId,
        const TCHAR* DisplayName,
        const TCHAR* Role,
        ECrewWatch Watch)
    {
        FNamedCrewMemberState Entry;
        Entry.StableId = FName(StableId);
        Entry.DisplayName = FText::FromString(FString(DisplayName));
        Entry.Role = FName(Role);
        Entry.PrimaryWatch = Watch;
        Entry.bAlive = true;
        Entry.bAboard = true;
        NamedCrew.Add(MoveTemp(Entry));
    };

    AddCrew(TEXT("crew.mira"), TEXT("Mira"), TEXT("FirstMate"), ECrewWatch::Dawn);
    AddCrew(TEXT("crew.big_tom"), TEXT("Big Tom"), TEXT("Smith"), ECrewWatch::Midday);
    AddCrew(TEXT("crew.esteban"), TEXT("Esteban"), TEXT("Navigator"), ECrewWatch::Night);
    AddCrew(TEXT("crew.ines"), TEXT("Ines"), TEXT("Surgeon"), ECrewWatch::Dusk);
    AddCrew(TEXT("crew.father_salvio"), TEXT("Father Salvio"), TEXT("Household"), ECrewWatch::Dawn);
}
