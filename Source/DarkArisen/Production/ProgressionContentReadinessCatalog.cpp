// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ProgressionContentReadinessCatalog.h"

#include "Systems/ProgressionEconomyComponent.h"

namespace
{
FProgressionContentReadinessRecord Record(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const int32 Required,
    const int32 Grounded,
    const int32 Gaps,
    const EProgressionContentReadiness Readiness,
    const TCHAR* Boundary)
{
    FProgressionContentReadinessRecord Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.RequiredCount = Required;
    Result.GroundedCount = Grounded;
    Result.ExplicitGapCount = Gaps;
    Result.Readiness = Readiness;
    Result.Boundary = Boundary;
    return Result;
}
}

TArray<FProgressionContentReadinessRecord> FProgressionContentReadinessCatalog::BuildRecords()
{
    return {
        Record(
            TEXT("progression.craft.skill-nodes"),
            TEXT("CRAFT skill-node identities"),
            TEXT("skill tree.md Sections 1, 4 and 6; UProgressionEconomyComponent"),
            RequiredSkillNodeCount,
            GroundedSkillNodeCount,
            SkillNodeIdentityGapCount,
            EProgressionContentReadiness::Partial,
            TEXT("The tree is structurally locked at 68 nodes across 16/12/14/13/13 branches, but the bible individually names only nineteen nodes. The remaining forty-nine identities, verbs, costs, prerequisites and gates are not invented.")),

        Record(
            TEXT("progression.craft.teachers"),
            TEXT("CRAFT named teachers"),
            TEXT("skill tree.md Section 5; UProgressionEconomyComponent::GetCanonicalTeacherIds"),
            RequiredTeacherCount,
            GroundedTeacherCount,
            0,
            EProgressionContentReadiness::Authored,
            TEXT("All twenty-three teacher identities are named. This does not imply all twenty-three teacher-gated node identities or teaching scenes are authored, because those are part of the skill-node gap above.")),

        Record(
            TEXT("progression.body.physicians-draughts"),
            TEXT("Physician's Draught placements"),
            TEXT("skill tree.md Section 2.2"),
            RequiredPhysiciansDraughtCount,
            GroundedPhysiciansDraughtPlacementCount,
            PhysiciansDraughtPlacementGapCount,
            EProgressionContentReadiness::Partial,
            TEXT("Nine of twelve placements are assigned to named practitioners: Brandt x2, Doña Mercedes x2, Helga Thorsdotter x1, Bu Wira x2, Madame Celestine x1 and Doc Wallace x1. The remaining three are only described as being in dungeons and are not assigned to individual dungeon identities.")),

        Record(
            TEXT("progression.body.deep-water-pearls"),
            TEXT("Deep-Water Pearl placements"),
            TEXT("skill tree.md Section 2.3"),
            RequiredDeepWaterPearlCount,
            GroundedDeepWaterPearlPlacementCount,
            DeepWaterPearlPlacementGapCount,
            EProgressionContentReadiness::Partial,
            TEXT("Sixteen pearls are required and six require Alliance routes, but the source does not individually identify the sixteen underwater placements. No coordinates, sites or substitute treasure locations are invented.")),

        Record(
            TEXT("progression.body.posture-deflections"),
            TEXT("Named posture-growth deflection sources"),
            TEXT("skill tree.md Section 2.4; Docs/M7_TIER1_BOSS_REGISTER.md; assassin network.md"),
            RequiredPostureDeflectionSourceCount,
            UncontestedTier1DeflectionSourceCount,
            PostureDeflectionSelectionGapCount,
            EProgressionContentReadiness::BlockedByAuthority,
            TEXT("The source locks fifteen total sources and says each Tier-1 boss and each named assassin qualifies. The nine Tier-1 commanders are unambiguous, but the current assassin roster contains more named hunters than the six remaining slots. Which six qualify is unresolved and must not be selected by implementation.")),

        Record(
            TEXT("progression.marks.specific-acts"),
            TEXT("One-off specific Mark acts"),
            TEXT("skill tree.md Section 3.3"),
            RequiredSpecificMarkActCount,
            GroundedSpecificMarkActCount,
            SpecificMarkActGapCount,
            EProgressionContentReadiness::Partial,
            TEXT("Six acts are explicitly listed: Ashenmoor desert crossing, Fjordlund winter storm without mast loss, ancient-map language read, Crystal Caves no-torch past Cathedral, Horse Bond 3 and twenty returned artifacts. The document reserves six further acts without identifying them."))
    };
}

