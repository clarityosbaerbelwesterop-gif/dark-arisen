// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/FaunaVisualProductionCatalog.h"

namespace
{
FFaunaVisualProductionBrief Fauna(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EFaunaVisualProductionKind Kind,
    const TCHAR* Source,
    const TCHAR* Visual,
    const TCHAR* Behavior,
    const TCHAR* Environment,
    const TCHAR* Boundary,
    const bool bProviderReady)
{
    FFaunaVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Kind = Kind;
    Result.GoverningSource = Source;
    Result.AuthoredVisualFacts = Visual;
    Result.BehaviorAndMotionRead = Behavior;
    Result.EnvironmentRead = Environment;
    Result.AntiInventionBoundary = Boundary;
    Result.bProviderReferenceReady = bProviderReady;
    return Result;
}
}

TArray<FFaunaVisualProductionBrief> FFaunaVisualProductionCatalog::BuildLegendaryCreatureBriefs()
{
    return {
        Fauna(
            TEXT("fauna.legendary.patriarch"), TEXT("The Patriarch"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 3.4; docs/design/fauna/legendary_creatures.md Section 3.1"),
            TEXT("Legendary male jaguar, about 220 cm and 130 kg; roughly forty percent larger than a normal jaguar; extensive scars across face and body; half of one ear missing; golden eyes."),
            TEXT("Old apex predator that stalks Jake rather than presenting as a trophy target. Movement remains natural jaguar motion: patient, low, silent and territorial."),
            TEXT("Deep Rexan jungle and a specific cave-network territory marked by abnormally high claw marks and larger tracks."),
            TEXT("Do not add fantasy mutations, armor, glowing markings or supernatural effects. Preserve a scarred natural jaguar at exceptional scale."),
            true),
        Fauna(
            TEXT("fauna.legendary.ghost-reindeer"), TEXT("The Ghost Reindeer"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 4.4; docs/design/fauna/legendary_creatures.md Section 3.2"),
            TEXT("Legendary caribou about 230 cm and 200 kg with an approximately 180 cm antler spread; pure white throughout, including antlers; eyes seem to glow in moonlight."),
            TEXT("Never flees or attacks; moves with deliberate awareness and makes eye contact. The encounter reads as quiet observation rather than monster behavior."),
            TEXT("Northern Fjordlund during Aurora Borealis conditions in a deep fjord."),
            TEXT("The source says the eyes seem to glow; do not convert that ambiguity into magical emissive anatomy or add spectral particles/body transparency."),
            true),
        Fauna(
            TEXT("fauna.legendary.ash-shadow"), TEXT("The Ash-Shadow"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 5.4; docs/design/fauna/legendary_creatures.md Section 3.3"),
            TEXT("Legendary wolf about 170 cm and 80 kg; pure black coat unlike the surrounding ash-colored wolves; yellow eyes that catch firelight."),
            TEXT("Stalks Jake silently and appears when he is alone. Motion should remain recognizably wolf-like and predatory, with silence as an authored behavior rather than a ghost effect."),
            TEXT("Ashenmoor volcanic terrain around dormant vents, including a hidden valley and full-moon night conditions."),
            TEXT("Do not make the animal incorporeal, smoky, demonic or magically shadow-built; pure-black natural anatomy is the locked visual fact."),
            true),
        Fauna(
            TEXT("fauna.legendary.emperors-stag"), TEXT("The Emperor's Stag"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 6.4; docs/design/fauna/legendary_creatures.md Section 3.4"),
            TEXT("Legendary red deer about 250 cm and 280 kg; massive white-and-chestnut stag; fourteen-point antlers spanning about 180 cm; visible scars from fighting challengers; noble bearing."),
            TEXT("Old dominant stag leading a large harem; appears in specific dawn clearings and behaves as protected wild game, not a fantasy king."),
            TEXT("Royal Forest of Pale Isle inside managed imperial hunting grounds."),
            TEXT("Do not add crowns, heraldic jewelry, human ornament or supernatural forest effects. Its political significance comes from protection law and scale."),
            true),
        Fauna(
            TEXT("fauna.legendary.old-iron-back"), TEXT("Old Iron-Back"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 7.4; docs/design/fauna/legendary_creatures.md Section 3.5"),
            TEXT("Legendary black bear genetic anomaly, about 280 cm and 450 kg; grizzly-sized; unique iron-gray fur; extensive scarring; one cloudy eye; yellow teeth."),
            TEXT("Very old dangerous bear that knows its forest and uses terrain; heavy natural bear motion, not boss posing."),
            TEXT("Deep Quiet Coast forest around a long-used cave and ravines associated with previous attacks."),
            TEXT("Do not add literal iron plating, metallic skin, armor or fantasy bear anatomy; Iron-Back refers to the unique fur/read."),
            true),
        Fauna(
            TEXT("fauna.legendary.memory-stag"), TEXT("The Memory Stag"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Impossible Region-06 stag about 280 cm and 350 kg; pure white with iridescent sheen; antlers change when not directly observed; its face can momentarily read as Jake's father."),
            TEXT("Never attacks or flees; eye contact triggers memory flashes. Provider reference may show only the stable white/iridescent stag read plus ambiguity, not choose a final antler state or paternal face."),
            TEXT("Region 06 only, when Jake is alone."),
            TEXT("Do not freeze the shifting antlers into a canonical configuration or define Jake's father's face from this animal brief. No fixed transformation sequence is authored."),
            true),
        Fauna(
            TEXT("fauna.legendary.threshold-fox"), TEXT("The Threshold Fox"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 9.2; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Impossible fox spirit about 80 cm and 5 kg; silver-white coat with red markings; multiple tails whose count varies between viewings."),
            TEXT("Non-hostile guide appearing at decision points. Motion should read alert and intentional without locking a single tail count."),
            TEXT("Region 06 paths and thresholds only."),
            TEXT("Do not choose a permanent tail count, add unlisted symbols, clothing or humanoid facial traits. A provider image is a non-canon reference for the stable silver-white/red-marking language only."),
            true),
        Fauna(
            TEXT("fauna.legendary.final-wolf"), TEXT("The Final Wolf"), EFaunaVisualProductionKind::LegendaryLandAnimal,
            TEXT("docs/design/fauna/land_animals.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Region-06 guardian about 200 cm and 100 kg whose appearance shifts between wolves the individual player has killed, including their faces."),
            TEXT("It confronts Jake about the player's own hunting history and must be addressed rather than fought."),
            TEXT("Region 06 only; presentation is save-history dependent."),
            TEXT("PROVIDER-BLOCKED: no single canonical face/coat exists. A static provider reference would invent a player-history-dependent identity and collapse the authored variability."),
            false),
        Fauna(
            TEXT("fauna.legendary.scarred-leviathan"), TEXT("The Scarred Leviathan"), EFaunaVisualProductionKind::LegendarySeaCreature,
            TEXT("docs/design/fauna/sea_animals.md Section 5.3; docs/design/fauna/legendary_creatures.md Section 4.1"),
            TEXT("Ancient male sperm whale about 24 m and 60 tonnes; body covered in old harpoon scars; healed wounds leave white patches; one eye is cloudy and blind."),
            TEXT("Active intelligence and learned response to whaling vessels. Peace-route presentation is non-hostile; hunt presentation remains natural whale mass and motion."),
            TEXT("Deep ocean between Fjordlund and Pale Isle."),
            TEXT("Do not exaggerate into a sea monster, add teeth/tentacles or fantasy bioluminescence. Historical harpoon damage and age are the distinctive read."),
            true),
        Fauna(
            TEXT("fauna.legendary.bone-ghost"), TEXT("The Bone Ghost"), EFaunaVisualProductionKind::LegendarySeaCreature,
            TEXT("docs/design/fauna/sea_animals.md Section 7.2; docs/design/fauna/legendary_creatures.md Section 4.2"),
            TEXT("Legendary great white shark about 8 m and 2,800 kg; unusually pale/white; numerous scars; multiple missing/replaced teeth; cold milky eyes."),
            TEXT("Large intelligent shark that can attack ships. Movement remains physical great-white predation, not spectral locomotion."),
            TEXT("Deep waters off the Quiet Coast."),
            TEXT("Despite the name Ghost, do not make it translucent, undead or supernatural. Its pale coloration, scars and scale are the authored identity."),
            true),
        Fauna(
            TEXT("fauna.legendary.kraken"), TEXT("The Kraken"), EFaunaVisualProductionKind::LegendarySeaCreature,
            TEXT("docs/design/fauna/sea_animals.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 4.3"),
            TEXT("Ancient giant squid/octopus-like creature with 30-50 m arm span; massive dark purple-black body; eyes larger than dinner plates; tentacles dwarf ships."),
            TEXT("Rises from deep water and can wrap a ship; not hostile unless disturbed. Scale and ship interaction are more important than fantasy ornament."),
            TEXT("Deep ocean trenches and rare open-sea encounter zones."),
            TEXT("Exact mass, complete taxonomy and detailed full-body anatomy beyond giant cephalopod language are not locked. Do not add humanoid features, armor or magical runes."),
            true),
        Fauna(
            TEXT("fauna.legendary.deep-one"), TEXT("The Deep One"), EFaunaVisualProductionKind::LegendarySeaCreature,
            TEXT("docs/design/fauna/sea_animals.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 4.3"),
            TEXT("Never fully seen. Source permits only glimpses: a massive tentacle, a single eye and an impossibly large shadow. Total size and complete anatomy are unknown."),
            TEXT("Watches rather than attacks directly. Any provider reference must be a glimpse/composition reference that protects the absence of a full reveal."),
            TEXT("Deepest ocean trenches during specific late-game moments."),
            TEXT("Do not render a complete creature, assign a final size, count limbs or define a face/body plan. The unseen body is a locked visual rule."),
            true),
        Fauna(
            TEXT("fauna.legendary.emperor-of-the-wind"), TEXT("The Emperor of the Wind"), EFaunaVisualProductionKind::LegendaryBird,
            TEXT("docs/design/fauna/birds.md Section 10.1; docs/design/fauna/legendary_creatures.md Section 5.1"),
            TEXT("Legendary golden eagle about 100 cm body length and 2.5 m wingspan; unusually large; golden plumage crossed by unusual white feathers; one leg carries an ancient injury that never healed."),
            TEXT("Old apex aerial predator. Flight should remain eagle-realistic and powerful rather than elemental or magical."),
            TEXT("Highest mountains of Pale Isle and inaccessible cliff nests."),
            TEXT("Do not add crowns, armor, lightning, heraldic devices or supernatural wing effects."),
            true),
        Fauna(
            TEXT("fauna.legendary.old-messenger"), TEXT("The Old Messenger"), EFaunaVisualProductionKind::LegendaryBird,
            TEXT("docs/design/fauna/birds.md Section 10.2; docs/design/fauna/legendary_creatures.md Section 5.2"),
            TEXT("Normal-sized black raven distinguished by a white chest spot and one milky-blue eye; apparent age is unknown."),
            TEXT("Quiet observer that repeatedly appears near important deaths and cannot be caught. No overt supernatural movement is authored."),
            TEXT("Cross-regional; follows Jake and perches near story moments."),
            TEXT("The source deliberately leaves whether it is one raven, many ravens or supernatural unresolved. Do not visually answer that question."),
            true),
        Fauna(
            TEXT("fauna.legendary.silver-albatross"), TEXT("The Silver Albatross"), EFaunaVisualProductionKind::LegendaryBird,
            TEXT("docs/design/fauna/birds.md Section 10.3; docs/design/fauna/legendary_creatures.md Section 5.3"),
            TEXT("Legendary wandering albatross about 130 cm body length with a 3.5 m wingspan; pure silver-white coloration."),
            TEXT("Follows La Liberacion on long voyages, in calm and storm, and never lands on the ship. It is a cultural omen, not a magical effect show."),
            TEXT("Open ocean around long-voyage routes."),
            TEXT("Do not add metallic feathers, halos, particles or literal luck effects to its body. Silver-white is coloration, not metal."),
            true),
        Fauna(
            TEXT("fauna.legendary.jungle-phoenix"), TEXT("The Jungle Phoenix"), EFaunaVisualProductionKind::LegendaryBird,
            TEXT("docs/design/fauna/birds.md Section 10.4; docs/design/fauna/legendary_creatures.md Section 5.4"),
            TEXT("Possibly real, possibly mythic Rexan bird; accounts vary from 70-150 cm; brilliant orange-red plumage, golden crest, and an appearance that seems to glow at dusk."),
            TEXT("Rarely seen at specific natural shrines and associated with renewal. Reference art must preserve the possibility that reports are folklore rather than confirming supernatural rebirth."),
            TEXT("Deepest Rexan jungles around natural shrines."),
            TEXT("Do not depict literal fire, resurrection, magical aura or a fixed exact scale. Provider output is folklore/concept reference, not proof the creature exists."),
            true),
        Fauna(
            TEXT("fauna.legendary.blood-moth"), TEXT("The Blood Moth"), EFaunaVisualProductionKind::LegendarySmallCreature,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 6.1"),
            TEXT("Legendary moth with approximately 30 cm wingspan; deep blood-red wings; white body markings that appear to shift when viewed."),
            TEXT("Singular, calm and active only under specific moon phases. It must be captured alive; death/dissolution is not a visual license for gore or magic effects."),
            TEXT("A specific grove in deep Rexan jungle associated with rare flowers and unusual quiet."),
            TEXT("Do not freeze the shifting white markings into a definitive canonical pattern or add extra eyes/limbs. The moving-mark ambiguity must remain."),
            true),
        Fauna(
            TEXT("fauna.legendary.ancient-tortoise"), TEXT("The Ancient Tortoise"), EFaunaVisualProductionKind::LegendarySmallCreature,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.2; docs/design/fauna/legendary_creatures.md Section 6.2"),
            TEXT("Legendary tortoise with about 180 cm shell length and 300+ kg mass; massive old body; shell naturally etched with patterns resembling maps."),
            TEXT("Extraordinarily slow, unafraid and entirely peaceful. The visual should communicate age and endurance rather than boss scale."),
            TEXT("Remote Rexan islands."),
            TEXT("Do not turn shell patterns into an explicit modern map/UI overlay or invent exact treasure coordinates. They only resemble maps."),
            true),
        Fauna(
            TEXT("fauna.legendary.phoenix-spider"), TEXT("The Phoenix Spider"), EFaunaVisualProductionKind::LegendarySmallCreature,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 6.3"),
            TEXT("Legendary arachnid about 35 cm legspan; fire-red and gold coloration; seems to flicker with inner light."),
            TEXT("Territorial but not inherently aggressive; observation, feeding or combat are all authored approaches."),
            TEXT("Volcanic caves of Ashenmoor under strong environmental heat."),
            TEXT("The source says seems to flicker; do not make it literally burning, elemental or immortal. Preserve normal arachnid anatomy unless separately authored."),
            true)
    };
}

TArray<FFaunaVisualProductionBrief> FFaunaVisualProductionCatalog::BuildHighmooreMvpBriefs()
{
    return {
        Fauna(
            TEXT("fauna.highmoore.grouse"), TEXT("Highmoore Grouse"), EFaunaVisualProductionKind::HighmooreMvpAnimal,
            TEXT("highmoore fauna.md Sections 1, 4.1 and 9.1"),
            TEXT("The grouse is Highmoore's signature high-moor bird, but the current higher-authority source does not lock an exact grouse species, plumage variant or body sheet."),
            TEXT("Ground-dwelling flush behavior is critical; its sudden flush is the loudest authored surprise sound on the High Moor."),
            TEXT("High Moor heather/gorse/bracken/grass ecology."),
            TEXT("PROVIDER-BLOCKED: do not choose a grouse species or plumage design on behalf of design authority."),
            false),
        Fauna(
            TEXT("fauna.highmoore.hare"), TEXT("Highmoore Hare"), EFaunaVisualProductionKind::HighmooreMvpAnimal,
            TEXT("highmoore fauna.md Sections 4.2, 6.2 and 9.1"),
            TEXT("An ordinary moorland hare is required, but the current Highmoore source does not lock an exact species, seasonal coat or unique visual variant."),
            TEXT("Extremely fast: uncatchable on foot and barely catchable mounted; ordinary rather than mystical."),
            TEXT("High Moor and Folds; also leaves a stopped shaft without explanation."),
            TEXT("PROVIDER-BLOCKED: do not choose species, coat coloration or a stylized magical variant."),
            false),
        Fauna(
            TEXT("fauna.highmoore.red-deer"), TEXT("Highmoore Red Deer"), EFaunaVisualProductionKind::HighmooreMvpAnimal,
            TEXT("highmoore fauna.md Section 4.3; fauna/land_animals.md red-deer family reference"),
            TEXT("Red deer is explicitly the largest Highmoore game and Great Hunt quarry. Use natural red-deer anatomy and a restrained moorland population read; no unique legendary variant is authored."),
            TEXT("Wild herd/game behavior; presentation serves House hunting culture rather than boss-like staging."),
            TEXT("The Folds and especially the Burnt March."),
            TEXT("No house heraldry, trophy staging, impossible antlers or fixed individual identity. This is a species/ecology reference only."),
            true),
        Fauna(
            TEXT("fauna.highmoore.fox"), TEXT("Highmoore Fox"), EFaunaVisualProductionKind::HighmooreMvpAnimal,
            TEXT("highmoore fauna.md Section 5.1; highmoore settlements.md"),
            TEXT("Ordinary agricultural fox is required throughout Highmoore, but the source does not lock an exact fox species or coat variant."),
            TEXT("Common nuisance/scavenger behavior discussed by yeomanry; no legendary behavior."),
            TEXT("Across Highmoore, especially settled/farm edges."),
            TEXT("PROVIDER-BLOCKED: do not choose a species/coat or make the fox a House mascot."),
            false),
        Fauna(
            TEXT("fauna.highmoore.fell-wolf"), TEXT("The Fell Wolf"), EFaunaVisualProductionKind::HighmooreMvpAnimal,
            TEXT("highmoore fauna.md Section 5.2; mounted travel.md Section 5.3"),
            TEXT("Rare high-country wolf and explicitly not a boss or legendary animal. Current source does not lock coat, exact size, species variant or scar pattern."),
            TEXT("Genuinely dangerous to a man alone on foot; horses bolt from it at low bond before riders usually see it."),
            TEXT("Stonecountry and high fells."),
            TEXT("PROVIDER-BLOCKED: do not invent a special fantasy wolf silhouette simply because it is called Fell Wolf."),
            false)
    };
}

TArray<FFaunaVisualProductionBrief> FFaunaVisualProductionCatalog::BuildAllBriefs()
{
    TArray<FFaunaVisualProductionBrief> Result = BuildLegendaryCreatureBriefs();
    Result.Append(BuildHighmooreMvpBriefs());
    return Result;
}

TArray<FFaunaVisualProductionDesignGap> FFaunaVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.fauna.highmoore-species-variants"),
            TEXT("Highmoore requires grouse, hare, fox and Fell Wolf for MVP, but the higher-authority source does not lock exact species/coat/silhouette variants. Only red deer is sufficiently bounded for a generic species-level provider reference."),
            TEXT("highmoore fauna.md")
        },
        {
            TEXT("design-gap.fauna.final-wolf-player-history"),
            TEXT("The Final Wolf has no single static appearance: its face shifts through wolves killed in the individual save. Provider generation is blocked until a player-history-driven runtime representation can preserve that rule."),
            TEXT("docs/design/fauna/land_animals.md Section 9.3")
        },
        {
            TEXT("design-gap.fauna.phase4-scale-not-authored-assets"),
            TEXT("Phase 4 target counts such as 40+ mammals, 60+ fish and ~280 total species are scale targets, not permission to synthesize unnamed or insufficiently specified visual variants. Production must advance through individually grounded species records."),
            TEXT("docs/design/fauna_overview.md Section 4")
        }
    };
}

bool FFaunaVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FFaunaVisualProductionBrief> Legendary = BuildLegendaryCreatureBriefs();
    const TArray<FFaunaVisualProductionBrief> Highmoore = BuildHighmooreMvpBriefs();
    const TArray<FFaunaVisualProductionBrief> All = BuildAllBriefs();

    if (Legendary.Num() != LegendaryCreatureBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Legendary fauna visual catalog requires %d entries; found %d."), LegendaryCreatureBriefCount, Legendary.Num()));
    }
    if (Highmoore.Num() != HighmooreMvpBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Highmoore fauna MVP requires %d entries; found %d."), HighmooreMvpBriefCount, Highmoore.Num()));
    }
    if (All.Num() != RequiredBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Fauna visual catalog requires %d entries total; found %d."), RequiredBriefCount, All.Num()));
    }

    int32 Ready = 0;
    int32 Blocked = 0;
    int32 LegendaryReady = 0;
    int32 HighmooreReady = 0;
    TSet<FName> Seen;

    for (const FFaunaVisualProductionBrief& Brief : All)
    {
        if (Brief.StableId.IsNone()
            || Brief.DisplayName.IsEmpty()
            || Brief.GoverningSource.IsEmpty()
            || Brief.AuthoredVisualFacts.IsEmpty()
            || Brief.BehaviorAndMotionRead.IsEmpty()
            || Brief.EnvironmentRead.IsEmpty()
            || Brief.AntiInventionBoundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Every fauna visual brief requires identity, source, visual facts, behavior, environment and anti-invention boundary."));
        }

        if (Seen.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate fauna visual id: %s"), *Brief.StableId.ToString()));
        }
        Seen.Add(Brief.StableId);

        if (Brief.bProviderReferenceReady)
        {
            ++Ready;
            if (Brief.Kind == EFaunaVisualProductionKind::HighmooreMvpAnimal) ++HighmooreReady;
            else ++LegendaryReady;
        }
        else
        {
            ++Blocked;
        }

        if (Brief.bApprovedReferenceExists || !Brief.ApprovedReferencePath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Fauna %s falsely claims an approved external reference."), *Brief.StableId.ToString()));
        }
    }

    if (Ready != ProviderReadyBriefCount || Blocked != ProviderBlockedBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Fauna provider boundary requires %d ready and %d blocked briefs; found %d ready and %d blocked."),
            ProviderReadyBriefCount,
            ProviderBlockedBriefCount,
            Ready,
            Blocked));
    }
    if (LegendaryReady != ProviderReadyLegendaryCount || HighmooreReady != ProviderReadyHighmooreMvpCount)
    {
        OutErrors.Add(TEXT("Fauna provider-ready split drifted from legendary/Highmoore authority."));
    }

    const FFaunaVisualProductionBrief* FinalWolf = All.FindByPredicate([](const FFaunaVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("fauna.legendary.final-wolf");
    });
    if (FinalWolf == nullptr || FinalWolf->bProviderReferenceReady || !FinalWolf->AntiInventionBoundary.Contains(TEXT("PROVIDER-BLOCKED")))
    {
        OutErrors.Add(TEXT("The Final Wolf must remain provider-blocked because its appearance depends on player hunting history."));
    }

    const FFaunaVisualProductionBrief* DeepOne = All.FindByPredicate([](const FFaunaVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("fauna.legendary.deep-one");
    });
    if (DeepOne == nullptr || !DeepOne->bProviderReferenceReady || !DeepOne->AntiInventionBoundary.Contains(TEXT("Do not render a complete creature")))
    {
        OutErrors.Add(TEXT("The Deep One may only be provider-ready as a bounded glimpse reference that never defines the full creature."));
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Fauna visual production must preserve Highmoore-variant, Final-Wolf and Phase-4-scale design gaps."));
    }

    return OutErrors.IsEmpty();
}
