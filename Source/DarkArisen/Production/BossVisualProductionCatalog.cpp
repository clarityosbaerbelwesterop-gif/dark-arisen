// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/BossVisualProductionCatalog.h"

namespace
{
FBossVisualProductionBrief Boss(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EBossVisualProductionKind Kind,
    const TCHAR* Source,
    const TCHAR* Visual,
    const TCHAR* Equipment,
    const TCHAR* Arena,
    const TCHAR* Performance,
    const TCHAR* Unknowns,
    const bool bProviderReady = true)
{
    FBossVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Kind = Kind;
    Result.GoverningSource = Source;
    Result.VisualFacts = Visual;
    Result.EquipmentOrSilhouette = Equipment;
    Result.ArenaVisualRead = Arena;
    Result.PerformanceRead = Performance;
    Result.ExplicitUnknowns = Unknowns;
    Result.bProviderReferenceReady = bProviderReady;
    return Result;
}
}

TArray<FBossVisualProductionBrief> FBossVisualProductionCatalog::BuildDeepDiveBossBriefs()
{
    return {
        Boss(
            TEXT("boss-visual.admiral-fitzmueller"), TEXT("Admiral Cornelius Fitzmueller"), EBossVisualProductionKind::NavalCommander,
            TEXT("docs/design/bosses/admiral_fitzmueller.md Section 2"),
            TEXT("58, 1.76 m, sturdy, gray-white short officer hair, close gray beard, pale-gray eyes, weathered sea skin, left-eyebrow scar."),
            TEXT("Deep-red Armada admiral coat over practical naval uniform; old service saber/flintlock. HMS Blacktide: 75 m black hull, crimson trim, bronze stern bell, reinforced rigging, huge red banner."),
            TEXT("Obsidian Waters: black sea, rain/spray silhouette visibility, six-vessel defensive formation."),
            TEXT("Officer-straight, relaxed, measured and professional rather than theatrical."),
            TEXT("No additional uniform ornament, facial detail or ship decoration is canonical.")),
        Boss(
            TEXT("boss-visual.ashen-wyrm"), TEXT("The Ashen Wyrm"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/ashen_wyrm.md Section 2"),
            TEXT("30 m long, 40 m wingspan; quadrupedal serpentine dragon; black obsidian scales with molten-gold cracks; long ancient horned head and sun-like eyes."),
            TEXT("Leather-black wings with gold veins; inner fire through obsidian surface; no human equipment."),
            TEXT("Wyrm's Heart: immense polished obsidian chamber, hoard, pre-human runes and high/open vent."),
            TEXT("Ancient, tired and restrained; mythic weight rather than rage."),
            TEXT("No extra horns, armor, jewelry or generic fantasy ornament.")),
        Boss(
            TEXT("boss-visual.brother-cleaver"), TEXT("Brother Cleaver"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/brother_cleaver.md Section 2"),
            TEXT("61, 1.90 m lanky; thinning white tonsure, long uneven white beard, dark-brown eyes, weathered swamp skin, ritual-name scars on forearms."),
            TEXT("Aged patched former cassock; cleaver-shaped wooden crucifix; 80 cm church-bell Great Cleaver, ritual knives and chain censer."),
            TEXT("Improvised swamp chapel grown over parish ruins, candlelit and organic."),
            TEXT("Warm pastoral gentleness must remain visible; never cartoon madness."),
            TEXT("Do not elaborate ritual imagery or gore beyond the source.")),
        Boss(
            TEXT("boss-visual.caldera-titan"), TEXT("The Caldera Titan — Ibu's Grief"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/caldera_titan.md Section 2"),
            TEXT("15 m feminine asymmetrical humanoid; one basalt arm, one flowing magma arm; cracked-stone torso, funeral-mask face, black eye hollows, molten tears, obsidian-glass hair."),
            TEXT("No clothing or weapon; visible heat distortion."),
            TEXT("Circular caldera rim around molten center and progressively broken safe ground."),
            TEXT("Sorrow and reluctance, not triumphant monster posing."),
            TEXT("No jewelry, armor or human facial detailing.")),
        Boss(
            TEXT("boss-visual.captain-corazon"), TEXT("Captain Beatriz Corazón"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/captain_corazon.md Section 2"),
            TEXT("44, 1.73 m athletic/lean; short deep-black hair silvering at temples; dark-brown eyes; warm-brown weathered skin; specific cheek/eye/lip scars."),
            TEXT("Black trousers/boots, fitted dark-crimson officer jacket, white shirt, gloves, father's insignia pendant; matched Toledo sabers, boot dagger, flintlock."),
            TEXT("Lower Battery observation platform above active coastal guns, command bell, smoke and naval battle below."),
            TEXT("Perfect economical dueling posture; refined but not flashy."),
            TEXT("No ornamental armor or sexualized fantasy costume.")),
        Boss(
            TEXT("boss-visual.captain-rojas"), TEXT("Captain Ramón Rojas"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/captain_rojas.md Section 2"),
            TEXT("56, 1.78 m athletic; short black hair graying at temples, precise goatee, calm dark-brown eyes, deeply tanned skin and old combat scars."),
            TEXT("Tailored black coat with dark-crimson piping, white shirt, black trousers/boots; matched inherited cavalry sabers."),
            TEXT("Bright Merchants' Parade Ground, colonnades and faceless imperial statue."),
            TEXT("Pure professional-soldier read: controlled, direct, no rage."),
            TEXT("No invented medals, armor or personality ornament.")),
        Boss(
            TEXT("boss-visual.don-mateo-salazar"), TEXT("Don Mateo Salazar"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/don_mateo_salazar.md Section 2"),
            TEXT("Mid-50s, 1.85 m, thin/upright; pale gaunt face, high cheekbones, black beard streaked silver, cold dark eyes."),
            TEXT("Pristine white linen suit with old stains, red silk cravat, white gloves, gold rings, pocket watch, silver-handled curved Toledo blade."),
            TEXT("Private Theater: intimate oval candlelit hall, dark polished wood, velvet and wine table."),
            TEXT("Elegant Spanish fencing-master movement; polite and controlled, never supernatural."),
            TEXT("Do not turn him into gothic-vampire imagery.")),
        Boss(
            TEXT("boss-visual.el-medico"), TEXT("El Médico"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/el_medico.md Section 2"),
            TEXT("54, 1.80 m thin/ascetic; silver-gray hair precisely combed back, clean-shaven, pale-blue expressive eyes, refined gaunt face, immaculate hands."),
            TEXT("Spotless white surgical apron over charcoal suit; leather gloves; osteotome, brass trephine, scalpels and glass syringes."),
            TEXT("Over-bright white-tile surgical theater with precise medical equipment."),
            TEXT("Warm country-doctor manner and unhurried precision create the unease."),
            TEXT("No invented mutations or fantasy medical technology.")),
        Boss(
            TEXT("boss-visual.draven-voss"), TEXT("Captain Draven Voss — legacy final-act spec"), EBossVisualProductionKind::FinalNarrativeHuman,
            TEXT("docs/design/bosses/draven_voss.md; docs/characters/docs/characters/docs/characters/draven_voss.md; Docs/DesignAuthority.md"),
            TEXT("Legacy source contains a detailed appearance, but DesignAuthority says this Draven character/boss material conflicts with current Phase 11."),
            TEXT("Legacy officer/pirate costume progression is reference-only history until rewritten/restored."),
            TEXT("Legacy Crimson Verdict final-fight staging is not current production authority."),
            TEXT("No provider generation from the conflicted final-act spec."),
            TEXT("Current Phase 11 Draven visual/performance sheet remains unresolved."),
            false),
        Boss(
            TEXT("boss-visual.ethan-harlow"), TEXT("Ethan Harlow — legacy final-act spec"), EBossVisualProductionKind::FinalNarrativeHuman,
            TEXT("docs/design/bosses/ethan_harlow.md; docs/characters/docs/characters/ethan_harlow.md; Docs/DesignAuthority.md"),
            TEXT("Legacy source contains a detailed appearance, but DesignAuthority says this Ethan character/boss material conflicts with current Phase 11."),
            TEXT("Legacy captain-coat/two-handed-blade presentation is reference-only history until rewritten/restored."),
            TEXT("Legacy burning-deck boss staging is not current production authority."),
            TEXT("No provider generation from the conflicted final-act spec."),
            TEXT("Current Phase 11 Ethan visual/performance sheet remains unresolved."),
            false),
        Boss(
            TEXT("boss-visual.halvard-grimm"), TEXT("Admiral Halvard Grimm"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/halvard_grimm.md Section 2"),
            TEXT("78, 1.84 m, thin/stooped; long unkempt white hair/beard; one sharp blue eye, one cataracted white eye; deeply salt-weathered face."),
            TEXT("Ancient repeatedly repaired imperial admiral uniform; immaculate old saber and two maintained flintlocks."),
            TEXT("Partially drowned abandoned fort at low tide, wet stone, rotted flag, ceremonially arranged remnants."),
            TEXT("Broken routine and age without comic madness; still straightens to officer bearing."),
            TEXT("No literal ghosts; edge-of-vision impressions remain atmosphere only.")),
        Boss(
            TEXT("boss-visual.havfrue-modor"), TEXT("Havfrue Modor — The Sea-Mother"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/havfrue_modor.md Section 2"),
            TEXT("About 200 m long; soft serpentine whale/sea-serpent body; iridescent ocean-blue/silver/green-gold skin; eight enormous kind eyes; subtly unresolved near-humanoid face; old harpoon scars."),
            TEXT("No armor; bioluminescence and liquid ceremonial movement define the body."),
            TEXT("Deep Trench around La Liberación, dark still ocean lit by her blue-green glow."),
            TEXT("Awe and maternal ancient kindness, never Lovecraftian horror."),
            TEXT("No exposed teeth, tentacle-horror or conventional mermaid treatment.")),
        Boss(
            TEXT("boss-visual.high-priest-silvano"), TEXT("High Priest Silvano"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/high_priest_silvano.md Section 2"),
            TEXT("58 but appears younger; 1.82 m slender; long silver-gray hair, precise white beard, bright pale-green eyes, dark-olive weathered skin, ritual wave/storm/compass tattoos."),
            TEXT("Gray-blue linen robe, crimson waist silk, bare feet, iron wave medallion; 2 m dragon-bone Tide-Staff; no blade."),
            TEXT("Inner Sanctum around seawater basin, polished black concentric marble, ceiling aperture and twelve altars."),
            TEXT("Relaxed, centered, fluid disciplined movement; no literal magic transformation."),
            TEXT("Bright eyes must not become emissive magical eyes.")),
        Boss(
            TEXT("boss-visual.jaw-of-the-mire"), TEXT("Jaw of the Mire"), EBossVisualProductionKind::MonsterCreature,
            TEXT("docs/design/bosses/jaw_of_the_mire.md Section 2"),
            TEXT("14 m ancient alligator, about 8 tons; dark olive-to-black back, pale-yellow belly, moss/mud/vine overgrowth, one amber eye and one cataracted eye, damaged tooth, old shoulder harpoon."),
            TEXT("No equipment; overgrowth and old injuries define silhouette."),
            TEXT("Black-water pool in mangrove clearing, old pier, mud banks and canopy twilight."),
            TEXT("Slow on land, explosively fast in water; predator rather than performer."),
            TEXT("No horns, armor, magic glow or invented mutations.")),
        Boss(
            TEXT("boss-visual.jungle-warden"), TEXT("The Jungle Warden"), EBossVisualProductionKind::ConstructCreature,
            TEXT("docs/design/bosses/jungle_warden.md Section 2"),
            TEXT("4.5 m vaguely humanoid guardian of twisted vines, ancient stone and weathered armor fragments; ironwood-root structure, eyeless carved mask, moss/flowers, oversized left arm, hollow chest with slow green pulse."),
            TEXT("No conventional weapon; rooted feet and vine/stone body define silhouette."),
            TEXT("Circular overgrown temple sanctum, broken-roof sun shafts and dead Heart Tree."),
            TEXT("Moves like remembered body motion reconstructed by roots; breathes/sighs rather than roars."),
            TEXT("Do not turn it into a generic Ent or add glowing eyes.")),
        Boss(
            TEXT("boss-visual.la-viuda-negra"), TEXT("La Viuda Negra — Isabela Moreno-Vasconcelos"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/la_viuda_negra.md Section 2"),
            TEXT("47, 1.72 m slender; severe black chignon with one silver temple streak; pale skin, dark-brown eyes, strong jaw/high cheekbones."),
            TEXT("Formal black mourning dress, lace shawl, long gloves/boots, wedding ring and locket; poisoned hairpins, garrote, parasol rapier and twin curved daggers."),
            TEXT("Dim candlelit Casa de Moreno Grand Salon, polished marble, long table and family portraits."),
            TEXT("Controlled mourning composure; dangerous without seductive-cartoon posing."),
            TEXT("Do not sexualize or add gothic-fantasy ornament.")),
        Boss(
            TEXT("boss-visual.kota-api"), TEXT("Red Lieutenant Kota Api — Viktor Hendricksen"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/red_lieutenant_kota_api.md Section 2"),
            TEXT("47, 1.82 m solid military build; reddish-blond graying regulation hair, reddish/gray beard, ice-blue eyes, volcanic-weathered skin, right-cheek burn scar."),
            TEXT("Modified imperial officer uniform with heavy red coat, gray trousers, black boots and rank medal; dragon-bone saber/plate, flare pistols, brass ignition device."),
            TEXT("Great Forge inside volcanic rock, furnace orange, iron grates, sulfur vents and industrial bone processing."),
            TEXT("Perfect officer bearing and institutional function before spectacle."),
            TEXT("No magical flame armor or fantasy regalia.")),
        Boss(
            TEXT("boss-visual.sturmkralle"), TEXT("Sturmkralle — The Sky Terror"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/sturmkralle_sky_terror.md Section 2"),
            TEXT("8 m standing, 20 m wingspan; massive eagle/condor-like raptor; blue-black feathers streaked silver, huge golden talons and intelligent amber eyes."),
            TEXT("No equipment; storm clouds form at wing tips and lightning arcs between primary feathers."),
            TEXT("Bare summit nest on rocky platform surrounded by rotating storm clouds."),
            TEXT("Apex-predator rage and speed, not a speaking dragon or sorrowful guardian."),
            TEXT("No humanoid features, armor or unrelated elemental motifs.")),
        Boss(
            TEXT("boss-visual.scarred-twins"), TEXT("The Scarred Twins — Kira & Mira Blackthorn"), EBossVisualProductionKind::PairedHumanEncounter,
            TEXT("docs/design/bosses/the_scarred_twins.md Section 2"),
            TEXT("Identical 29-year-old 1.75 m athletic twins; deep-black/copper tactical braids, warm-brown skin, dark-brown eyes, angular faces; Kira eyebrow scar, Mira long torso scar."),
            TEXT("Kira: articulated dark close-combat armor, twin shortswords/knives/grappling gauntlet. Mira: ranged-modified armor/long coat, composite bow, alchemical arrows, daggers/traps."),
            TEXT("Prepared Inner Keep Gate courtyard with ranged vantage and trap field."),
            TEXT("Two distinct professionals coordinating by choice; never mystical mirrored clones."),
            TEXT("Do not merge individual scars/equipment or add supernatural synchronization.")),
        Boss(
            TEXT("boss-visual.twin-hooks"), TEXT("The Twin Hooks — Castor & Pollux"), EBossVisualProductionKind::PairedHumanEncounter,
            TEXT("docs/design/bosses/the_twin_hooks.md Section 2"),
            TEXT("Identical 33-year-old 1.76 m lean twins; long black/copper braids, warm-brown skin, dark-brown eyes, matching Fjordlund forearm tattoos; mirrored cheek scars."),
            TEXT("Castor crimson vest/right-handed 1.2 m hook; Pollux navy vest/left-handed matching hook; both barefoot."),
            TEXT("Drowned Shipyard across three partially submerged wrecks, boardwalks, rigging and shallow swamp."),
            TEXT("Economical mirrored X movement and tragic enmeshment; distinct from Scarred Twins."),
            TEXT("No telepathy effects or loss of right/left and crimson/navy distinctions.")),
        Boss(
            TEXT("boss-visual.ulfar-stormhand"), TEXT("Ulfar Stormhand"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/ulfar_stormhand.md Section 2"),
            TEXT("52, 1.96 m broad/thick and softening at middle; long gray-blond hair with iron rings, massive beard with bones/totems, scarred face, gold front tooth, pale-blue eyes, blue raid-tally tattoos."),
            TEXT("Dark-crimson fur cloak, iron-studded leather, fur boots, huge bearded axe Brytari and iron-banded round shield."),
            TEXT("Defaced old imperial fort hall turned crude smoky throne room."),
            TEXT("Aging brutal brawler, not noble Norse hero."),
            TEXT("No heroic Viking beautification or horned helmet imagery."))
    };
}

TArray<FBossVisualProductionDesignGap> FBossVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.boss-visual.approved-reference-set"),
            TEXT("Provider-ready deep-dive boss specs still have no approved Higgsfield/reference output merely because briefs exist."),
            TEXT("docs/design/bosses/*; Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md")
        },
        {
            TEXT("design-gap.boss-visual.phase11-ethan-draven"),
            TEXT("Ethan and Draven legacy character/boss material conflicts with current Phase 11 and is provider-blocked until rewritten against the current story or explicitly restored."),
            TEXT("Docs/DesignAuthority.md")
        },
        {
            TEXT("design-gap.boss-visual.category-reconciliation"),
            TEXT("Deep-dive boss visuals are not the Tier-1 counting authority. The Nine Who Hold register governs Tier-1, while dungeon/secret/creature/legacy bosses remain separate categories."),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; Docs/DesignAuthority.md")
        }
    };
}

bool FBossVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FBossVisualProductionBrief> Briefs = BuildDeepDiveBossBriefs();
    if (Briefs.Num() != DeepDiveBossBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Boss visual catalog requires exactly %d deep-dive specs; found %d."), DeepDiveBossBriefCount, Briefs.Num()));
    }

    int32 Ready = 0;
    int32 Blocked = 0;
    TSet<FName> Seen;
    for (const FBossVisualProductionBrief& BossBrief : Briefs)
    {
        if (BossBrief.StableId.IsNone()
            || BossBrief.DisplayName.IsEmpty()
            || BossBrief.GoverningSource.IsEmpty()
            || BossBrief.VisualFacts.IsEmpty()
            || BossBrief.EquipmentOrSilhouette.IsEmpty()
            || BossBrief.ArenaVisualRead.IsEmpty()
            || BossBrief.PerformanceRead.IsEmpty()
            || BossBrief.ExplicitUnknowns.IsEmpty())
        {
            OutErrors.Add(TEXT("Every boss visual brief needs identity, source, visual, silhouette/equipment, arena, performance and unknown boundary."));
        }
        if (Seen.Contains(BossBrief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate boss visual id: %s"), *BossBrief.StableId.ToString()));
        }
        Seen.Add(BossBrief.StableId);

        if (BossBrief.bProviderReferenceReady) ++Ready;
        else ++Blocked;

        if (BossBrief.bApprovedReferenceExists || !BossBrief.ApprovedReferencePath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Boss visual %s falsely claims an approved provider reference."), *BossBrief.StableId.ToString()));
        }
    }

    if (Ready != ProviderReadyBossBriefCount || Blocked != AuthorityBlockedBossBriefCount)
    {
        OutErrors.Add(TEXT("Boss provider-ready/blocked split drifted from current DesignAuthority."));
    }
    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Boss visual production must preserve approved-reference, Phase-11 final-act and category-reconciliation gaps."));
    }
    return OutErrors.IsEmpty();
}