// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Systems/ProgressionEconomyComponent.h"

#include "Math/NumericLimits.h"

UProgressionEconomyComponent::UProgressionEconomyComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UProgressionEconomyComponent::BeginPlay()
{
    Super::BeginPlay();
    RegisterKnownCanonicalNodes();
}

bool UProgressionEconomyComponent::ApplyPhysiciansDraught(FName SourceId)
{
    if (SourceId.IsNone() || PhysiciansDraughtSources.Contains(SourceId) || PhysiciansDraughtSources.Num() >= 12)
    {
        return false;
    }

    PhysiciansDraughtSources.Add(SourceId);
    MaximumHealth = FMath::Min(380, MaximumHealth + 15);
    return true;
}

bool UProgressionEconomyComponent::ApplyDeepWaterPearl(FName SourceId)
{
    if (SourceId.IsNone() || DeepWaterPearlSources.Contains(SourceId) || DeepWaterPearlSources.Num() >= 16)
    {
        return false;
    }

    DeepWaterPearlSources.Add(SourceId);
    MaximumStamina = FMath::Min(200, MaximumStamina + 5);
    return true;
}

bool UProgressionEconomyComponent::RecordNamedDeflectionSet(FName OpponentId)
{
    if (OpponentId.IsNone() || NamedDeflectionSources.Contains(OpponentId) || NamedDeflectionSources.Num() >= 15)
    {
        return false;
    }

    NamedDeflectionSources.Add(OpponentId);
    MaximumPosture = FMath::Min(175, MaximumPosture + 5);
    return true;
}

bool UProgressionEconomyComponent::RecordCarryMilestone(FName MilestoneId, float AuthoredCarryKg)
{
    if (MilestoneId.IsNone() || CarryMilestones.Contains(MilestoneId))
    {
        return false;
    }

    if (AuthoredCarryKg <= CarryKilograms || AuthoredCarryKg > 130.0f)
    {
        return false;
    }

    CarryMilestones.Add(MilestoneId);
    CarryKilograms = AuthoredCarryKg;
    return true;
}

bool UProgressionEconomyComponent::AwardMarks(FName AuthoredSourceId, int32 Amount)
{
    if (AuthoredSourceId.IsNone() || Amount <= 0 || AwardedMarkSources.Contains(AuthoredSourceId))
    {
        return false;
    }

    if (MarksEarned > MaximumAvailableMarks - Amount)
    {
        return false;
    }

    AwardedMarkSources.Add(AuthoredSourceId);
    MarksEarned += Amount;
    return true;
}

void UProgressionEconomyComponent::RecordTeacherMet(FName TeacherId)
{
    if (!TeacherId.IsNone() && GetCanonicalTeacherIds().Contains(TeacherId))
    {
        TeachersMet.Add(TeacherId);
    }
}

void UProgressionEconomyComponent::SetWorldFlag(FName FlagId, bool bEnabled)
{
    if (FlagId.IsNone())
    {
        return;
    }

    if (bEnabled)
    {
        WorldFlags.Add(FlagId);
    }
    else
    {
        WorldFlags.Remove(FlagId);
    }
}

bool UProgressionEconomyComponent::RegisterAuthoredNode(const FSkillNodeDefinition& Definition)
{
    static const TSet<FName> CanonicalBranches = {
        TEXT("Blade"), TEXT("Shadow"), TEXT("Sea"), TEXT("Land"), TEXT("Word")
    };

    if (Definition.NodeId.IsNone() || Definition.MarkCost <= 0 || !CanonicalBranches.Contains(Definition.BranchId))
    {
        return false;
    }

    if (SkillNodeDefinitions.Contains(Definition.NodeId) || SkillNodeDefinitions.Num() >= RequiredSkillNodeCount)
    {
        return false;
    }

    for (const FName TeacherId : Definition.TeacherOptions)
    {
        if (!GetCanonicalTeacherIds().Contains(TeacherId))
        {
            return false;
        }
    }

    SkillNodeDefinitions.Add(Definition.NodeId, Definition);
    return true;
}

bool UProgressionEconomyComponent::TryLearnNode(FName NodeId)
{
    const FSkillNodeDefinition* Definition = SkillNodeDefinitions.Find(NodeId);
    if (Definition == nullptr || !DefinitionCanBeLearned(*Definition))
    {
        return false;
    }

    MarksSpent += Definition->MarkCost;
    LearnedNodes.Add(NodeId);

    if (!Definition->ExclusiveGroup.IsNone())
    {
        ChosenExclusiveGroups.Add(Definition->ExclusiveGroup);
    }

    return true;
}

