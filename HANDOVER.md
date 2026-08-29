# Engineering handover

**Updated:** 2026-08-29  
**Integrated baseline:** `develop` at `6e1620b70d70d2535b94e0edf59d64debfc57d6b` before PR #13  
**Current integration tranche:** `feature/content-alpha-production` / PR #13 → `develop`  
**Merge direction:** operator explicitly authorized PR #13 merge after the Animation + Presentation source boundary is coherent.  
**Runtime state:** UE 5.5 Windows/Linux compile, Unreal Automation, runtime, performance, packaging and playthrough gates are deferred while matching self-hosted runners are offline. They are not waived or marked passed.

## Operator direction

Take the **Rockstar path**, not the shortest path. Continue building the actual game and preserve authored specificity, world coherence, restrained presentation and technical integrity even when that is slower than filling checkboxes.

Continue source/content production without pausing for unavailable runners. Stop only for a real design/tool decision that cannot be resolved from repository authority, or when the requested tranche is actually complete.

Do not invent missing canon. If a quest, final-act scene, tuning number, asset or production identity is not supported by approved source/evidence, keep an explicit `DESIGN-GAP` instead of manufacturing filler.

All executable project implementation remains native C++. Repository/build/configuration formats remain in their required native formats.

## Integrated history

- PR #7 integrated M3 ship + M4 systems source closure.
- PR #8 integrated M5 Colonial War + M6 Highmoore source closure.
- PR #9 integrated M7/M8 source contracts and the native-C++ project-operations migration.
- The follow-up CI/governance repair tranche is integrated on `develop` at `6e1620b...`.
- PR #13 is the current authored-content production tranche. If this file is being read on `develop` after PR #13 is merged, start the next tranche from that exact merged head rather than reviving the old feature branch.

## PR #13 authored-content state

### Dungeons

- The global 61-site structure is represented without procedural/radiant generation.
- Forty source-grounded named regional/cross-network sites are represented in native C++ production catalogs.
- Crystal Caves remain a separate category-of-one contract.
- Twenty Minor/Tier-A regional slots are reserved according to canonical distribution without fabricated names/stories.
- Named-site production profiles retain authored discovery/access, puzzle language, hazard, unforgettable image, boss/bottom and reward data where source provides it.
- Shared progression enforces Threshold → Descent → Turn → Depth → Floor → Return and prevents an outside-opened return shortcut.
- Secret-site access/resolution rules and no-boss carve-outs remain explicit.
- Cenote, Drowned Mead Hall, Obsidian Vault and Hollow City have dedicated native mechanic owners in addition to common authored-site boundaries.
- `DarkArisenOps validate-dungeons` remains the fail-closed source gate.
- Final `.umap` geometry, art, lighting, encounter placement, navmesh, creature assets, animation, VFX, audio and runtime playtesting remain open production evidence.

### Quests, Threads and missions

- All 17 named Threads are source-grounded in `FAuthoredQuestCatalog` and now have explicit native runtime/source coverage owners.
- Crew source includes Ines/Esperanza, Mira and Big Tom state chains; Father Salvio's newer parish thread and Esteban's Last Chart stop at explicit authorship gaps where later canon does not provide enough stage detail.
- Archipelago owners cover Kessler, the Ledger Trilogy, Schreiber documentation, Captain Vasquez and Old Fortresses; Vasquez remains gap-bearing rather than receiving a fabricated six-hour chain.
- Named Dead/Ethan memory state is owned without adding a closure reward/mechanic. Bjorn remains unresolved until authored source resolves him.
- Light Elves/Eleven Days and Highmoore Reconstruction retain their authored timing/path restrictions without fast-travel or redemption-meter shortcuts.
- Exactly 132 finite Turns are required. Only three individual Turn identities are currently strong enough for production source: `Rexa.Turn.EmptyHammock`, `Rexa.Turn.ThreeCutsInStone`, `Rexa.Turn.SaltLedger`.
- Exactly 147 finite Standing variants are required. Only `Rexa.Standing.Salvage.SanTelmoBell` is currently individually complete.
- The deliberate gap remains **129 unauthored Turns + 146 unauthored Standing variants = 275 identities**. Neutral coverage slots contain no invented title, NPC, location, dialogue, reward, premise or outcome.
- `DarkArisenOps validate-missions` keeps generated/radiant mission paths forbidden.

### Tier-1 bosses

- The Nine Who Hold remain exactly Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft and Thorne.
- Shared encounter source covers authored arena/phase/resolution requirements while Isabel Cruz retains her dedicated M2 actor.
- Non-combat resolutions are now constrained to source-backed route IDs instead of accepting arbitrary strings.
- Vega still cannot resolve through a clean spare. His leave/walk-away path is tied to the observed refusal sequence rather than a generic bypass.
- Final arenas, character/weapon assets, animation, AI tuning, dialogue staging and runtime evidence remain open.

### World, populations and fauna

- Native authored-region registry and level-binding contracts exist for eight production regions; a level cannot claim a canonical region identity without its source-backed registration.
- No region may claim a production `.umap` until the real reviewed asset exists.
- Population production now encodes the source density bands, exact demographic percentages where authored, regional daily rhythms, purpose-driven routines, chapter evolution, crowd recovery timing, 200+ ambient-line production minimum and child-protection requirement.
- Region 06 remains population-by-absence rather than receiving crowd filler.
- Highmoore now uses `highmoore populations.md`: four source classes — Houses, Yeomanry, Villages, Roads — with no invented demographic percentage split and no reputation meter.
- Fauna/flora source scale locks 42 land animal species + 6 legendary variants, 52 birds + 4 legendary, 65 sea species including two legendary identities inside that total, 45 small-creature types + 3 legendary and at least 60 plants/herbs + 4 legendary.
- Highmoore has its own later ecology authority rather than being silently folded into Phase-4 counts. Its MVP five are grouse, hare, red deer, fox and fell wolf; stopped-shaft animal absence and horse refusal remain explicit.
- The Patriarch, The Keeper Below and The Canopy Jaguar are source-backed named ecological anchors.
- Random population/wildlife generation and generic spawn-point substitution are prohibited. Full identity transcription and actual crowd/creature/plant/AI/audio assets remain open.
- `DarkArisenOps validate-world` now covers these contracts and generation prohibitions.

