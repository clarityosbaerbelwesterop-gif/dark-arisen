// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Missions/MissionScaleRequirements.h"

TArray<FTurnProductionRequirement> FMissionScaleRequirements::BuildTurnRequirements()
{
    return {
        {ETurnProductionRegion::RexaMoran, TEXT("region.rexa-moran"), 22, 3},
        {ETurnProductionRegion::Fjordlund, TEXT("region.fjordlund"), 18, 0},
        {ETurnProductionRegion::Ashenmoor, TEXT("region.ashenmoor"), 16, 0},
        {ETurnProductionRegion::QuietCoast, TEXT("region.quiet-coast"), 14, 0},
        {ETurnProductionRegion::PaleIsleSilvera, TEXT("region.pale-isle-silvera"), 15, 0},
        {ETurnProductionRegion::AtSea, TEXT("region.at-sea"), 19, 0},
        {ETurnProductionRegion::Highmoore, TEXT("region.highmoore"), 24, 0},
        {ETurnProductionRegion::Region06, TEXT("region.06"), 4, 0}
    };
}

TArray<FStandingTypeProductionRequirement> FMissionScaleRequirements::BuildStandingTypeRequirements()
{
    return {
        {EM7StandingMissionType::Escort, 21, 0, TEXT("mission types.md Section 3")},
        {EM7StandingMissionType::ConvoyRaid, 18, 0, TEXT("mission types.md Section 4")},
        {EM7StandingMissionType::Recovery, 24, 0, TEXT("mission types.md Section 5")},
        {EM7StandingMissionType::Champion, 12, 0, TEXT("mission types.md Section 6")},
        {EM7StandingMissionType::Transport, 16, 0, TEXT("mission types.md Section 7")},
        {EM7StandingMissionType::PrivateerCommission, 14, 0, TEXT("mission types.md Section 2")},
        {EM7StandingMissionType::RoadWork, 15, 0, TEXT("mission types.md Section 9")},
        {EM7StandingMissionType::Hunt, 13, 0, TEXT("mission types.md Section 9")},
        {EM7StandingMissionType::Salvage, 14, 1, TEXT("mission types.md Section 9; RexaM2MissionCatalog.cpp")}
    };
}

TArray<FMissionScaleDesignGap> FMissionScaleRequirements::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.turn-129-identities"),
            TEXT("The exact regional distribution requires 132 finite Turns. Three Rexa Turns are currently individually authored in native source; the remaining 129 require hand-written identities, people, places and outcomes."),
            TEXT("side quest catalog.md Section 4; RexaM2MissionCatalog.cpp")
        },
        {
            TEXT("design-gap.standing-146-identities"),
            TEXT("The nine Standing structures require 147 finite variants. Only the San Telmo Bell salvage variant is currently a complete native identity. The remaining 146 require authored people/places/outcomes rather than generated templates."),
            TEXT("mission types.md Sections 1-10; RexaM2MissionCatalog.cpp")
        },
        {
            TEXT("design-gap.standing-region-allocation"),
            TEXT("Phase 11 fixes Standing totals by mission type and says pools are finite per region/type, but it does not publish a complete region-by-type allocation matrix. Source must not invent one."),
            TEXT("mission types.md Sections 1.2, 2 and 10")
        },
        {
            TEXT("design-gap.pay-curve-middle"),
            TEXT("Canon fixes first same-region/type run at 100 percent and sixth at 60 percent. Intermediate pay multipliers remain authored tuning and may not be guessed."),
            TEXT("mission types.md Section 10.2; economy core.md Section 6")
        }
    };
}

EAuthoredMissionSlotState FMissionScaleRequirements::ResolveKnownIdentity(const FName QuestId)
{
    static const TSet<FName> Known = {
        TEXT("Rexa.Turn.EmptyHammock"),
        TEXT("Rexa.Turn.ThreeCutsInStone"),
        TEXT("Rexa.Turn.SaltLedger"),
        TEXT("Rexa.Standing.Salvage.SanTelmoBell")
    };
    return Known.Contains(QuestId)
        ? EAuthoredMissionSlotState::AuthoredIdentity
        : EAuthoredMissionSlotState::ReservedUnauthored;
}

bool FMissionScaleRequirements::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    int32 TurnTotal = 0;
    int32 AuthoredTurns = 0;
    TSet<FName> TurnRegions;
    for (const FTurnProductionRequirement& Requirement : BuildTurnRequirements())
    {
        if (Requirement.RegionId.IsNone() || Requirement.RequiredCount <= 0 || TurnRegions.Contains(Requirement.RegionId))
        {
            OutErrors.Add(TEXT("Turn production requirement has invalid/duplicate region."));
            continue;
        }
        if (Requirement.AuthoredIdentityCount < 0 || Requirement.AuthoredIdentityCount > Requirement.RequiredCount)
        {
            OutErrors.Add(FString::Printf(TEXT("Turn authored identity count is invalid for %s."), *Requirement.RegionId.ToString()));
        }
        TurnRegions.Add(Requirement.RegionId);
        TurnTotal += Requirement.RequiredCount;
        AuthoredTurns += Requirement.AuthoredIdentityCount;
    }
    if (TurnTotal != RequiredTurnCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Turn production total must be %d; found %d."), RequiredTurnCount, TurnTotal));
    }
    if (AuthoredTurns != KnownTurnIdentityCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Known Turn identity count must be %d until more authored missions are added; found %d."), KnownTurnIdentityCount, AuthoredTurns));
    }

    int32 StandingTotal = 0;
    int32 AuthoredStanding = 0;
    TSet<EM7StandingMissionType> StandingTypes;
    for (const FStandingTypeProductionRequirement& Requirement : BuildStandingTypeRequirements())
    {
        if (Requirement.Type == EM7StandingMissionType::None
            || Requirement.RequiredCount <= 0
            || Requirement.GoverningSource.IsEmpty()
            || StandingTypes.Contains(Requirement.Type))
        {
            OutErrors.Add(TEXT("Standing production requirement has invalid/duplicate type."));
            continue;
        }
        if (Requirement.AuthoredIdentityCount < 0 || Requirement.AuthoredIdentityCount > Requirement.RequiredCount)
        {
            OutErrors.Add(TEXT("Standing authored identity count is outside its finite type pool."));
        }
        StandingTypes.Add(Requirement.Type);
        StandingTotal += Requirement.RequiredCount;
        AuthoredStanding += Requirement.AuthoredIdentityCount;
    }
    if (StandingTotal != RequiredStandingCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Standing production total must be %d; found %d."), RequiredStandingCount, StandingTotal));
    }
    if (AuthoredStanding != KnownStandingIdentityCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Known Standing identity count must remain %d until source authors more variants; found %d."), KnownStandingIdentityCount, AuthoredStanding));
    }

    if (BuildDesignGaps().Num() != 4)
    {
        OutErrors.Add(TEXT("Mission production must retain the four current source gaps instead of filling them procedurally."));
    }

    return OutErrors.IsEmpty();
}