const TArray<FName>& UProgressionEconomyComponent::GetCanonicalTeacherIds()
{
    static const TArray<FName> Teachers = {
        TEXT("teacher.mira"),
        TEXT("teacher.big_tom"),
        TEXT("teacher.ines"),
        TEXT("teacher.father_salvio"),
        TEXT("teacher.esteban"),
        TEXT("teacher.mama_jacinta"),
        TEXT("teacher.mbah_seruni"),
        TEXT("teacher.mormor_astrid"),
        TEXT("teacher.don_alejandro"),
        TEXT("teacher.mateusz_voltari"),
        TEXT("teacher.cassandra_wells"),
        TEXT("teacher.dr_eleanor_whitcomb"),
        TEXT("teacher.lt_marcus_sandford"),
        TEXT("teacher.yara_sun_walker"),
        TEXT("teacher.pak_setyo"),
        TEXT("teacher.bu_wira"),
        TEXT("teacher.isolde_thorn"),
        TEXT("teacher.gruffydd"),
        TEXT("teacher.maeve_donovan"),
        TEXT("teacher.margarethe_fitzmueller"),
        TEXT("teacher.admiral_sterling"),
        TEXT("teacher.bram_kettle"),
        TEXT("teacher.wooden_tooth_holloway")
    };
    return Teachers;
}

bool UProgressionEconomyComponent::CreditCurrency(EDarkArisenCurrency Currency, int64 Amount)
{
    if (Amount <= 0)
    {
        return false;
    }

    int64& Balance = ResolveCurrencyMutable(Currency);
    if (Balance > TNumericLimits<int64>::Max() - Amount)
    {
        return false;
    }

    Balance += Amount;
    return true;
}

bool UProgressionEconomyComponent::SpendCurrency(EDarkArisenCurrency Currency, int64 Amount)
{
    if (Amount <= 0)
    {
        return false;
    }

    int64& Balance = ResolveCurrencyMutable(Currency);
    if (Balance < Amount)
    {
        return false;
    }

    Balance -= Amount;
    return true;
}

void UProgressionEconomyComponent::SetGreetingState(FName SocialContextId, ESocialGreetingState NewState)
{
    if (!SocialContextId.IsNone())
    {
        GreetingStates.FindOrAdd(SocialContextId) = NewState;
    }
}

ESocialGreetingState UProgressionEconomyComponent::GetGreetingState(FName SocialContextId) const
{
    if (const ESocialGreetingState* Existing = GreetingStates.Find(SocialContextId))
    {
        return *Existing;
    }
    return ESocialGreetingState::Stranger;
}

bool UProgressionEconomyComponent::BeginListening(FName ConversationId)
{
    if (ConversationId.IsNone() || !ActiveListeningConversation.IsNone())
    {
        return false;
    }

    ActiveListeningConversation = ConversationId;
    return true;
}

void UProgressionEconomyComponent::InterruptListening()
{
    ActiveListeningConversation = NAME_None;
}

bool UProgressionEconomyComponent::CompleteListening()
{
    if (ActiveListeningConversation.IsNone())
    {
        return false;
    }

    OverheardConversations.Add(ActiveListeningConversation);
    ActiveListeningConversation = NAME_None;
    return true;
}

