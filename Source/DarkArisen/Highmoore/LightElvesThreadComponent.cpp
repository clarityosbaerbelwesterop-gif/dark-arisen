// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/LightElvesThreadComponent.h"

#include "Engine/World.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

const FName ULightElvesThreadComponent::ThreadId(TEXT("thread.highmoore.light-elves"));

ULightElvesThreadComponent::ULightElvesThreadComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool ULightElvesThreadComponent::HasElevenDaySpanElapsed(
    const int64 StartWorldMinutes,
    const int64 CurrentWorldMinutes)
{
    return StartWorldMinutes >= 0
        && CurrentWorldMinutes >= StartWorldMinutes
        && (CurrentWorldMinutes - StartWorldMinutes) >= ElevenDaysInWorldMinutes;
}

bool ULightElvesThreadComponent::BeginAtDroversRest(const bool bArionEjectionCompleted)
{
    if (Stage != ELightElvesThreadStage::Dormant || !bArionEjectionCompleted)
    {
        return false;
    }

    const int64 Now = ReadWorldMinutes();
    if (Now == INDEX_NONE)
    {
        return false;
    }

    CampaignStartWorldMinutes = Now;
    Stage = ELightElvesThreadStage::RecruitedAtDroversRest;
    return true;
}

bool ULightElvesThreadComponent::RecordLordRefusal(const ELightElvesLordRefusal Lord)
{
    if (Stage != ELightElvesThreadStage::RecruitedAtDroversRest || LordRefusals.Contains(Lord))
    {
        return false;
    }

    LordRefusals.Add(Lord);
    if (LordRefusals.Num() == 4)
    {
        Stage = ELightElvesThreadStage::LordsRefused;
    }
    return true;
}

bool ULightElvesThreadComponent::RecordTakingAccount(const FName VillageAccountId)
{
    if (Stage != ELightElvesThreadStage::LordsRefused
        || VillageAccountId.IsNone()
        || TakingAccounts.Contains(VillageAccountId)
        || TakingAccounts.Num() >= 4)
    {
        return false;
    }

    TakingAccounts.Add(VillageAccountId);
    return true;
}

bool ULightElvesThreadComponent::RecordAbbeyRecordsCounted()
{
    if (Stage != ELightElvesThreadStage::LordsRefused || TakingAccounts.Num() != 4)
    {
        return false;
    }

    Stage = ELightElvesThreadStage::TakingsInvestigated;
    return true;
}

bool ULightElvesThreadComponent::RecordReturnedManHeard()
{
    if (Stage != ELightElvesThreadStage::TakingsInvestigated)
    {
        return false;
    }
    Stage = ELightElvesThreadStage::ReturnedManHeard;
    return true;
}

bool ULightElvesThreadComponent::RecordPracticesLearned()
{
    if (Stage != ELightElvesThreadStage::ReturnedManHeard)
    {
        return false;
    }
    Stage = ELightElvesThreadStage::PracticesLearned;
    return true;
}

bool ULightElvesThreadComponent::RecordMoorSiteResolved(const ELightElvesMoorSite Site)
{
    if (Stage != ELightElvesThreadStage::PracticesLearned
        || ResolvedMoorSites.Contains(Site))
    {
        return false;
    }

    ResolvedMoorSites.Add(Site);
    if (ResolvedMoorSites.Num() == 3)
    {
        Stage = ELightElvesThreadStage::ThreePlacesResolved;
    }
    return true;
}

bool ULightElvesThreadComponent::RecordShaftBossResolved()
{
    if (Stage != ELightElvesThreadStage::ThreePlacesResolved || !HasReachedElevenDays())
    {
        return false;
    }

    Stage = ELightElvesThreadStage::ShaftBossResolved;
    return true;
}

bool ULightElvesThreadComponent::RecordLetterHandedOver(const EEllisLetterResponse Response)
{
    if (Stage != ELightElvesThreadStage::ShaftBossResolved || Response == EEllisLetterResponse::None)
    {
        return false;
    }

    LetterResponse = Response;
    Stage = ELightElvesThreadStage::Complete;
    return true;
}

bool ULightElvesThreadComponent::RecordAdditionalTaking()
{
    if (Stage == ELightElvesThreadStage::Dormant
        || static_cast<uint8>(Stage) >= static_cast<uint8>(ELightElvesThreadStage::ShaftBossResolved))
    {
        return false;
    }

    ++AdditionalTakingCount;
    return true;
}

int64 ULightElvesThreadComponent::GetElapsedWorldMinutes() const
{
    const int64 Now = ReadWorldMinutes();
    if (CampaignStartWorldMinutes == INDEX_NONE || Now == INDEX_NONE || Now < CampaignStartWorldMinutes)
    {
        return 0;
    }
    return Now - CampaignStartWorldMinutes;
}

bool ULightElvesThreadComponent::HasReachedElevenDays() const
{
    return HasElevenDaySpanElapsed(CampaignStartWorldMinutes, ReadWorldMinutes());
}

int64 ULightElvesThreadComponent::ReadWorldMinutes() const
{
    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    return Rules ? Rules->GetTotalWorldMinutes() : INDEX_NONE;
}
