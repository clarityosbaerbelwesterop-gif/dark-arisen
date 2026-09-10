# Dark Arisen — Non-AI-Slop Production Standard

**Status:** mandatory production gate  
**Applies to:** authored content, level design, environments, characters, creatures, props, animation, cinematics, audio briefs, external-AI prompts and external-media review  
**Quality target:** deliberate, authored, coherent AAA craftsmanship. The reference bar is the density, restraint, physicality and intentionality associated with top-tier studios such as Rockstar Games and FromSoftware — not imitation of any one game's visual style.

This document is a rejection gate, not an inspiration board. A technically valid asset, prompt, scene, animation or level can still fail because it feels generic, decorative, over-signalled, physically weightless, culturally interchangeable or obviously machine-filled.

## 1. Authority before aesthetics

Every production item must begin from a real Dark Arisen authority source.

Required before a provider brief or implementation is accepted:

1. stable requirement/content ID;
2. governing source path;
3. the exact authored facts that must survive;
4. explicit unknowns / `DESIGN-GAP`s;
5. the gameplay, narrative or spatial reason the item exists;
6. the evidence state: requirement, previs, candidate, imported-for-review or runtime-accepted.

If a visual or narrative choice is not source-backed and is not a harmless implementation detail inside a locked boundary, it remains unresolved. The model/provider is never allowed to complete it merely because an image or scene would otherwise look more finished.

## 2. The specificity test

A Dark Arisen production brief must be recognisable as belonging to this game even after all franchise names are removed.

Reject a brief if it could be pasted unchanged into a generic pirate game, generic Souls-like, generic medieval-fantasy game or generic cinematic trailer.

Every accepted brief must contain at least three source-specific anchors from relevant categories such as:

- culture / region / historical layer;
- function / occupation / gameplay role;
- material / construction / wear logic;
- physical hazard / traversal condition;
- environmental story / prior human use;
- character behaviour / posture / relationship context;
- exact camera or control-ownership law;
- exact combat-readability or animation law.

More adjectives are not specificity.

## 3. Functional world rule

Nothing exists only because it looks cool.

Architecture, rooms, paths, tools, weapons, clothing, ships, settlements and props must answer:

- who made it;
- what it was made to do;
- how people use it;
- what climate/material constraints shaped it;
- how it aged, failed, was repaired or was repurposed;
- what the player can infer without an exposition dump.

Reject ornamental filler that has no believable maker, user, maintenance path or structural reason.

## 4. Restraint and negative space

Dark Arisen does not solve emptiness by adding decoration.

Reject by default:

- decorative clutter inserted only to fill composition;
- excessive hanging objects, skulls, spikes, chains, candles, banners, crates or debris without source purpose;
- unexplained glowing runes, magical particles, aura fog or energy effects;
- constant volumetric shafts, embers, fireflies or floating dust used as a quality shortcut;
- over-detailed surfaces where material separation and silhouette should carry the read;
- symmetrical set dressing that makes lived spaces look staged;
- every wall carrying lore dressing;
- every frame trying to be a poster.

Quiet space is intentional when it improves scale, dread, orientation, pacing or focus.

## 5. Material honesty

Materials must behave like materials, not texture packs.

Review wood, iron, stone, cloth, leather, glass, vegetation, mud, water, ash, ice and bone for:

- plausible thickness and edge behaviour;
- construction joints and fastening;
- region/climate-appropriate wear;
- contact wear where hands, feet, ropes, weapons or weather actually act;
- dirt accumulation based on gravity/use rather than random grunge masks;
- wetness, corrosion, cracking or fading only where the environment supports it;
- readable large material masses before micro-detail.

Reject uniform procedural grime and universal edge damage.

## 6. Silhouette before detail

Characters, bosses, creatures, ships, landmarks and important props must read at gameplay distance.

The first review pass asks:

- is the silhouette distinctive for a source-backed reason;
- can role/weight/function be read without zooming in;
- is asymmetry caused by use/history rather than arbitrary decoration;
- are proportions serving movement and gameplay;
- does equipment placement make physical sense.

If a design needs surface noise to become distinctive, it is not ready.

## 7. Character anti-slop rule

No provider may silently choose missing face, body, ethnicity, hair, costume, age detail, attractiveness treatment or fashion language when the governing sources do not authorize it.

Reject:

- same-face syndrome;
- fashion-model skin in a physically demanding world without source reason;
- hyper-clean costume layers presented as historical authenticity;
- random belts/pouches/straps with no use;
- exaggerated fantasy armour anatomy;
- universal heroic posture;
- generic "rugged pirate" shorthand;
- modern editorial posing unless the scene is explicitly a reference sheet.

Character identity comes from authored physical facts, behaviour, social role, history, movement and objects — not from AI beauty defaults.

## 8. Creature and boss anti-slop rule

Natural fauna stays natural unless the source says otherwise. Deliberately ambiguous anatomy stays ambiguous.

Reject:

- adding horns, glowing eyes, bone armour, spikes or mutations because the subject is a boss;
- turning old animals into fantasy monsters;
- increasing scale without source authority;
- making every boss visually aggressive before combat;
- explaining a creature through decorative symbolism;
- resolving withheld anatomy, identity or origin.

