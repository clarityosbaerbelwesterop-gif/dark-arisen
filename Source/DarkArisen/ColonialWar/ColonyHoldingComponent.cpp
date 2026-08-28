// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ColonialWar/ColonyHoldingComponent.h"

UColonyHoldingComponent::UColonyHoldingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UColonyHoldingComponent::ClaimHolding(
    const FName InHoldingId,
    const bool bCastleTakenIntact,
    const bool bOldFortressForAlliance,
    const bool bAbandonedSite,
    const bool bWasRazed)
{
    if (bClaimed
        || InHoldingId.IsNone()
        || bWasRazed
        || (!bCastleTakenIntact && !bOldFortressForAlliance && !bAbandonedSite))
    {
        return false;
    }

    HoldingId = InHoldingId;
    bClaimed = true;
    DevelopmentStage = EHoldingDevelopmentStage::Claimed;
    if (bOldFortressForAlliance)
    {
        HoldingType = EHoldingType::AllianceBastion;
    }
    return true;
}

bool UColonyHoldingComponent::DeclareHoldingType(const EHoldingType NewType)
{
    if (!bClaimed
        || DevelopmentStage != EHoldingDevelopmentStage::Established
        || HoldingType != EHoldingType::Undeclared
        || NewType == EHoldingType::Undeclared
        || NewType == EHoldingType::AllianceBastion)
    {
        return false;
    }

    HoldingType = NewType;
    return true;
}

bool UColonyHoldingComponent::AdvanceDevelopment(const bool bAuthoredRequirementsSatisfied)
{
    if (!bClaimed || !bAuthoredRequirementsSatisfied || DevelopmentStage == EHoldingDevelopmentStage::Thriving)
    {
        return false;
    }

    const uint8 Current = static_cast<uint8>(DevelopmentStage);
    const EHoldingDevelopmentStage Next = static_cast<EHoldingDevelopmentStage>(Current + 1);

    if (Next == EHoldingDevelopmentStage::Established
        && HoldingType != EHoldingType::AllianceBastion
        && DeliveredPeople.IsEmpty())
    {
        return false;
    }
    if (Next == EHoldingDevelopmentStage::Developed
        && HoldingType == EHoldingType::Undeclared)
    {
        return false;
    }

    DevelopmentStage = Next;
    return true;
}

bool UColonyHoldingComponent::ChooseSettlementSpecialisation(
    const ESettlementSpecialisation NewSpecialisation)
{
    if (HoldingType != EHoldingType::Settlement
        || DevelopmentStage < EHoldingDevelopmentStage::Established
        || SettlementSpecialisation != ESettlementSpecialisation::None
        || NewSpecialisation == ESettlementSpecialisation::None)
    {
        return false;
    }

    SettlementSpecialisation = NewSpecialisation;
    return true;
}

bool UColonyHoldingComponent::RecordDeliveredPerson(const FDeliveredHoldingPerson& Person)
{
    if (!bClaimed
        || HoldingType == EHoldingType::AllianceBastion
        || Person.PersonId.IsNone()
        || Person.VoyageId.IsNone()
        || DeliveredPeople.Contains(Person.PersonId))
    {
        return false;
    }

    DeliveredPeople.Add(Person.PersonId, Person);
    return true;
}

bool UColonyHoldingComponent::RecordPersistentDamage(const FName DamageId)
{
    if (!bClaimed || DamageId.IsNone() || PersistentDamageIds.Contains(DamageId))
    {
        return false;
    }
    PersistentDamageIds.Add(DamageId);
    return true;
}

bool UColonyHoldingComponent::RecordDamageRepaired(const FName DamageId)
{
    return !DamageId.IsNone() && PersistentDamageIds.Remove(DamageId) > 0;
}