TArray<FProgressionContentDesignGap> FProgressionContentReadinessCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.progression.skill-node-identities"),
            TEXT("Forty-nine of the required sixty-eight CRAFT nodes have no individual authored identity in the current skill-tree source. Do not create filler verbs, names, teachers, prerequisites or costs."),
            TEXT("skill tree.md Sections 4 and 6")
        },
        {
            TEXT("design-gap.progression.draught-dungeon-placements"),
            TEXT("Three of the twelve Physician's Draughts are only described as dungeon finds; their exact dungeon identities/physical placements are not authored."),
            TEXT("skill tree.md Section 2.2")
        },
        {
            TEXT("design-gap.progression.deep-water-pearl-placements"),
            TEXT("All sixteen Deep-Water Pearls require authored underwater placements; the source locks the count and six Alliance-route gates but does not identify individual sites."),
            TEXT("skill tree.md Section 2.3")
        },
        {
            TEXT("design-gap.progression.posture-source-selection"),
            TEXT("The posture source total is fifteen. Nine Tier-1 commanders are unambiguous, but the named-hunter roster exceeds the six remaining slots, so implementation cannot determine which assassin encounters grant the remaining posture increases."),
            TEXT("skill tree.md Section 2.4; assassin network.md")
        },
        {
            TEXT("design-gap.progression.specific-mark-acts"),
            TEXT("Six of the twelve one-off specific Mark acts are not individually identified by the current skill-tree source."),
            TEXT("skill tree.md Section 3.3")
        }
    };
}

bool FProgressionContentReadinessCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    if (RequiredSkillNodeCount != UProgressionEconomyComponent::RequiredSkillNodeCount)
    {
        OutErrors.Add(TEXT("Progression readiness skill-node total drifted from UProgressionEconomyComponent."));
    }

    if (RequiredTeacherCount != UProgressionEconomyComponent::RequiredTeacherCount
        || UProgressionEconomyComponent::GetCanonicalTeacherIds().Num() != RequiredTeacherCount)
    {
        OutErrors.Add(TEXT("Progression readiness teacher total drifted from the canonical teacher roster."));
    }

    if (GroundedSkillNodeCount + SkillNodeIdentityGapCount != RequiredSkillNodeCount)
    {
        OutErrors.Add(TEXT("Grounded + missing CRAFT node identities must close to sixty-eight."));
    }

    if (GroundedPhysiciansDraughtPlacementCount + PhysiciansDraughtPlacementGapCount != RequiredPhysiciansDraughtCount)
    {
        OutErrors.Add(TEXT("Physician's Draught placement accounting must close to twelve."));
    }

    if (GroundedDeepWaterPearlPlacementCount + DeepWaterPearlPlacementGapCount != RequiredDeepWaterPearlCount)
    {
        OutErrors.Add(TEXT("Deep-Water Pearl placement accounting must close to sixteen."));
    }

    if (UncontestedTier1DeflectionSourceCount + PostureDeflectionSelectionGapCount != RequiredPostureDeflectionSourceCount)
    {
        OutErrors.Add(TEXT("Posture deflection-source accounting must close to fifteen without guessing assassin identities."));
    }

    if (GroundedSpecificMarkActCount + SpecificMarkActGapCount != RequiredSpecificMarkActCount)
    {
        OutErrors.Add(TEXT("Specific one-off Mark-act accounting must close to twelve."));
    }

    const TArray<FProgressionContentReadinessRecord> Records = BuildRecords();
    if (Records.Num() != RequiredReadinessFamilyCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Progression readiness requires %d families; found %d."), RequiredReadinessFamilyCount, Records.Num()));
    }

    TSet<FName> StableIds;
    for (const FProgressionContentReadinessRecord& Entry : Records)
    {
        if (Entry.StableId.IsNone() || Entry.GoverningSource.IsEmpty() || Entry.Boundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Progression readiness record lacks stable id, governing source or boundary."));
            continue;
        }

        if (StableIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate progression readiness id: %s"), *Entry.StableId.ToString()));
        }
        StableIds.Add(Entry.StableId);

        if (Entry.GroundedCount + Entry.ExplicitGapCount != Entry.RequiredCount)
        {
            OutErrors.Add(FString::Printf(
                TEXT("Progression readiness does not close for %s: required=%d grounded=%d gaps=%d."),
                *Entry.StableId.ToString(), Entry.RequiredCount, Entry.GroundedCount, Entry.ExplicitGapCount));
        }
    }

    if (BuildDesignGaps().Num() != 5)
    {
        OutErrors.Add(TEXT("Exactly five current progression authoring gaps must remain explicit until design authority changes."));
    }

    return OutErrors.IsEmpty();
}