### Animation — PR #13 merge boundary

`Docs/ANIMATION_PRESENTATION_SOURCE_COMPLETION.md` is the detailed source-completion record.

- All eleven named animation requirements from `animation system.md` are collected in one finite production authority.
- All five exact weight-class timing contracts are represented: Light 9/14, Medium 13/20, Heavy 19/31, Great 26/44, Polearm 15/24 startup/recovery frames.
- Compile-time source assertions preserve the eight-frame minimum attack readability floor and exact six-frame deflection window.
- Non-cancellable recovery, posture visual states, the four wound layers, Katana clean-set exception, people carrying, interaction reach, traversal effort, horse gait transitions, horse body-language mood and Jake's permanently worse rider layer are explicit production requirements.
- Previously hard-coded wound movement/camera magnitudes are now `EditDefaultsOnly` tuning with explicit `DESIGN-GAP` comments because the source fixes the qualitative deterioration but not those numeric amplitudes.
- Qualitative law remains fixed: Winded breathing, Hurt side-favouring, Bad limp/stagger-run, Failing no sprint + weapon drag. Camera instability is driven by deterioration only.
- No binary animation asset/path is claimed before a real imported/reviewed asset exists.

### Presentation — PR #13 merge boundary

- Exactly 19 authored cinematic slots, 22 protected playable moments, 5 inserts and 6 slow pushes remain the canonical production counts.
- Control ownership is now explicit rather than flattening every catalog entry into full Sequencer control:
  - Grove: anchored camera with movement preserved, then free.
  - Emergence: six-second anchored wide with movement preserved; never a flythrough.
  - Voice From Behind: brief authored takeover for the turn only.
  - Arrow: exactly 1.5-second brief takeover; camera static; music does not react; the thirty-second aftermath is protected gameplay.
  - Real Letter: only the eight-second pillow slow push is authored; reading remains player-paced.
- Every boss death, Ibarra garden, Guardian settling, Liberation aftermath and all other Section-6 entries remain protected from cinematic conversion.
- Final-act slots #15-#19 remain unresolved because `cutscene catalog.md` points to a Crimson Armada source identity that is absent from the repository. No final-act scenes are fabricated to close this gap.
- The #9/P1 local-vs-summary discrepancy stays explicitly recorded; the narrow #9 entry wins locally without rewriting the conflicting aggregate text.
- `DarkArisenOps validate-presentation` now checks control-ownership boundaries, wound tuning-gap discipline and animation readability locks.
- Native Unreal Automation specs were added/strengthened for world, animation and presentation, but **they have not been executed** on UE 5.5 runners yet.

### Voice

- Five canonical crew voice-audition texts are source-locked against existing voice bibles.
- AI Voice Generator audition previews for Mira, Big Tom, Ines, Father Salvio and Esteban remain auditions only.
- They are not Shipping voice assets and are not treated as commercially cleared until service rights/cost terms are explicitly confirmed acceptable.
- Final voice production remains intentionally late, after dialogue/casting/pronunciation/subtitle lock and after the major world/animation/presentation content is actually stable.

## Game Studios / Kimi K3

- Imported Game Studios profiles are methodology only; Dark Arisen design authority remains above them.
- `DarkArisenOps studio-kimi` targets NVIDIA NIM model `moonshotai/kimi-k3` and reads `NVIDIA_API_KEY` only from process environment.
- No secret is stored in source or proposal output.
- Kimi output is review material under `Docs/AIProposals/` / isolated AI branches; it never becomes canon or merges automatically.
- Mission review is not a mission generator.
- The workflow cannot honestly be reported as executed while the required self-hosted UE runner is offline.

## Next Rockstar-path tranche after PR #13 merge

1. Branch from the exact new `develop` merge head.
2. Continue real world/region reachability, settlement placement, ecology and dungeon integration — no generic filler.
3. Continue finite Treasure/Reward/Unique Item and physical-evidence integration.
4. Continue source-backed mission/main-story work without inventing the 275 missing identities.
5. Continue boss/dungeon encounter integration and specific AI/presentation dependencies.
6. When real UE editor/runtime access exists, move Animation/Presentation from source contracts into actual imported animation, AnimGraph/Montage, capture/facial and Sequencer assets and review them against the source boundary.
7. Then finish audio/music/credits rights and production.
8. Lock dialogue/subtitles/pronunciation/casting and produce final voices last.
9. Bring Windows/Linux UE 5.5 runners online and execute compile + Unreal Automation; fix failures in a new PR rather than rewriting history.
10. Only after real assets/runtime exist: save/load, performance, accessibility/controller, full start-to-credits, same-commit Windows/Linux Shipping, checksums, private Pixel Streaming access test and operator Alpha approval.

## Never claim without evidence

- UE compilation passed,
- Unreal Automation passed,
- a binary level/animation/audio/cinematic asset exists merely because a C++ contract exists,
- 60 fps passed,
- start-to-credits passed,
- voice/music usage is commercially cleared,
- Windows/Linux packages are Alpha-ready,
- the Alpha is accepted.
