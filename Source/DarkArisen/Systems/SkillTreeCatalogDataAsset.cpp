// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Systems/SkillTreeCatalogDataAsset.h"

bool USkillTreeCatalogDataAsset::IsCatalogStructurallyValid() const
{
    if (Nodes.Num() != UProgressionEconomyComponent::RequiredSkillNodeCount)
    {
        return false;
    }

    static const FName BladeBranch(TEXT("Blade"));
    static const FName ShadowBranch(TEXT("Shadow"));
    static const FName SeaBranch(TEXT("Sea"));
    static const FName LandBranch(TEXT("Land"));
    static const FName WordBranch(TEXT("Word"));

    int32 Blade = 0;
    int32 Shadow = 0;
    int32 Sea = 0;
    int32 Land = 0;
    int32 Word = 0;
    int32 Cost = 0;
    int32 TeacherGated = 0;
    int32 StandingGated = 0;
    TSet<FName> Ids;

    for (const FSkillNodeDefinition& Node : Nodes)
    {
        if (Node.NodeId.IsNone()
            || Ids.Contains(Node.NodeId)
            || Node.MarkCost < 1
            || Node.MarkCost > 4
            || Node.PrerequisiteNodeIds.Num() > 3)
        {
            return false;
        }
        Ids.Add(Node.NodeId);
        Cost += Node.MarkCost;

        if (Node.BranchId == BladeBranch) ++Blade;
        else if (Node.BranchId == ShadowBranch) ++Shadow;
        else if (Node.BranchId == SeaBranch) ++Sea;
        else if (Node.BranchId == LandBranch) ++Land;
        else if (Node.BranchId == WordBranch) ++Word;
        else return false;

        if (!Node.TeacherOptions.IsEmpty())
        {
            ++TeacherGated;
            for (const FName Teacher : Node.TeacherOptions)
            {
                if (!UProgressionEconomyComponent::GetCanonicalTeacherIds().Contains(Teacher))
                {
                    return false;
                }
            }
        }

        if (Node.GateKind == ESkillGateKind::Standing)
        {
            ++StandingGated;
        }

        const bool bNeedsGate = Node.GateKind != ESkillGateKind::None;
        if (bNeedsGate == Node.RequiredGateId.IsNone())
        {
            return false;
        }
    }

    if (Blade != 16 || Shadow != 12 || Sea != 14 || Land != 13 || Word != 13
        || Cost != UProgressionEconomyComponent::FullTreeMarkCost
        || TeacherGated != UProgressionEconomyComponent::RequiredTeacherGatedNodeCount
        || StandingGated != UProgressionEconomyComponent::RequiredStandingGatedNodeCount)
    {
        return false;
    }

    for (const FSkillNodeDefinition& Node : Nodes)
    {
        TSet<FName> UniquePrerequisites;
        for (const FName Prerequisite : Node.PrerequisiteNodeIds)
        {
            if (Prerequisite.IsNone()
                || Prerequisite == Node.NodeId
                || !Ids.Contains(Prerequisite)
                || UniquePrerequisites.Contains(Prerequisite))
            {
                return false;
            }
            UniquePrerequisites.Add(Prerequisite);
        }
    }

    return true;
}

bool USkillTreeCatalogDataAsset::RegisterInto(UProgressionEconomyComponent* Progression) const
{
    if (!Progression || !IsCatalogStructurallyValid())
    {
        return false;
    }

    for (const FSkillNodeDefinition& Node : Nodes)
    {
        if (Progression->HasRegisteredNode(Node.NodeId))
        {
            continue;
        }
        if (!Progression->RegisterAuthoredNode(Node))
        {
            return false;
        }
    }
    return Progression->IsSkillCatalogComplete();
}
