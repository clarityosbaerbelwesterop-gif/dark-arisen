// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/CreatureVisualProductionCatalog.h"

namespace
{
FCreatureVisualProductionBrief Creature(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const ECreatureVisualFamily Family,
    const TCHAR* Source,
    const TCHAR* VisualFacts,
    const TCHAR* BehaviorRead,
    const TCHAR* Boundary,
    const bool bReady,
    const bool bCombat,
    const bool bNonCombat,
    const FName Dependency = NAME_None)
{
    FCreatureVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Family = Family;
    Result.GoverningSource = Source;
    Result.VisualFacts = VisualFacts;
    Result.BehaviorRead = BehaviorRead;
    Result.AntiInventionBoundary = Boundary;
    Result.ExistingVisualDependency = Dependency;
    Result.bProviderReferenceReady = bReady;
    Result.bCombatCreature = bCombat;
    Result.bMustRemainNonCombat = bNonCombat;
    return Result;
}
}

TArray<FCreatureVisualProductionBrief> FCreatureVisualProductionCatalog::BuildBriefs()
{
    return {
        // Land legendary creatures.
        Creature(
            TEXT("creature-visual.patriarch"), TEXT("The Patriarch"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 3.4; docs/design/fauna/legendary_creatures.md Section 3.1"),
            TEXT("Legendary male jaguar, roughly 220 cm and 130 kg; about forty percent larger than normal, extensive facial/body scarring, one ear half missing and golden eyes."),
            TEXT("A solitary old apex predator that reverse-hunts Jake and studies him before the final confrontation."),
            TEXT("Keep a natural jaguar anatomy and old-predator read. Do not add supernatural glow, armor, collars, tribal decoration or humanlike expression."),
            true, true, false),
        Creature(
            TEXT("creature-visual.ghost-reindeer"), TEXT("Ghost Reindeer"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 4.4; docs/design/fauna/legendary_creatures.md Section 3.2"),
            TEXT("Legendary caribou about 230 cm and 200 kg with roughly 180 cm antler spread; pure white body and antlers, eyes that seem to glow in moonlight, ethereal presence."),
            TEXT("Appears only under the Northern Lights; never flees or attacks and makes deliberate eye contact."),
            TEXT("The source permits an ethereal read but does not define literal magic particles, transparency, runes or ornaments. Preserve the impossible white natural animal rather than inventing fantasy armor."),
            true, false, false),
        Creature(
            TEXT("creature-visual.ash-shadow"), TEXT("The Ash-Shadow"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 5.4; docs/design/fauna/legendary_creatures.md Section 3.3"),
            TEXT("Extremely large wolf around 170 cm and 80 kg; pure black among ash-gray packs, yellow eyes catching firelight, visually silent and clean against volcanic terrain."),
            TEXT("Stalks Jake alone around dormant vents and behaves as the reverse predator rather than a frontal monster."),
            TEXT("No smoke body, demon horns, lava cracks or magical VFX are authored. It remains a physically wolf-shaped animal whose uncanny quality comes from color, scale and silence."),
            true, true, false),
        Creature(
            TEXT("creature-visual.emperors-stag"), TEXT("The Emperor's Stag"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 6.4; docs/design/fauna/legendary_creatures.md Section 3.4"),
            TEXT("Legendary red deer around 250 cm and 280 kg; massive white-and-chestnut body, fourteen-point antlers spanning about 180 cm, scars from challenger fights and a noble old-stag bearing."),
            TEXT("Protected royal-forest animal leading more than thirty does and appearing in dawn clearings."),
            TEXT("Do not add crowns, imperial cloth, jewelry or heraldry. The class symbolism comes from legal ownership and the animal's natural magnificence."),
            true, false, false),
        Creature(
            TEXT("creature-visual.old-iron-back"), TEXT("Old Iron-Back"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 7.4; docs/design/fauna/legendary_creatures.md Section 3.5"),
            TEXT("Genetically anomalous black bear roughly 280 cm and 450 kg; iron-gray fur, extensive scarring, one cloudy eye and old yellow teeth."),
            TEXT("Thirty-plus-year forest survivor that knows terrain, has killed seven hunters and attacks when disturbed."),
            TEXT("Keep black-bear anatomy despite grizzly-scale mass. No metal plating, literal iron hide or fantasy equipment."),
            true, true, false),
        Creature(
            TEXT("creature-visual.memory-stag"), TEXT("The Memory Stag"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Impossible stag about 280 cm and 350 kg, pure white with iridescent sheen; antlers visibly shift when not looked at directly and its face may momentarily read as Jake's father."),
            TEXT("Region 06 solitary encounter; never flees or attacks, with eye contact triggering memories."),
            TEXT("Do not lock one permanent antler topology or literal human-faced deer model. The authored visual law is perceptual instability; reference work must show a neutral base plus controlled variants."),
            true, false, true),
        Creature(
            TEXT("creature-visual.threshold-fox"), TEXT("The Threshold Fox"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 9.2; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Silver-white fox around 80 cm and 5 kg with red markings and multiple tails whose count varies between viewings."),
            TEXT("Region 06 guide appearing at decision points; cannot be harmed and behaves as a deliberate philosophical guide."),
            TEXT("Tail count is intentionally unstable. Do not canonize a fixed kitsune count, clothing, jewelry, speech mouth shapes or additional magical markings."),
            true, false, true),
        Creature(
            TEXT("creature-visual.final-wolf"), TEXT("The Final Wolf"), ECreatureVisualFamily::LandLegendary,
            TEXT("docs/design/fauna/land_animals.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 3.6"),
            TEXT("Region 06 guardian wolf around 200 cm and 100 kg whose appearance shifts between wolves Jake has killed."),
            TEXT("Confronts the player with hunting consequences and must be addressed rather than fought."),
            TEXT("No single permanent coat/face is canon. Create a transformation/reference grammar only from approved ordinary wolf looks; never turn it into a combat monster."),
            true, false, true),

        // Sea legendary and post-game creatures. Boss-owned Havfrue is deliberately not duplicated.
        Creature(
            TEXT("creature-visual.scarred-leviathan"), TEXT("The Scarred Leviathan"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 5.3; docs/design/fauna/legendary_creatures.md Section 4.1"),
            TEXT("Record-breaking male sperm whale about 24 m and 60 tonnes, covered in old harpoon scars, healed white patches and one cloudy blind eye."),
            TEXT("Ancient intelligent survivor of forty-plus whaling attempts that recognizes harpoons and may seek whaling vessels."),
            TEXT("Natural sperm-whale anatomy only. Do not add fantasy armor, glowing wounds or embedded weapons beyond historically grounded old scar evidence."),
            true, true, false),
        Creature(
            TEXT("creature-visual.bone-ghost"), TEXT("The Bone Ghost"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 7.2; docs/design/fauna/legendary_creatures.md Section 4.2"),
            TEXT("Abnormally large great white shark around 8 m and 2,800 kg; unusually pale white hide, numerous scars, visibly replaced/missing teeth and cold milky eyes."),
            TEXT("Deep Quiet Coast hunter known to attack ships and behave with unusual tactical intelligence."),
            TEXT("Despite the name it is not a ghost. No transparency, skeletal body, undead wounds or paranormal glow."),
            true, true, false),
        Creature(
            TEXT("creature-visual.kraken"), TEXT("The Kraken"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 4.3"),
            TEXT("Ancient giant squid/octopus-like creature with 30-50 m arm span, dark purple-black body, eyes larger than dinner plates and tentacles large enough to dwarf a ship."),
            TEXT("Deep-ocean creature that can wrap a ship; not hostile unless disturbed and demonstrates ancient intelligence."),
            TEXT("The source does not lock exact cephalopod species anatomy, sucker pattern, beak visibility or full mass. Preserve the written scale/color/eye/tentacle read without adding eldritch jewelry or humanoid features."),
            true, true, false),
        Creature(
            TEXT("creature-visual.deep-one"), TEXT("The Deep One"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 4.3"),
            TEXT("Never fully seen: only an impossibly large shadow, a massive tentacle and a single eye are authored visual glimpses."),
            TEXT("Deepest-ocean entity that watches rather than directly attacks; Chapter 9-10 philosophical presence."),
            TEXT("Absolutely do not design a complete canonical body. Provider reference may explore only the three authored glimpses: shadow, one tentacle and one eye."),
            true, false, true),
        Creature(
            TEXT("creature-visual.memory-fish"), TEXT("The Memory Fish"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 10.1"),
            TEXT("Intentionally variable Region 06 fish: each specimen looks like a fish Jake once caught; size and appearance change."),
            TEXT("Fishing encounter used to return memories rather than establish a new stable species silhouette."),
            TEXT("DESIGN-GAP: there is no single canonical visual identity to generate. Its appearance depends on the player's prior catch history."),
            false, false, true),
        Creature(
            TEXT("creature-visual.guide-whale"), TEXT("The Guide Whale"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 10.2"),
            TEXT("Normal humpback-whale silhouette with only a subtle glow distinguishing the post-game guide."),
            TEXT("Non-hostile Region 06 whale that leads Jake to locations and sings authored melodies."),
            TEXT("Keep it anatomically a normal humpback. Glow must remain subtle; do not invent runes, giant size changes or armor."),
            true, false, true),
        Creature(
            TEXT("creature-visual.sea-mothers-shadow"), TEXT("The Sea Mother's Shadow"), ECreatureVisualFamily::SeaLegendary,
            TEXT("docs/design/fauna/sea_animals.md Section 10.3; docs/design/bosses/havfrue_modor.md"),
            TEXT("Whale-sized dark underwater shadow explicitly reminiscent of Havfrue Modor rather than a fully surfaced new creature."),
            TEXT("Peace-route post-game echo that watches Jake and functions as closure rather than combat."),
            TEXT("Must inherit its read from the approved Havfrue visual authority. Do not create a second Sea-Mother species or expose a new full body."),
            true, false, true, TEXT("boss-visual.havfrue-modor")),

        // Bird legendaries.
        Creature(
            TEXT("creature-visual.emperor-of-wind"), TEXT("The Emperor of the Wind"), ECreatureVisualFamily::BirdLegendary,
            TEXT("docs/design/fauna/birds.md Section 10.1; docs/design/fauna/legendary_creatures.md Section 5.1"),
            TEXT("Abnormally large golden eagle, about 100 cm body and 2.5 m wingspan, unusual white feather streaks and one visibly old injured leg."),
            TEXT("Forty-plus-year apex aerial predator of Pale Isle's highest mountains."),
            TEXT("No imperial crown, harness or heraldry. It is a natural old eagle made legendary through age, size and history."),
            true, true, false),
        Creature(
            TEXT("creature-visual.old-messenger"), TEXT("The Old Messenger"), ECreatureVisualFamily::BirdLegendary,
            TEXT("docs/design/fauna/birds.md Section 10.2; docs/design/fauna/legendary_creatures.md Section 5.2"),
            TEXT("Otherwise normal black raven with one white chest spot and one milky-blue eye."),
            TEXT("Ancient-seeming witness that follows Jake at story deaths; cannot be caught or killed."),
            TEXT("Its ambiguity is essential. Do not enlarge it, make it spectral, add symbols or visually answer whether it is supernatural."),
            true, false, true),
        Creature(
            TEXT("creature-visual.silver-albatross"), TEXT("The Silver Albatross"), ECreatureVisualFamily::BirdLegendary,
            TEXT("docs/design/fauna/birds.md Section 10.3; docs/design/fauna/legendary_creatures.md Section 5.3"),
            TEXT("Massive wandering albatross around 130 cm body and 3.5 m wingspan with pure silver-white coloration."),
            TEXT("Open-ocean omen following La Liberacion across long voyages and storms without landing aboard."),
            TEXT("Preserve real albatross anatomy. The silver-white coloration is the authored anomaly; no metallic feathers, glow or decorative bands."),
            true, false, false),
        Creature(
            TEXT("creature-visual.jungle-phoenix"), TEXT("The Jungle Phoenix"), ECreatureVisualFamily::BirdLegendary,
            TEXT("docs/design/fauna/birds.md Section 10.4; docs/design/fauna/legendary_creatures.md Section 5.4"),
            TEXT("Possibly-real Rexan bird with accounts varying from 70-150 cm, brilliant orange-red plumage, a golden crest and a dusk glow-like impression."),
            TEXT("Extremely rare shrine-associated symbol of renewal; whether it is literally supernatural remains unanswered."),
            TEXT("Do not resolve the myth. Reference art may show plausible natural and slightly uncanny reads, but no literal fire body, rebirth scene or fixed supernatural anatomy is canon."),
            true, false, false),

        // Small creature legendaries.
        Creature(
            TEXT("creature-visual.blood-moth"), TEXT("The Blood Moth"), ECreatureVisualFamily::SmallLegendary,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.1; docs/design/fauna/legendary_creatures.md Section 6.1"),
            TEXT("Huge moth with roughly 30 cm wingspan, deep blood-red wings and white body markings that shift while viewed."),
            TEXT("Singular moon-phase Rexan encounter; supernatural calm surrounds it and it must be captured alive."),
            TEXT("Marking movement is authored but exact fixed pattern is not. Do not freeze one canonical pattern or turn it into a hostile monster."),
            true, false, true),
        Creature(
            TEXT("creature-visual.ancient-tortoise"), TEXT("The Ancient Tortoise"), ECreatureVisualFamily::SmallLegendary,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.2; docs/design/fauna/legendary_creatures.md Section 6.2"),
            TEXT("Massive tortoise with about 180 cm shell length and 300+ kg body; shell bears natural patterns that resemble maps."),
            TEXT("Two-hundred-plus-year peaceful island presence, unafraid and mechanically impossible to kill."),
            TEXT("Map-like shell patterns must remain natural-looking rather than engraved cartography. No combat pose, armor or saddle."),
            true, false, true),
        Creature(
            TEXT("creature-visual.phoenix-spider"), TEXT("The Phoenix Spider"), ECreatureVisualFamily::SmallLegendary,
            TEXT("docs/design/fauna/insects_and_small_creatures.md Section 9.3; docs/design/fauna/legendary_creatures.md Section 6.3"),
            TEXT("Large volcanic spider with roughly 35 cm leg span, fire-red and gold coloration and an impression of inner flickering light."),
            TEXT("Territorial but not inherently aggressive; can be killed, observed or befriended through a specific feeding approach."),
            TEXT("Do not add literal flames, humanoid face, phoenix wings or magma anatomy. Its visual identity is a spider with authored coloration and subtle inner-light read."),
            true, true, false),

        // Highmoore MVP ecology and hound visual families.
        Creature(
            TEXT("creature-visual.highmoore.moor-hound"), TEXT("Highmoore Moor Hound"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Section 3.2"),
            TEXT("Long-legged sight-hunting hound bred for the open moor; built to run open ground better than a horse over the relevant terrain."),
            TEXT("House hunting dog, athletic and functional rather than pet-show ornamental."),
            TEXT("Exact coat palette, head shape and House-specific livery are not locked. Keep a plausible long-legged working sighthound silhouette without inventing heraldic markings."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.fell-terrier"), TEXT("Highmoore Fell Terrier"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Section 3.2"),
            TEXT("Small fell-working terrier; compact, practical and explicitly described as appalling but universally beloved."),
            TEXT("Highmoore working/hunting companion with comic personality emerging from behavior rather than costume."),
            TEXT("Exact breed standard and coat are not locked. Do not invent House uniforms, fantasy proportions or a mascot costume."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.house-hound"), TEXT("Highmoore House Hound"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Section 3.2; the_lesser_houses.md Section 3.6"),
            TEXT("House-owned hunting hound family; Carrick's are described as prettier and Belos's as better, but no final breed silhouette is authored."),
            TEXT("Political/cultural hunting animal rather than one universal breed."),
            TEXT("DESIGN-GAP: provider output would force an unauthored breed standard, coat and House differentiation. Keep blocked until House-specific visual facts are written."),
            false, false, false),
        Creature(
            TEXT("creature-visual.highmoore.grouse"), TEXT("Highmoore Grouse"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Sections 4.1 and 9.1; docs/design/fauna/birds.md"),
            TEXT("Natural moorland grouse used as Highmoore's signature bird; grounded coloration and scale, visually suited to heather/gorse concealment."),
            TEXT("Flushes explosively from open moor and acts as a major sudden sound/atmosphere cue."),
            TEXT("No fantasy plumage or oversized game-bird design. Exact local color morph is not separately authored, so use natural grouse reference."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.hare"), TEXT("Highmoore Hare"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Sections 4.2 and 9.1; docs/design/fauna/land_animals.md"),
            TEXT("Natural hardy moorland hare with long running build and terrain-appropriate coat."),
            TEXT("Extremely fast: uncatchable on foot and barely catchable while mounted."),
            TEXT("Keep it an ordinary hare. No supernatural speed VFX, jewelry or Elowen-rabbit symbolism may be baked into the animal itself."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.red-deer"), TEXT("Highmoore Red Deer"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Sections 4.3 and 9.1; docs/design/fauna/land_animals.md"),
            TEXT("Natural red deer and stag silhouettes adapted to the Folds/Burnt March; Highmoore's largest ordinary quarry."),
            TEXT("Primary Great Hunt quarry and readable long-distance wildlife on sparse moorland."),
            TEXT("No royal/Imperial Stag features, giant legendary antlers or House heraldry. This is ordinary Highmoore game."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.fox"), TEXT("Highmoore Fox"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Sections 5.1 and 9.1; docs/design/fauna/land_animals.md"),
            TEXT("Ordinary agricultural fox suited to the Folds and moor edges."),
            TEXT("Common nuisance discussed by the yeomanry; part of the everyday ecological read."),
            TEXT("Natural fox only. Do not reuse Region 06 Threshold Fox tails, silver-white coloration or supernatural red markings."),
            true, false, false),
        Creature(
            TEXT("creature-visual.highmoore.fell-wolf"), TEXT("Highmoore Fell Wolf"), ECreatureVisualFamily::HighmooreMoorland,
            TEXT("highmoore fauna.md Sections 5.2 and 9.1; docs/design/fauna/land_animals.md"),
            TEXT("Rare high-country wolf of the Stonecountry/high fells; natural wolf anatomy with a rugged cold/open-moor survival read."),
            TEXT("Genuinely dangerous to a lone person and causes low-bond horses to bolt, but explicitly not a boss or legendary creature."),
            TEXT("Do not upscale into a legendary wolf, reuse Ash-Shadow black coloration or add supernatural features."),
            true, true, false)
    };
}

TArray<FCreatureVisualProductionDesignGap> FCreatureVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.creature.memory-fish-fixed-look"),
            TEXT("Memory Fish deliberately inherit prior player-catch appearances and have no single fixed canonical silhouette."),
            TEXT("docs/design/fauna/sea_animals.md Section 10.1")
        },
        {
            TEXT("design-gap.creature.highmoore-house-hound-breeds"),
            TEXT("Highmoore House hounds are culturally differentiated but the final House-specific breed silhouettes, coats and markings are not authored."),
            TEXT("highmoore fauna.md Section 3.2; the_lesser_houses.md Section 3.6")
        }
    };
}

bool FCreatureVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FCreatureVisualProductionBrief> Briefs = BuildBriefs();
    if (Briefs.Num() != RequiredBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Creature visual catalog requires exactly %d briefs; found %d."), RequiredBriefCount, Briefs.Num()));
    }

    int32 Ready = 0;
    int32 Blocked = 0;
    TSet<FName> Seen;
    for (const FCreatureVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.StableId.IsNone()
            || Brief.DisplayName.IsEmpty()
            || Brief.GoverningSource.IsEmpty()
            || Brief.VisualFacts.IsEmpty()
            || Brief.BehaviorRead.IsEmpty()
            || Brief.AntiInventionBoundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Every creature visual brief requires identity, source, visual facts, behavior and anti-invention boundary."));
        }
        if (Seen.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate creature visual id: %s"), *Brief.StableId.ToString()));
        }
        Seen.Add(Brief.StableId);

        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;

        if (Brief.bMustRemainNonCombat && Brief.bCombatCreature)
        {
            OutErrors.Add(FString::Printf(TEXT("Creature %s cannot be both combat-enabled and hard non-combat."), *Brief.StableId.ToString()));
        }
    }

    if (Ready != ProviderReadyBriefCount || Blocked != ProviderBlockedBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Creature provider boundary requires %d ready and %d blocked briefs; found %d ready and %d blocked."),
            ProviderReadyBriefCount, ProviderBlockedBriefCount, Ready, Blocked));
    }

    for (const FName BlockedId : { FName(TEXT("creature-visual.memory-fish")), FName(TEXT("creature-visual.highmoore.house-hound")) })
    {
        const FCreatureVisualProductionBrief* Brief = Briefs.FindByPredicate([BlockedId](const FCreatureVisualProductionBrief& Candidate)
        {
            return Candidate.StableId == BlockedId;
        });
        if (Brief == nullptr || Brief->bProviderReferenceReady || !Brief->AntiInventionBoundary.Contains(TEXT("DESIGN-GAP")))
        {
            OutErrors.Add(FString::Printf(TEXT("Creature visual design gap %s must remain provider-blocked."), *BlockedId.ToString()));
        }
    }

    const FCreatureVisualProductionBrief* SeaMotherShadow = Briefs.FindByPredicate([](const FCreatureVisualProductionBrief& Candidate)
    {
        return Candidate.StableId == TEXT("creature-visual.sea-mothers-shadow");
    });
    if (SeaMotherShadow == nullptr || SeaMotherShadow->ExistingVisualDependency != TEXT("boss-visual.havfrue-modor"))
    {
        OutErrors.Add(TEXT("Sea Mother's Shadow must depend on the existing Havfrue visual rather than creating a second owner."));
    }

    if (BuildDesignGaps().Num() != ProviderBlockedBriefCount)
    {
        OutErrors.Add(TEXT("Creature visual design-gap count must match provider-blocked brief count."));
    }

    return OutErrors.IsEmpty();
}