A boss may be memorable because of behaviour, setting, timing, silhouette, material context or player knowledge rather than visual excess.

## 9. Level and dungeon anti-slop rule

Every space must have authored spatial intent.

For dungeons preserve the actual production profile: discovery/access, puzzle language, primary hazard, unforgettable image, boss/bottom, reward/evidence and Return law where applicable.

Reject:

- generic room-corridor-room chains;
- procedural-looking modular repetition;
- combat arenas telegraphed by suspiciously empty circular rooms;
- arbitrary verticality;
- extra rooms added to make a dungeon feel "bigger";
- meaningless traversal squeezes;
- identical treasure-room composition;
- environmental storytelling duplicated through notes, corpses and props when one strong physical fact would do;
- map-marker-shaped landmarks or excessive visual signposting.

Density means meaningful information per metre, not asset count per metre.

## 10. Animation anti-slop rule

Movement must show mass, commitment, balance and consequence.

Preserve Dark Arisen's locked combat readability: the six-frame deflection window, readable attack tells, non-cancellable recovery where authored, wound layers, horse gait ownership and weapon-specific timing.

Reject:

- floaty interpolation;
- feet sliding through planted actions;
- centre-of-mass motion that ignores the action's load;
- weapon arcs with no shoulder/hip/stance consequence;
- animation smoothing that erases authored timing;
- constant idle micro-motion;
- over-performed breathing and head movement;
- generic mocap gestures pasted onto every character;
- motion-reference video being treated as a final game animation.

External video is reference until retargeting, montage/AnimGraph ownership, notifies and runtime review actually exist.

## 11. Cinematic anti-slop rule

Camera movement must have a narrative reason and must respect presentation ownership.

Reject:

- default orbit shots;
- drone-like establishing flythroughs where the scene does not require them;
- speed ramps used as excitement punctuation;
- constant shallow depth of field;
- lens flares and rack focuses as decoration;
- unnecessary cuts during strong performance beats;
- converting protected playable moments into cinematics;
- camera motion that telegraphs an authored surprise before the player should know it.

Stillness is a valid authored choice. Player control is a valid authored choice. A generated previs cannot override either.

## 12. Audio anti-slop rule

Silence and environmental sound are first-class tools.

Reject:

- reactive score where the design forbids it;
- ambient dungeon music where the dungeon law forbids it;
- constant cinematic bass impacts;
- whooshes on ordinary camera motion;
- musical stingers for every reveal;
- layered ambience that leaves no acoustic negative space;
- generic fantasy whispers, drones or choir used to signal mystery.

Sound should emerge from place, material, weather, bodies, machinery, animals and authored score moments.

## 13. External-AI prompt rule

Every Higgsfield/Kimi/provider prompt must contain:

- governing source(s);
- non-negotiable authored facts;
- explicit unknowns that must remain unresolved;
- what the output is for (look exploration, motion reference, cinematic previs, etc.);
- what the output is **not** allowed to decide;
- an instruction to prefer restraint and functional logic over decorative completion.

A prompt containing only mood/style adjectives is rejected before submission.

Provider output is rejected if it introduces canon that the brief explicitly withheld, even if the result looks excellent.

## 14. Review passes

Every important visual/content candidate is reviewed in this order:

### Pass A — Canon
Does it preserve the source and keep unknowns unknown?

### Pass B — Function
Can every major form, room, object, costume layer and movement be explained by use?

### Pass C — Composition / silhouette
Does it read clearly without micro-detail or effects?

### Pass D — Physical credibility
Do materials, weight, construction, locomotion, weather and damage behave plausibly?

### Pass E — Restraint
What can be removed while making the result stronger?

### Pass F — Repetition
Does this reuse an AI-default face, camera move, prop cluster, architecture pattern, colour treatment or animation cadence already seen elsewhere?

### Pass G — Runtime role
Does the candidate actually serve player navigation, combat readability, story, mood or world understanding?

Failing any required pass returns the item to production. "Looks good" is not an acceptance criterion.

## 15. Automatic rejection list

Reject without polishing when any of these are true:

- missing governing source;
- missing stable source requirement ID;
- provider filled a `DESIGN-GAP`;
- generic filler exists only to satisfy a count;
- generated output is being called canon automatically;
- generated output is being called a UE asset before import/review;
- generated output is being called runtime-ready before runtime evidence;
- visual identity is largely produced by ungrounded fantasy decoration;
- scene depends on effects/clutter to hide weak composition;
- animation violates authored timing/weight/control ownership;
- camera converts protected gameplay into spectacle;
- asset repeats a recognisable provider default without a source-backed reason.

## 16. The subtraction question

Before approval, ask:

> If we remove the five most decorative things in this asset/scene, does the authored idea become stronger or disappear?

If it becomes stronger, remove them.

If it disappears, the concept probably depended on decoration instead of design.

## 17. Evidence boundary

Passing this standard does **not** mean the item is runtime accepted.

The evidence ladder remains:

`RequirementOnly` → `PrevisGenerated` / `CandidateAssetGenerated` → `ImportedForReview` → `RuntimeAccepted`.

Only actual Unreal import/review can establish imported content. Only exact-commit UE/runtime evidence can establish runtime acceptance.
