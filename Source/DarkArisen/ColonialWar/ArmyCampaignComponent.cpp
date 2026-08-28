// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/ArmyCampaignComponent.h"

UArmyCampaignComponent::UArmyCampaignComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UArmyCampaignComponent::RegisterForce(const FCampaignForceDefinition& Definition)
{
    if (Definition.ForceId.IsNone()
        || Definition.SourceId.IsNone()
        || Definition.Men <= 0
        || Forces.Contains(Definition.ForceId))
    {
        return false;
    }

    Forces.Add(Definition.ForceId, Definition);
    UnpaidChapters.Add(Definition.ForceId, 0);
    return true;
}

bool UArmyCampaignComponent::SetNamedOfficerCount(const int32 Count)
{
    if (Count < 0 || Count > 5)
    {
        return false;
    }
    NamedOfficerCount = Count;
    return true;
}

bool UArmyCampaignComponent::RecordForcePaid(const FName ForceId, const bool bPaid)
{
    if (!Forces.Contains(ForceId) || DepartedForces.Contains(ForceId))
    {
        return false;
    }

    if (bPaid)
    {
        PaidThisChapter.Add(ForceId);
    }
    else
    {
        PaidThisChapter.Remove(ForceId);
    }
    return true;
}

void UArmyCampaignComponent::AdvanceChapter()
{
    for (const TPair<FName, FCampaignForceDefinition>& Pair : Forces)
    {
        const FName ForceId = Pair.Key;
        const FCampaignForceDefinition& Force = Pair.Value;
        if (DepartedForces.Contains(ForceId) || Force.Type == ECampaignForceType::Crew)
        {
            continue;
        }

        const bool bRequiresCashPay = Force.Type == ECampaignForceType::Hired
            || Force.Type == ECampaignForceType::SettlementMilitia;

        if (!bRequiresCashPay || PaidThisChapter.Contains(ForceId))
        {
            UnpaidChapters.FindOrAdd(ForceId) = 0;
            continue;
        }

        int32& Unpaid = UnpaidChapters.FindOrAdd(ForceId);
        ++Unpaid;

        if ((Force.bCompanyMarines && Unpaid >= 1)
            || (Force.Type == ECampaignForceType::Hired && Unpaid >= 2))
        {
            DepartedForces.Add(ForceId);
            LastOrders.Remove(ForceId);
        }
    }

    PaidThisChapter.Reset();
}

bool UArmyCampaignComponent::IssueOrder(const FName ForceId, const EArmyOrder Order)
{
    const FCampaignForceDefinition* Force = Forces.Find(ForceId);
    if (!Force || DepartedForces.Contains(ForceId) || Force->Type == ECampaignForceType::Alliance)
    {
        return false;
    }

    LastOrders.FindOrAdd(ForceId) = Order;
    return true;
}

EForceMoraleRead UArmyCampaignComponent::GetForceMoraleRead(const FName ForceId) const
{
    const FCampaignForceDefinition* Force = Forces.Find(ForceId);
    if (!Force || DepartedForces.Contains(ForceId))
    {
        return EForceMoraleRead::Departed;
    }

    const int32 Unpaid = UnpaidChapters.FindRef(ForceId);
    if (Unpaid >= 2)
    {
        return EForceMoraleRead::Fraying;
    }
    if (Unpaid == 1)
    {
        return EForceMoraleRead::Quiet;
    }
    return EForceMoraleRead::Steady;
}

int32 UArmyCampaignComponent::GetCommandCapacity() const
{
    return JakeBaseCommandCapacity + NamedOfficerCount * CapacityPerNamedOfficer;
}

bool UArmyCampaignComponent::IsOverCommandCapacity() const
{
    return GetArmyMenForTests() > GetCommandCapacity();
}

int64 UArmyCampaignComponent::CalculateChapterDoubloonUpkeep() const
{
    int64 Total = 0;
    for (const TPair<FName, FCampaignForceDefinition>& Pair : Forces)
    {
        const FCampaignForceDefinition& Force = Pair.Value;
        if (DepartedForces.Contains(Pair.Key)
            || Force.Type == ECampaignForceType::Alliance
            || Force.Type == ECampaignForceType::FactionLevy
            || Force.Type == ECampaignForceType::Crew)
        {
            continue;
        }

        int64 Rate = Force.Type == ECampaignForceType::SettlementMilitia
            ? MilitiaUpkeepPerMan
            : HiredUpkeepPerMan;
        if (Force.bCavalry)
        {
            Rate *= 3;
        }
        if (Force.bSpecialist)
        {
            Rate *= 2;
        }
        Total += Rate * static_cast<int64>(Force.Men);
    }
    return Total;
}

int32 UArmyCampaignComponent::GetArmyMenForTests() const
{
    int32 Men = 0;
    for (const TPair<FName, FCampaignForceDefinition>& Pair : Forces)
    {
        if (!DepartedForces.Contains(Pair.Key) && Pair.Value.Type != ECampaignForceType::Crew)
        {
            Men += Pair.Value.Men;
        }
    }
    return Men;
}

bool UArmyCampaignComponent::IsForceActiveForTests(const FName ForceId) const
{
    return Forces.Contains(ForceId) && !DepartedForces.Contains(ForceId);
}
