// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Systems/ProgressionEconomyComponent.h"

#include "Math/NumericLimits.h"

#include <initializer_list>

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

bool UProgressionEconomyComponent::CompleteTeachingScene(FName TeacherId, FName NodeId)
{
    if (!TeachersMet.Contains(TeacherId))
    {
        return false;
    }

    const FSkillNodeDefinition* Definition = SkillNodeDefinitions.Find(NodeId);
    if (Definition == nullptr || !Definition->TeacherOptions.Contains(TeacherId))
    {
        return false;
    }

    CompletedTeachingScenes.Add(MakeTeachingSceneKey(TeacherId, NodeId));
    return true;
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

    if (Definition.NodeId.IsNone()
        || Definition.MarkCost < 1
        || Definition.MarkCost > 4
        || !CanonicalBranches.Contains(Definition.BranchId))
    {
        return false;
    }

    if (SkillNodeDefinitions.Contains(Definition.NodeId) || SkillNodeDefinitions.Num() >= RequiredSkillNodeCount)
    {
        return false;
    }

    if (Definition.PrerequisiteNodeIds.Num() > 3)
    {
        return false;
    }

    TSet<FName> UniquePrerequisites;
    for (const FName PrerequisiteId : Definition.PrerequisiteNodeIds)
    {
        if (PrerequisiteId.IsNone()
            || PrerequisiteId == Definition.NodeId
            || UniquePrerequisites.Contains(PrerequisiteId))
        {
            return false;
        }
        UniquePrerequisites.Add(PrerequisiteId);
    }

    const bool bRequiresGate = Definition.GateKind != ESkillGateKind::None;
    if (bRequiresGate == Definition.RequiredGateId.IsNone())
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

    auto Register = [this](
        const TCHAR* NodeId,
        const TCHAR* BranchId,
        int32 Cost,
        std::initializer_list<const TCHAR*> Teachers,
        ESkillGateKind GateKind,
        const TCHAR* GateId,
        int32 MinimumPostureValue)
    {
        FSkillNodeDefinition Definition;
        Definition.NodeId = FName(NodeId);
        Definition.BranchId = FName(BranchId);
        Definition.MarkCost = Cost;
        Definition.GateKind = GateKind;
        Definition.RequiredGateId = FName(GateId);
        Definition.MinimumPosture = MinimumPostureValue;
        for (const TCHAR* Teacher : Teachers)
        {
            Definition.TeacherOptions.Add(FName(Teacher));
        }
        RegisterAuthoredNode(Definition);
    };

    // The bible explicitly names these nodes. The remaining authored nodes are not invented here.
    Register(TEXT("node.blade.seamans_guard"), TEXT("Blade"), 1, {},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.blade.mercy"), TEXT("Blade"), 2, {TEXT("teacher.father_salvio")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.blade.officers_line"), TEXT("Blade"), 3, {TEXT("teacher.don_alejandro")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.blade.big_toms_answer"), TEXT("Blade"), 1, {TEXT("teacher.big_tom")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.blade.long_night"), TEXT("Blade"), 4, {},
        ESkillGateKind::None, TEXT(""), 150);

    Register(TEXT("node.shadow.sound_discipline"), TEXT("Shadow"), 1, {},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.shadow.counting_halls"), TEXT("Shadow"), 2, {TEXT("teacher.mateusz_voltari")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.shadow.fourth_rule"), TEXT("Shadow"), 2, {},
        ESkillGateKind::Standing, TEXT("standing.highmoore.fourth_rule"), 0);

    Register(TEXT("node.sea.estebans_sky"), TEXT("Sea"), 2, {TEXT("teacher.esteban")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.sea.weather_gauge"), TEXT("Sea"), 3,
        {TEXT("teacher.mira"), TEXT("teacher.admiral_sterling")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.sea.hot_shot"), TEXT("Sea"), 2, {TEXT("teacher.maeve_donovan")},
        ESkillGateKind::None, TEXT(""), 0);

    Register(TEXT("node.land.hot_stone_walk"), TEXT("Land"), 2,
        {TEXT("teacher.mbah_seruni")}, ESkillGateKind::Standing, TEXT("standing.hot_stone_walk"), 0);
    Register(TEXT("node.land.cold_is_a_teacher"), TEXT("Land"), 2, {TEXT("teacher.mormor_astrid")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.land.the_line"), TEXT("Land"), 3, {},
        ESkillGateKind::WorldState, TEXT("world.horse_bond_3"), 0);
    Register(TEXT("node.land.the_canopy"), TEXT("Land"), 2, {},
        ESkillGateKind::Standing, TEXT("standing.cultural_web_tier_3"), 0);

    Register(TEXT("node.word.the_register"), TEXT("Word"), 2,
        {TEXT("teacher.margarethe_fitzmueller"), TEXT("teacher.don_alejandro")},
        ESkillGateKind::None, TEXT(""), 0);
    Register(TEXT("node.word.the_invoice"), TEXT("Word"), 2,
        {TEXT("teacher.bram_kettle")}, ESkillGateKind::WorldState, TEXT("world.regulators_turned"), 0);
    Register(TEXT("node.word.whose_man"), TEXT("Word"), 1, {},
        ESkillGateKind::Standing, TEXT("standing.highmoore.whose_man"), 0);
    Register(TEXT("node.word.the_ledger"), TEXT("Word"), 3, {TEXT("teacher.ines")},
        ESkillGateKind::None, TEXT(""), 0);
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

    for (const FName PrerequisiteId : Definition.PrerequisiteNodeIds)
    {
        if (!LearnedNodes.Contains(PrerequisiteId))
        {
            return false;
        }
    }

    if (Definition.GateKind != ESkillGateKind::None
        && !WorldFlags.Contains(Definition.RequiredGateId))
    {
        return false;
    }

    if (!Definition.ExclusiveGroup.IsNone() && ChosenExclusiveGroups.Contains(Definition.ExclusiveGroup))
    {
        return false;
    }

    if (Definition.TeacherOptions.Num() > 0)
    {
        bool bTeachingSceneSatisfied = false;
        for (const FName TeacherId : Definition.TeacherOptions)
        {
            if (HasCompletedTeachingScene(TeacherId, Definition.NodeId))
            {
                bTeachingSceneSatisfied = true;
                break;
            }
        }

        if (!bTeachingSceneSatisfied)
        {
            return false;
        }
    }

    return true;
}

bool UProgressionEconomyComponent::ValidateCompleteCatalog() const
{
    if (SkillNodeDefinitions.Num() != RequiredSkillNodeCount)
    {
        return false;
    }

    int32 BladeCount = 0;
    int32 ShadowCount = 0;
    int32 SeaCount = 0;
    int32 LandCount = 0;
    int32 WordCount = 0;
    int32 TotalMarkCost = 0;
    int32 TeacherGatedNodes = 0;
    int32 StandingGatedNodes = 0;
    TSet<FName> RepresentedTeachers;

    for (const TPair<FName, FSkillNodeDefinition>& Pair : SkillNodeDefinitions)
    {
        const FSkillNodeDefinition& Definition = Pair.Value;
        TotalMarkCost += Definition.MarkCost;

        if (Definition.BranchId == TEXT("Blade")) ++BladeCount;
        else if (Definition.BranchId == TEXT("Shadow")) ++ShadowCount;
        else if (Definition.BranchId == TEXT("Sea")) ++SeaCount;
        else if (Definition.BranchId == TEXT("Land")) ++LandCount;
        else if (Definition.BranchId == TEXT("Word")) ++WordCount;
        else return false;

        if (Definition.TeacherOptions.Num() > 0)
        {
            ++TeacherGatedNodes;
            for (const FName TeacherId : Definition.TeacherOptions)
            {
                RepresentedTeachers.Add(TeacherId);
            }
        }

        if (Definition.GateKind == ESkillGateKind::Standing)
        {
            ++StandingGatedNodes;
        }

        for (const FName PrerequisiteId : Definition.PrerequisiteNodeIds)
        {
            if (!SkillNodeDefinitions.Contains(PrerequisiteId))
            {
                return false;
            }
        }
    }

    if (BladeCount != 16
        || ShadowCount != 12
        || SeaCount != 14
        || LandCount != 13
        || WordCount != 13
        || TotalMarkCost != FullTreeMarkCost
        || TeacherGatedNodes != RequiredTeacherGatedNodeCount
        || StandingGatedNodes != RequiredStandingGatedNodeCount
        || RepresentedTeachers.Num() != RequiredTeacherCount)
    {
        return false;
    }

    for (const FName TeacherId : GetCanonicalTeacherIds())
    {
        if (!RepresentedTeachers.Contains(TeacherId))
        {
            return false;
        }
    }

    return true;
}

bool UProgressionEconomyComponent::HasCompletedTeachingScene(FName TeacherId, FName NodeId) const
{
    return CompletedTeachingScenes.Contains(MakeTeachingSceneKey(TeacherId, NodeId));
}

FName UProgressionEconomyComponent::MakeTeachingSceneKey(FName TeacherId, FName NodeId)
{
    return FName(*FString::Printf(TEXT("%s::%s"), *TeacherId.ToString(), *NodeId.ToString()));
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
