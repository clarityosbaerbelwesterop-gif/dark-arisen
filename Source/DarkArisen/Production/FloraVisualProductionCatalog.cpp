// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/FloraVisualProductionCatalog.h"

namespace
{
FFloraVisualProductionBrief Flora(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EFloraVisualProductionKind Kind,
    const TCHAR* Source,
    const TCHAR* Visual,
    const TCHAR* Phenomenon,
    const TCHAR* Environment,
    const TCHAR* Boundary,
    const bool bProviderReady)
{
    FFloraVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Kind = Kind;
    Result.GoverningSource = Source;
    Result.AuthoredVisualFacts = Visual;
    Result.BehaviorOrPhenomenonRead = Phenomenon;
    Result.EnvironmentRead = Environment;
    Result.AntiInventionBoundary = Boundary;
    Result.bProviderReferenceReady = bProviderReady;
    return Result;
}
}

TArray<FFloraVisualProductionBrief> FFloraVisualProductionCatalog::BuildBriefs()
{
    return {
        Flora(
            TEXT("flora.legendary.weeping-mother"), TEXT("The Weeping Mother"), EFloraVisualProductionKind::LegendaryTree,
            TEXT("docs/design/fauna/plants_and_herbs.md Section 7.1; docs/design/fauna/trees_and_fruits.md Section 8.1"),
            TEXT("Single pale-silver legendary willow, approximately 60 m tall, with long branches carrying amber sap globules described as tears."),
            TEXT("Rare amber sap forms and falls; the tree itself must not be damaged. The source says it seems to respond to presence without defining overt sentience."),
            TEXT("A single remote coastal location known to few sailors."),
            TEXT("Do not add a face, humanoid anatomy, literal crying eyes or magical particles. Amber tears are sap; emotional meaning comes from context."),
            true),
        Flora(
            TEXT("flora.legendary.dragons-beard"), TEXT("The Dragon's Beard"), EFloraVisualProductionKind::LegendaryPlant,
            TEXT("docs/design/fauna/plants_and_herbs.md Section 7.2"),
            TEXT("Legendary red-gold underwater algae forming flowing strands about 5-8 m long."),
            TEXT("Grows near volcanic vents, responds to heat and appears to move on its own in the water."),
            TEXT("Specific volcanic Ashenmoor coast, underwater near active thermal vents."),
            TEXT("Do not turn the algae into fire, a dragon organism or a magical tentacle creature. Heat response and water motion must remain botanical/algal."),
            true),
        Flora(
            TEXT("flora.legendary.midnight-rose"), TEXT("The Midnight Rose"), EFloraVisualProductionKind::LegendaryPlant,
            TEXT("docs/design/fauna/plants_and_herbs.md Section 7.3"),
            TEXT("Single legendary rose plant with pitch-black, thornless roses."),
            TEXT("Blooms only at midnight during one week of the year; petals are harvested gently without damaging the plant."),
            TEXT("A specific abandoned garden."),
            TEXT("Do not add glowing veins, thorns, supernatural smoke or a fixed garden architecture not present in the source. The impossible black bloom is sufficient."),
            true),
        Flora(
            TEXT("flora.legendary.memory-lichen"), TEXT("The Memory Lichen"), EFloraVisualProductionKind::LegendaryPlant,
            TEXT("docs/design/fauna/plants_and_herbs.md Section 7.4"),
            TEXT("Extensive ancient silver-white lichen that glows softly."),
            TEXT("Responds to Jake's presence, presents memories when touched, regrows quickly and seems welcoming."),
            TEXT("Region 06 only, appearing broadly through the post-game environment."),
            TEXT("Do not convert the soft glow into UI markers, holograms or explicit projected scenes in a static plant model. Memory presentation remains a separate runtime phenomenon."),
            true),
        Flora(
            TEXT("flora.region06.memory-oak"), TEXT("The Memory Oak"), EFloraVisualProductionKind::Region06ImpossibleTree,
            TEXT("docs/design/fauna/trees_and_fruits.md Section 7.1; docs/design/fauna/legendary_creatures.md Section 8.3"),
            TEXT("Ancient oak about 30 m tall with impossibly intricate bark patterns."),
            TEXT("Bark can show scenes from Jake's journey and the tree cannot be felled."),
            TEXT("Region 06."),
            TEXT("Provider reference may define ancient scale and intricate bark only. Do not hard-code one journey scene into the canonical bark texture; memory content is player-history/runtime dependent."),
            true),
        Flora(
            TEXT("flora.region06.whispering-grove"), TEXT("The Whispering Grove"), EFloraVisualProductionKind::Region06ImpossibleTree,
            TEXT("docs/design/fauna/trees_and_fruits.md Section 7.1; docs/design/fauna/legendary_creatures.md Section 8.3"),
            TEXT("Small grove of silver-barked trees with visibly trembling leaves."),
            TEXT("Leaves whisper in Jake's voice and the grove participates in philosophical encounters."),
            TEXT("Region 06."),
            TEXT("Do not put mouths/faces in trunks, visible dialogue text or audio-wave UI into the tree design. Voice behavior is separate from the silver-bark/trembling-leaf visual."),
            true),
        Flora(
            TEXT("flora.region06.infinite-branch"), TEXT("The Infinite Branch"), EFloraVisualProductionKind::Region06ImpossibleTree,
            TEXT("docs/design/fauna/trees_and_fruits.md Section 7.1; docs/design/fauna/legendary_creatures.md Section 8.3"),
            TEXT("Impossible tree whose apparent size changes between viewings and whose branches seem to extend into dimensions that cannot be resolved normally."),
            TEXT("Climbing reveals impossible sights. No stable final geometry is authored."),
            TEXT("Region 06."),
            TEXT("Reference work may explore impossible perspective only; it must not establish one canonical full silhouette, exact dimensions or a literal cosmology."),
            true),
        Flora(
            TEXT("flora.legendary.world-root"), TEXT("The World Root"), EFloraVisualProductionKind::LegendaryTree,
            TEXT("docs/design/fauna/trees_and_fruits.md Section 8.2; docs/design/fauna/legendary_creatures.md Section 8.1"),
            TEXT("Legendary oak approximately 60 m tall with a 15 m diameter trunk, forming the center of a sacred grove and reading as possibly thousands of years old."),
            TEXT("Its presence is felt before it is seen; animals are never harmed nearby; weather stays calm; time only seems slower. Fallen acorns/branches may be gathered, but the tree cannot be harvested directly."),
            TEXT("Hidden valley in Fjordlund interior and its sacred grove."),
            TEXT("Do not add a literal world-tree portal, glowing runes, faces, clocks or cosmic branches. The source locks monumental ancient oak scale, not explicit magic visuals."),
            true),
        Flora(
            TEXT("flora.story.ethans-grove"), TEXT("Ethan's Grove"), EFloraVisualProductionKind::StoryGrove,
            TEXT("docs/design/fauna/trees_and_fruits.md Section 8.3; Docs/DesignAuthority.md"),
            TEXT("Legacy story-grove record describes an orderly hidden grove of roughly 15-20 mixed plants/trees: Ethan's tobacco, hops bines, one mango tree, the mother's apple variety and a weeping willow."),
            TEXT("The grove appears tended and is framed as a quiet family-memory discovery with journals and family seeds."),
            TEXT("Legacy Chapter-9 hidden location."),
            TEXT("PROVIDER-BLOCKED: its discovery, Ethan framing and Chapter-9 placement intersect the unresolved Phase-11 Ethan/main-story authority. Do not generate it as current canon until that story authority is reconciled."),
            false)
    };
}