void UProgressionEconomyComponent::RegisterKnownCanonicalNodes()
{
    if (SkillNodeDefinitions.Num() > 0)
    {
        return;
    }

    auto Register = [this](const TCHAR* NodeId, const TCHAR* BranchId, int32 Cost,
        std::initializer_list<const TCHAR*> Teachers, const TCHAR* WorldFlag = TEXT(""), int32 MinimumPostureValue = 0)
    {
        FSkillNodeDefinition Definition;
        Definition.NodeId = FName(NodeId);
        Definition.BranchId = FName(BranchId);
        Definition.MarkCost = Cost;
        Definition.RequiredWorldFlag = FName(WorldFlag);
        Definition.MinimumPosture = MinimumPostureValue;
        for (const TCHAR* Teacher : Teachers)
        {
            Definition.TeacherOptions.Add(FName(Teacher));
        }
        RegisterAuthoredNode(Definition);
    };

    // The bible explicitly names these nodes. The remaining authored nodes are not invented here.
    Register(TEXT("node.blade.seamans_guard"), TEXT("Blade"), 1, {});
    Register(TEXT("node.blade.mercy"), TEXT("Blade"), 2, {TEXT("teacher.father_salvio")});
    Register(TEXT("node.blade.officers_line"), TEXT("Blade"), 3, {TEXT("teacher.don_alejandro")});
    Register(TEXT("node.blade.big_toms_answer"), TEXT("Blade"), 1, {TEXT("teacher.big_tom")});
    Register(TEXT("node.blade.long_night"), TEXT("Blade"), 4, {}, TEXT(""), 150);

    Register(TEXT("node.shadow.sound_discipline"), TEXT("Shadow"), 1, {});
    Register(TEXT("node.shadow.counting_halls"), TEXT("Shadow"), 2, {TEXT("teacher.mateusz_voltari")});
    Register(TEXT("node.shadow.fourth_rule"), TEXT("Shadow"), 2, {}, TEXT("standing.highmoore.fourth_rule"));

    Register(TEXT("node.sea.estebans_sky"), TEXT("Sea"), 2, {TEXT("teacher.esteban")});
    Register(TEXT("node.sea.weather_gauge"), TEXT("Sea"), 3,
        {TEXT("teacher.mira"), TEXT("teacher.admiral_sterling")});
    Register(TEXT("node.sea.hot_shot"), TEXT("Sea"), 2, {TEXT("teacher.maeve_donovan")});

    Register(TEXT("node.land.hot_stone_walk"), TEXT("Land"), 2,
        {TEXT("teacher.mbah_seruni")}, TEXT("standing.hot_stone_walk"));
    Register(TEXT("node.land.cold_is_a_teacher"), TEXT("Land"), 2, {TEXT("teacher.mormor_astrid")});
    Register(TEXT("node.land.the_line"), TEXT("Land"), 3, {}, TEXT("world.horse_bond_3"));
    Register(TEXT("node.land.the_canopy"), TEXT("Land"), 2, {}, TEXT("standing.cultural_web_tier_3"));

    Register(TEXT("node.word.the_register"), TEXT("Word"), 2,
        {TEXT("teacher.margarethe_fitzmueller"), TEXT("teacher.don_alejandro")});
    Register(TEXT("node.word.the_invoice"), TEXT("Word"), 2,
        {TEXT("teacher.bram_kettle")}, TEXT("world.regulators_turned"));
    Register(TEXT("node.word.whose_man"), TEXT("Word"), 1, {}, TEXT("standing.highmoore.whose_man"));
    Register(TEXT("node.word.the_ledger"), TEXT("Word"), 3, {TEXT("teacher.ines")});
}

bool UProgressionEconomyComponent::DefinitionCanBeLearned(const FSkillNodeDefinition& Definition) const
{
    if (LearnedNodes.Contains(Definition.NodeId))
    {
        return false;
    }

    if (MarksEarned - MarksSpent < Definition.MarkCost)
    {
        return false;
    }

    if (MaximumPosture < Definition.MinimumPosture)
    {
        return false;
    }

    if (!Definition.RequiredWorldFlag.IsNone() && !WorldFlags.Contains(Definition.RequiredWorldFlag))
    {
        return false;
    }

    if (!Definition.ExclusiveGroup.IsNone() && ChosenExclusiveGroups.Contains(Definition.ExclusiveGroup))
    {
        return false;
    }

    if (Definition.TeacherOptions.Num() > 0)
    {
        bool bTeacherSatisfied = false;
        for (const FName TeacherId : Definition.TeacherOptions)
        {
            if (TeachersMet.Contains(TeacherId))
            {
                bTeacherSatisfied = true;
                break;
            }
        }

        if (!bTeacherSatisfied)
        {
            return false;
        }
    }

    return true;
}

int64& UProgressionEconomyComponent::ResolveCurrencyMutable(EDarkArisenCurrency Currency)
{
    switch (Currency)
    {
    case EDarkArisenCurrency::Doubloons:
        return Wallet.Doubloons;
    case EDarkArisenCurrency::Pounds:
        return Wallet.Pounds;
    case EDarkArisenCurrency::SilverMarks:
        return Wallet.SilverMarks;
    default:
        checkNoEntry();
        return Wallet.Doubloons;
    }
}
