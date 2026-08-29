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
    const TCHAR* Unknowns)
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
    Result.bProviderReferenceReady = true;
    return Result;
}
}

TArray<FBossVisualProductionBrief> FBossVisualProductionCatalog::BuildDeepDiveBossBriefs()
{
    return {
        Boss(
            TEXT("boss-visual.admiral-fitzmueller"), TEXT("Admiral Cornelius Fitzmueller"), EBossVisualProductionKind::NavalCommander,
            TEXT("docs/design/bosses/admiral_fitzmueller.md Section 2"),
            TEXT("Age 58; 1.76 m; solid/sturdy build; gray-white short officer hair with receding hairline; close-trimmed gray beard; pale-gray calm eyes; weathered sea skin; deep scar across left eyebrow."),
            TEXT("Deep-red Crimson Armada admiral coat in imperial officer cut with gold trim and epaulets; practical naval uniform; silver-chain pocket watch; old imperial service saber and practical flintlock. HMS Blacktide is 75 m, black hull with crimson trim, massive bronze stern bell, reinforced rigging and enormous red mainmast banner."),
            TEXT("Obsidian Waters: black sea, heavy-rain silhouette visibility, first-rate warship and six-vessel defensive formation; ship is the boss's primary visual body."),
            TEXT("Officer-straight but relaxed; measured professional presence with no theatrical menace."),
            TEXT("Do not invent facial ornament, alternate uniform or ship decoration beyond the source.")),
        Boss(
            TEXT("boss-visual.ashen-wyrm"), TEXT("The Ashen Wyrm"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/ashen_wyrm.md Section 2"),
            TEXT("30 m snout-to-tail, 40 m wingspan; quadrupedal serpentine form closer to an Eastern dragon but with massive wings; black-obsidian scales crossed by molten-gold cracks; long ancient horned head, two backward-curving horns, eyes like twin suns."),
            TEXT("Leather-black wings with gold veins; no human equipment. Inner fire is visible through the shifting obsidian surface."),
            TEXT("Wyrm's Heart: immense polished-black obsidian chamber, sleeping pit and hoard, pre-human runes, ceiling lost in darkness/open vent revealed later."),
            TEXT("Deeply tired rather than angry or malicious; mythic weight comes from age, restraint and scale."),
            TEXT("Do not add extra horns, armor, jewelry or fantasy ornament not in source.")),
        Boss(
            TEXT("boss-visual.brother-cleaver"), TEXT("Brother Cleaver"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/brother_cleaver.md Section 2"),
            TEXT("Age 61; 1.90 m lanky frame; white thinning tonsure; long uneven white beard; intense dark-brown eyes; tanned weathered swamp skin; forearms covered in small ritual-name scars; slightly bent posture except when preaching/fighting."),
            TEXT("Tattered former black cassock aged gray and repeatedly patched; homespun layers; wooden crucifix reshaped into a cleaver motif. Signature 80 cm Great Cleaver forged from his old church bell, six smaller ritual knives and chain censer."),
            TEXT("Chapel rebuilt from swamp materials over old parish ruins; candlelit, organic and improvised rather than gothic-fantasy generic."),
            TEXT("Warm pastoral priestly demeanor is essential; horror comes from genuine gentleness coexisting with the authored ritual practice, not cartoon madness."),
            TEXT("Avoid embellishing ritual imagery or adding gore beyond the governing source.")),
        Boss(
            TEXT("boss-visual.caldera-titan"), TEXT("The Caldera Titan — Ibu's Grief"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/caldera_titan.md Section 2"),
            TEXT("15 m feminine asymmetrical humanoid; one arm cooled black basalt, the other a flowing/reshaping magma river; cracked-stone torso with molten veins that widen like breathing; smooth ancient Anak Gunung funeral-mask face with black eye hollows and molten tears; obsidian-glass hair."),
            TEXT("No clothing or weapons; heat distortion is visible around the body."),
            TEXT("Circular caldera-rim battlefield around molten center; volcanic rock and progressively broken safe ground."),
            TEXT("Embodies sorrow rather than aggression; attacks should look reluctant and heavy, not triumphant monster posing."),
            TEXT("Do not invent jewelry, armor, human facial features or unrelated elemental effects.")),
        Boss(
            TEXT("boss-visual.captain-corazon"), TEXT("Captain Beatriz Corazón"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/captain_corazon.md Section 2"),
            TEXT("Age 44; 1.73 m; athletic lean build; strong bone structure and jaw; deep-black hair with silver at temples, short practical military cut; dark-brown focused eyes; warm-brown weathered skin; forearm training scars; thin right-cheek scar, puncture scar below left eye, near-invisible upper-lip scar."),
            TEXT("Black trousers, knee-high boots, fitted dark-crimson jacket with gold rank piping, white linen shirt, black gloves; silver chain with father's military insignia; matched 85 cm Toledo sabers, boot dagger and flintlock."),
            TEXT("Lower Battery command platform high above active coastal guns; stone, command bell, gun smoke and ongoing naval battle below."),
            TEXT("Perfect dueling posture even at rest; precise, economical, experienced rather than flashy."),
            TEXT("Do not add ornamental armor or feminized fantasy costume elements.")),
        Boss(
            TEXT("boss-visual.captain-rojas"), TEXT("Captain Ramón Rojas"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/captain_rojas.md Section 2"),
            TEXT("Age 56; 1.78 m athletic not bulky; short black hair graying at temples in naval style; precise goatee; calm dark-brown eyes; deeply tanned skin with decades of combat scars and a long burn along left ribs."),
            TEXT("Tailored black coat with dark-crimson piping, white linen shirt, black trousers, high polished leather boots; matched inherited curved cavalry sabers."),
            TEXT("Bright open Merchants' Parade Ground, 35 m square, colonnades and defaced imperial statue; emptiness and daylight are part of the read."),
            TEXT("Pure professional soldier; textbook officer posture, no rage or theatrical flourish."),
            TEXT("Do not add medals, armor or personality flourishes not authored.")),
        Boss(
            TEXT("boss-visual.don-mateo-salazar"), TEXT("Don Mateo Salazar"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/don_mateo_salazar.md Section 2"),
            TEXT("Mid-50s; 1.85 m; thin and upright; pale gaunt face, high cheekbones, trimmed black beard streaked silver, cold dark eyes."),
            TEXT("Pristine white linen suit retaining old bloodstains, red silk cravat, white leather gloves; gold rings on every finger, pocket watch, silver-handled curved Toledo blade."),
            TEXT("Private Theater: intimate oval hall with dark polished wood, velvet, candle chandeliers, round table with wine; elegance over rot is the governing contrast."),
            TEXT("Spanish fencing-master elegance; never runs; controlled ornamental motion and soft politeness remain more disturbing than monster behavior."),
            TEXT("Do not convert him into gothic vampire imagery or add supernatural traits.")),
        Boss(
            TEXT("boss-visual.el-medico"), TEXT("El Médico"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/el_medico.md Section 2"),
            TEXT("Age 54; 1.80 m; thin/ascetic; unusually smooth skin; silver-gray hair meticulously combed back; clean-shaven; pale-blue clear expressive eyes; handsome, gaunt, refined face; perfectly maintained hands."),
            TEXT("Spotless white surgical apron over charcoal-gray suit; thin leather gloves; ivory-handled osteotome, ornate brass trephine, seven-scapel bandolier and glass medical syringes."),
            TEXT("Clinical white-tile surgical theater, over-bright gas lamps and precise medical equipment; keep reference focused on sterile refinement and unease rather than gratuitous gore."),
            TEXT("Warm, genuinely kind doctor manner and unhurried precision are the core contradiction."),
            TEXT("Do not invent additional body-horror alterations or fantasy medical devices.")),
        Boss(
            TEXT("boss-visual.draven-voss"), TEXT("Captain Draven Voss — The Black Tide"), EBossVisualProductionKind::FinalNarrativeHuman,
            TEXT("docs/design/bosses/draven_voss.md Section 2; docs/characters/docs/characters/docs/characters/draven_voss.md"),
            TEXT("Age 45; 1.90 m; wiry with military bearing; black hair streaked silver and neatly tied back; trimmed precise beard; ice-blue tired eyes; old burn scar on left neck."),
            TEXT("Phase 1 deep imperial navy-blue captain's coat with brass buttons/faded gold piping and naval saber; Phase 2 coat removed over white rolled-sleeve shirt plus flintlock; Phase 3 torn/bloodied shirt and bare hands."),
            TEXT("Three connected spaces on burning Crimson Verdict: intimate captain's study, burning open deck, tiny broken helm above the storm."),
            TEXT("Never slouches; calm and sad, not feral. Phase progression strips institutional dignity rather than adding fantasy power."),
            TEXT("Existing character face identity must remain consistent; do not create a second face design.")),
        Boss(
            TEXT("boss-visual.ethan-harlow"), TEXT("Ethan Harlow"), EBossVisualProductionKind::FinalNarrativeHuman,
            TEXT("docs/design/bosses/ethan_harlow.md Section 2; docs/characters/docs/characters/ethan_harlow.md"),
            TEXT("Age 23-24 at confrontation; 1.88 m; powerful athletic build; dark-brown hair longer than prologue and tied in sailor's knot, premature gray at temples; father Thomas's resemblance; new scar left temple to jaw; eyes visibly recognize Jake."),
            TEXT("Tailored dignified deep-crimson captain's coat; massive modified two-handed family blade named Untergang."),
            TEXT("Burning main deck of Crimson Verdict in heavy rain and lightning, fire/steam, Draven silently visible at helm."),
            TEXT("The most important visual requirement is voluntary recognition: never brainwashed-husk body language. Emotional familiarity must survive the captain's coat and scar."),
            TEXT("Existing Ethan character face identity must remain consistent; do not redesign him independently for the boss encounter.")),
        Boss(
            TEXT("boss-visual.halvard-grimm"), TEXT("Admiral Halvard Grimm"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/halvard_grimm.md Section 2"),
            TEXT("Age 78; 1.84 m; once powerful, now thin/stooped; very long unkempt white hair and uneven beard; one sharp bright-blue eye, one white cataracted eye; heavily salt-weathered face."),
            TEXT("Fifty-year-old full imperial admiral uniform repaired hundreds of times with sail canvas, net cord and cloth; tarnished-black gold trim, verdigris brass buttons; immaculate old officer saber and two maintained flintlocks."),
            TEXT("Partially drowned abandoned imperial fort at low tide, wet stone, rotted flag and meticulously arranged remnants of a dead command."),
            TEXT("He still stands straight when addressed. Broken routine and age must not become comic madness."),
            TEXT("No ghost effects as literal supernatural beings; edge-of-vision impressions remain atmosphere only.")),
        Boss(
            TEXT("boss-visual.havfrue-modor"), TEXT("Havfrue Modor — The Sea-Mother"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/havfrue_modor.md Section 2"),
            TEXT("Approximately 200 m long; soft serpentine body combining whale/sea-serpent qualities without hard monster edges; iridescent skin shifting ocean-blue, silver-white and pale green-gold; eight enormous intelligent kind eyes in two rows of four; subtly unresolved near-humanoid face; long pale dragon-bone harpoon scars on left flank and tail."),
            TEXT("No armor or equipment; bioluminescent body and liquid ceremonial movement define silhouette."),
            TEXT("Deep Trench encounter around La Liberación in still dark ocean; her blue-green bioluminescence becomes the primary light source."),
            TEXT("Awe and ancient maternal kindness, not Lovecraftian horror. Default motion slow, graceful and considered despite immense destructive capability."),
            TEXT("Do not bare teeth in the reference, add tentacle-horror language or turn the face into a conventional mermaid.")),
        Boss(
            TEXT("boss-visual.high-priest-silvano"), TEXT("High Priest Silvano"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/high_priest_silvano.md Section 2"),
            TEXT("Age 58 but appears about 40; 1.82 m deceptively slender; long unbound silver-gray hair past shoulders; precise full white beard; unusually bright pale-green eyes; dark-olive sea-weathered skin; torso/arms/neck covered in faded ritual wave/storm/compass tattoos."),
            TEXT("Simple gray-blue linen robe open at chest, crimson silk waist tie, bare feet, small hand-forged iron wave medallion; 2 m polished dragon-bone Tide-Staff with iron wave cap; no blade."),
            TEXT("Large sacred Inner Sanctum centered on 8 m seawater basin, polished black concentric marble, ceiling aperture, carved theology and twelve small altars."),
            TEXT("Relaxed, centered, fluid movement from two decades of disciplined practice; no literal supernatural glow or magic transformation."),
            TEXT("Bright-eye effect remains biological/ritual-source read, not magical emissive eyes.")),
        Boss(
            TEXT("boss-visual.jaw-of-the-mire"), TEXT("Jaw of the Mire"), EBossVisualProductionKind::MonsterCreature,
            TEXT("docs/design/bosses/jaw_of_the_mire.md Section 2"),
            TEXT("14 m giant ancient alligator, roughly 8 tons; dark olive-green to nearly black spine, pale-yellow belly; back overgrown with moss, mud, vines and coral-like growths; one golden-amber intelligent eye, one milky cataracted eye; missing part of left front tooth; old rusted harpoon embedded in right shoulder."),
            TEXT("No equipment. Mire overgrowth and asymmetrical old injuries define silhouette."),
            TEXT("Black-water deep pool in dense mangrove clearing, broken old pier, muddy banks and twilight canopy."),
            TEXT("Slow/deliberate on land, explosively fast in water; pure predator behavior with almost no performative roaring."),
            TEXT("Do not add fantasy horns, armor, magical glow or mutations outside the source.")),
        Boss(
            TEXT("boss-visual.jungle-warden"), TEXT("The Jungle Warden"), EBossVisualProductionKind::ConstructCreature,
            TEXT("docs/design/bosses/jungle_warden.md Section 2"),
            TEXT("4.5 m vaguely humanoid guardian built from twisted vines, ancient stone slabs and weathered armor fragments; ironwood-root skeleton; eyeless carved wooden mask; moss/small flowers on back/shoulders; oversized left arm; hollow chest cavity with slowly pulsing green light."),
            TEXT("No conventional weapon set; rooted feet and vine/stone body are the silhouette. Ancient mask language must match the authored temple connection."),
            TEXT("25 m circular overgrown inner sanctum, shafts of golden light through broken roof, ancient pillars, cracked polished stone and massive dead Heart Tree."),
            TEXT("Moves as if reconstructing remembered body motion; roots when still; breathes and sighs instead of roaring."),
            TEXT("Do not turn the Warden into a generic Ent/tree-man or add glowing eyes.")),
        Boss(
            TEXT("boss-visual.la-viuda-negra"), TEXT("La Viuda Negra — Isabela Moreno-Vasconcelos"), EBossVisualProductionKind::HumanDuelist,
            TEXT("docs/design/bosses/la_viuda_negra.md Section 2"),
            TEXT("Age 47; 1.72 m slender/delicate-looking; severe black chignon with a single silver streak at right temple; unusually pale skin; dark-brown watchful eyes; high cheekbones, strong jaw, thin lips."),
            TEXT("Always formal black mourning dress in Spanish cut with black-on-black embroidery, black lace shawl, elbow gloves and boots; original gold wedding ring and locket portrait; six poisoned hairpins, concealed garrote, parasol rapier and twin curved Toledo daggers."),
            TEXT("Dim candlelit Grand Salon in Casa de Moreno, polished marble, single long dining table and family portrait walls."),
            TEXT("Controlled mourning composure; dangerous intelligence and speed without seductive-cartoon posing."),
            TEXT("Do not sexualize costume or add gothic-fantasy ornament beyond the authored mourning design.")),
        Boss(
            TEXT("boss-visual.kota-api"), TEXT("Red Lieutenant Kota Api — Viktor Hendricksen"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/red_lieutenant_kota_api.md Section 2"),
            TEXT("Age 47; 1.82 m solid military build; reddish-blond hair going gray, short regulation cut with widow's peak; close full reddish/gray beard; ice-blue tired eyes; weathered/tanned volcanic skin; recent burn scar across right cheekbone."),
            TEXT("Modified imperial officer uniform for volcanic climate: heavy red wool coat, iron-gray trousers, black boots, gold rank insignia and Siege of Kolsa medal; pale cream dragon-bone saber with dark veining, dragon-bone protective inserts, twin flare pistols and small brass ignition device."),
            TEXT("Great Forge built into active volcanic rock: orange furnace light, iron grates, sulfur vents and industrial dragon-bone processing."),
            TEXT("Perfect officer bearing and controlled certainty; institutional function should read before villain spectacle."),
            TEXT("Do not add magical flame armor or fantasy imperial regalia.")),
        Boss(
            TEXT("boss-visual.sturmkralle"), TEXT("Sturmkralle — The Sky Terror"), EBossVisualProductionKind::MythicCreature,
            TEXT("docs/design/bosses/sturmkralle_sky_terror.md Section 2"),
            TEXT("8 m standing, 20 m wingspan; massive raptor blending eagle/condor with something older; blue-black feathers streaked silver, sharpened appearance; huge curved golden talons; intelligent amber eyes."),
            TEXT("No equipment; storm-cloud formation at wing tips and lightning between primary feathers are explicit silhouette effects."),
            TEXT("Bare summit nest on irregular rocky platform surrounded by clouds and rotating storm; bone/charred-branch nest at center."),
            TEXT("Apex predator rage and speed; not a speaking dragon or sorrowful guardian."),
            TEXT("Do not add humanoid features, armor or unrelated elemental motifs.")),
        Boss(
            TEXT("boss-visual.scarred-twins"), TEXT("The Scarred Twins — Kira & Mira Blackthorn"), EBossVisualProductionKind::PairedHumanEncounter,
            TEXT("docs/design/bosses/the_scarred_twins.md Section 2"),
            TEXT("Identical 29-year-old twins, both 1.75 m athletic; deep-black hair with dark-copper undertones in matching tactical braids; warm-brown weathered skin; dark-brown observant eyes; angular jaws/prominent cheekbones; numerous professional scars. Kira: scar across right eyebrow. Mira: long scar from left collarbone to right hip."),
            TEXT("Kira wears low-profile black/dark-gray articulated leather-steel close-combat armor with twin shortswords, knives and grappling gauntlet. Mira wears same base cut adapted for ranged equipment plus long coat, composite bow, alchemical arrows, twin daggers and trap kit."),
            TEXT("Inner Keep Gate courtyard prepared as professional defensive position, with Mira's elevated archery vantage and authored trap field."),
            TEXT("Two distinct professionals with conscious coordination; never depict them as one mystical mind or mirrored clones with identical combat roles."),
            TEXT("Do not collapse their individual scars/equipment or invent matching supernatural effects.")),
        Boss(
            TEXT("boss-visual.twin-hooks"), TEXT("The Twin Hooks — Castor & Pollux"), EBossVisualProductionKind::PairedHumanEncounter,
            TEXT("docs/design/bosses/the_twin_hooks.md Section 2"),
            TEXT("Identical 33-year-old twins, both 1.76 m lean/whip-like; long black hair with subtle copper undertones in matching combat braids; warm-brown swamp-tanned skin; dark-brown eyes; matching Fjordlund protection tattoos on forearms. Castor has jagged right-cheek scar; Pollux deliberately mirrors it on left cheek."),
            TEXT("Castor: dark-crimson open leather vest, black trousers, bare feet, right-handed 1.2 m Fjordlund boarding hook. Pollux: matching dark-navy vest, bare feet, mirrored left-handed identical hook."),
            TEXT("Drowned Shipyard made from three partially submerged wrecks, makeshift boardwalks, hanging rigging and shallow swamp water."),
            TEXT("Economical mirrored movement forming a visual X; tragic enmeshment differs from Scarred Twins' rational partnership."),
            TEXT("Do not make them supernatural telepaths or erase the crimson/navy and right/left distinctions.")),
        Boss(
            TEXT("boss-visual.ulfar-stormhand"), TEXT("Ulfar Stormhand"), EBossVisualProductionKind::HumanCommander,
            TEXT("docs/design/bosses/ulfar_stormhand.md Section 2"),
            TEXT("Age 52; 1.96 m; broad/thick with huge arms but softening at middle after retirement; long gray-blond hair braided with iron rings; massive untrimmed beard woven with bones/small carved totems; weather-beaten face with old scar web on left cheek/jaw; gold front tooth; pale-blue predatory eyes; crude blue raid-tally tattoos on both forearms."),
            TEXT("Heavy dark-crimson fur-trimmed cloak, iron-studded leather chest piece, fur-lined boots, no helmet; huge named bearded axe Brytari and iron-banded round wooden shield."),
            TEXT("Old Imperial Fort hall turned crude throne room: defaced military stonework, raid trophies, furs, heavy table/ale and smoke-heavy torchlight."),
            TEXT("Aging brutal brawler, not noble Viking hero; effort and declining endurance should be visible."),
            TEXT("Do not beautify into heroic Norse-fantasy royalty or add horned helmet imagery."))
    };
}

TArray<FBossVisualProductionDesignGap> FBossVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.boss-visual.approved-reference-set"),
            TEXT("The twenty-one deep-dive boss specs are visually source-ready, but no Higgsfield/reference output is approved merely because these briefs exist."),
            TEXT("docs/design/bosses/*; Docs/HIGGSFIELD_GAME_ASSET_PIPELINE.md")
        },
        {
            TEXT("design-gap.boss-visual.deep-dive-vs-runtime-reconciliation"),
            TEXT("The deep-dive boss corpus is a visual/reference family, not permission to replace newer runtime boss authorities such as the Nine Who Hold. Any identity/state conflict must be reconciled by DesignAuthority before gameplay integration."),
            TEXT("Docs/DesignAuthority.md; ContentScale/Tier1CommanderCatalog; docs/design/bosses/*")
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
        if (BossBrief.bApprovedReferenceExists || !BossBrief.ApprovedReferencePath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Boss visual %s falsely claims an approved provider reference."), *BossBrief.StableId.ToString()));
        }
    }

    if (Ready != ProviderReadyBossBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("All %d deep-dive boss visual specs should remain provider-reference ready; found %d."), ProviderReadyBossBriefCount, Ready));
    }
    if (BuildDesignGaps().Num() != 2)
    {
        OutErrors.Add(TEXT("Boss visual production must preserve approved-reference and runtime-reconciliation design gaps."));
    }
    return OutErrors.IsEmpty();
}