TArray<FFloraVisualProductionDesignGap> FFloraVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.flora.ethans-grove-phase11"),
            TEXT("Ethan's Grove is visually described but tied to a legacy Chapter-9 Ethan story beat. It remains provider-blocked until current Phase-11 story authority confirms, rewrites or removes that discovery."),
            TEXT("docs/design/fauna/trees_and_fruits.md Section 8.3; Docs/DesignAuthority.md")
        },
        {
            TEXT("design-gap.flora.legendary-master-count"),
            TEXT("The legendary master introduction summarizes three legendary trees, while its own cross-reference table and tree source expose World Root, Ethan's Grove, Memory Oak, Whispering Grove and Infinite Branch. Production follows individually grounded records and does not silently repair the summary count."),
            TEXT("docs/design/fauna/legendary_creatures.md Sections 1, 8 and 12")
        },
        {
            TEXT("design-gap.flora.phase4-scale-not-assets"),
            TEXT("Broad Phase-4 plant/tree target counts are not permission to invent visual species or variants. Additional flora enters production only from individually grounded source records."),
            TEXT("docs/design/fauna_overview.md Section 4")
        }
    };
}

bool FFloraVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FFloraVisualProductionBrief> Briefs = BuildBriefs();

    if (Briefs.Num() != RequiredBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Flora visual catalog requires %d entries; found %d."), RequiredBriefCount, Briefs.Num()));
    }

    int32 Ready = 0;
    int32 Blocked = 0;
    TSet<FName> Seen;
    for (const FFloraVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.StableId.IsNone()
            || Brief.DisplayName.IsEmpty()
            || Brief.GoverningSource.IsEmpty()
            || Brief.AuthoredVisualFacts.IsEmpty()
            || Brief.BehaviorOrPhenomenonRead.IsEmpty()
            || Brief.EnvironmentRead.IsEmpty()
            || Brief.AntiInventionBoundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Every flora visual brief requires identity, source, visual facts, phenomenon, environment and anti-invention boundary."));
        }

        if (Seen.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate flora visual id: %s"), *Brief.StableId.ToString()));
        }
        Seen.Add(Brief.StableId);

        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;

        if (Brief.bApprovedReferenceExists || !Brief.ApprovedReferencePath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Flora %s falsely claims an approved external reference."), *Brief.StableId.ToString()));
        }
    }

    if (Ready != ProviderReadyBriefCount || Blocked != ProviderBlockedBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Flora provider boundary requires %d ready and %d blocked briefs; found %d ready and %d blocked."),
            ProviderReadyBriefCount,
            ProviderBlockedBriefCount,
            Ready,
            Blocked));
    }

    const FFloraVisualProductionBrief* EthanGrove = Briefs.FindByPredicate([](const FFloraVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("flora.story.ethans-grove");
    });
    if (EthanGrove == nullptr || EthanGrove->bProviderReferenceReady || !EthanGrove->AntiInventionBoundary.Contains(TEXT("PROVIDER-BLOCKED")))
    {
        OutErrors.Add(TEXT("Ethan's Grove must remain provider-blocked until Phase-11 story authority is reconciled."));
    }

    const FFloraVisualProductionBrief* InfiniteBranch = Briefs.FindByPredicate([](const FFloraVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("flora.region06.infinite-branch");
    });
    if (InfiniteBranch == nullptr
        || !InfiniteBranch->bProviderReferenceReady
        || !InfiniteBranch->AntiInventionBoundary.Contains(TEXT("must not establish one canonical full silhouette")))
    {
        OutErrors.Add(TEXT("Infinite Branch reference work must preserve its deliberately unstable geometry."));
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Flora visual production must preserve Ethan-story, legendary-count and Phase-4-scale gaps."));
    }

    return OutErrors.IsEmpty();
}
