# Dark Arisen authored-content production plan

**Branch:** `feature/content-alpha-production`  
**Integrated base:** `develop` at `6e1620b70d70d2535b94e0edf59d64debfc57d6b`  
**Purpose:** move from source contracts to authored game content without inventing missing canon, weakening design laws, or pretending unavailable UE runtime evidence exists.

## Authority

1. Current operator direction.
2. `Docs/AGENT_RULES.md`, `SECURITY.md`, `Docs/DesignAuthority.md`.
3. Locked design laws.
4. Newest narrow subsystem documents.
5. Phase/global documents.
6. Older non-conflicting drafts.

Executable Dark Arisen code remains native C++. Data/config/document formats remain data/config/document formats. No generated/radiant replacement is permitted for missing authored content.

**Operator quality direction, 2026-08-29:** pursue the Rockstar path, not the shortest path. Content quality, authored specificity, world coherence, performance readability and presentation restraint take precedence over apparent checklist speed. PR #13 may merge after the Animation + Presentation source tranche is coherent; that merge is not Alpha acceptance and does not convert missing Unreal assets/runtime evidence into completed work.

## Production sequence

### P1 — Dungeon corpus

- Reconcile the authored dungeon build list against `dungeon system global.md`, `regional dungeon catalog.md`, `dungeon design philosophy.md`, `secret bosses.md`, Highmoore sources and Crystal Caves sources.
- Maintain native C++ authored dungeon identities, production profiles, shared spatial progression, access rules, no-boss carve-outs, Secret Site decisions and native tests.
- Do not invent names or narrative content for the 20 minor sites. Their regional slot counts are locked; individual definitions remain explicit authoring gaps until a governing source exists.
- Preserve the unresolved count tension: the global table says 41 named + 20 minor = 61 while Crystal Caves is separately described as a category of one. Source records ambiguity instead of fabricating reconciliation.
- Current source state: forty unambiguously named regional/cross-network sites are catalogued; Crystal Caves is separate; twenty minor slots are regionally reserved; the unresolved Region 06 named/Tier-E slot remains fail-closed.

### P2 — Regions, levels, populations and ecology

- Build native C++ level/runtime descriptors and authored placement contracts for the eight production region identities.
- Integrate already-written M2-M6 systems into reachable game paths; eliminate the condition where major systems exist only as uninstantiated source/test classes.
- Level geometry, art, materials, foliage, lighting, navigation and Unreal assets remain real authored production work and must cite source/spec ownership. No procedural world filler.
- No region is called production-complete until an actual reviewed `.umap`/asset path is bound and runtime evidence exists.
- Population production now preserves source-authored density ranges, demographics where percentages exist, daily rhythms, purpose-driven routines, chapter evolution, child protection and Region 06's population-by-absence.
- Highmoore now uses its later narrow `highmoore populations.md` authority: Houses / Yeomanry / Villages / Roads remain four distinct classes, demographic percentages remain deliberately unspecified, and no reputation meter is introduced.
- Fauna/flora source production now locks the Phase-4 corpus scale plus a separate Highmoore ecology profile. Highmoore's MVP five are grouse, hare, red deer, fox and fell wolf; its stopped-shaft animal absence and horse refusal are preserved rather than flattened into generic wildlife rules.
- Random population/wildlife generation and generic authored-ecology spawn substitution remain prohibited. Full identity transcription and actual Unreal crowd/fauna assets remain open production work.

### P3 — Treasures, rewards, unique items and physical evidence

- Reconcile dungeon rewards, nine state treasures, unique weapons/items, letters/documents, Secret-Site take/leave objects and other authored physical rewards into one finite C++ production catalog.
- Preserve source-specific ownership and consequences: a treasure/item is never substituted with generic loot just to populate a chest.
- Reuse existing inventory/economy/quest/castle/dungeon owners; do not create a second reward economy.
- Missing reward identity, location, quantity, ownership or consequence remains `DESIGN-GAP`.

### P4 — Main quests, side quests and finite missions

- Finish the main-story production graph and all individually authored side-quest Threads/Turns from the existing corpus.
- Expand Standing variants only where the corpus provides an actual authored identity; exact totals are acceptance requirements, not permission to generate filler.
- Bind missions to existing journal, world-clock, economy, war, ship, settlement, social and progression authorities rather than parallel state.
- Reconcile every mission's source identity, initiating person/place, world preconditions, outcome, reward/consequence, failure/expiry behavior and dialogue ownership.
- The current deliberate authored-identity gap remains 129 Turns + 146 Standing variants = 275. Do not reduce this number with generated filler.

### P5 — Boss and encounter corpus

- Keep all nine Tier-1 commanders grounded in the canonical register and `colonial war bosses.md`.
- Build/complete native C++ encounter state only where the authored mechanics require it; special cases remain explicit.
- Dungeon bosses remain separate Tier-4 content and obey dungeon silence/hazard/spatial rules.
- Tier-1 non-combat routes are source-ID constrained rather than accepting arbitrary route strings; Vega retains his refusal-to-be-spared exception.

### P6 — Animation production

- Use `animation system.md` plus existing combat/movement/wound/horse/weapon owners to produce the finite animation requirement set before asset generation/integration.
- Preserve the six-frame deflection law, 8+ frame readable attack tells, non-cancellable recovery, wounded layers, Katana exception, mounted transitions and named hero animations.
- Executable behavior stays native C++; animation assets are presentation/data, never gameplay authority.
- No animation is called complete until a reviewed imported asset exists and is wired to the correct C++ state.

**Current P6 source boundary:**
- all eleven named Bible animation requirements are catalogued;
- all five authored weapon timing classes are encoded (Light 9/14, Medium 13/20, Heavy 19/31, Great 26/44, Polearm 15/24 frames);
- compile-time source assertions preserve the 8-frame attack-read floor and exact 6-frame deflection window;
- wound/body requirements, carrying, horse gait/mood, traversal, interaction and Katana clean-set behavior are explicit;
- unlocked wound movement/camera magnitudes are `EditDefaultsOnly` values marked `DESIGN-GAP` rather than misrepresented as canon;
- binary animation/capture assets remain unclaimed and runtime verification remains open.

### P7 — Cutscenes and protected playable presentation

- Use the 19-cutscene catalog and 22 protected playable moments as separate production lists.
- Protected playable moments may never become Sequencer-owned cutscenes.
- Lock camera ownership, timing, animation priority, silence/music state and transitions before Sequencer/capture production.
- No unapproved slow motion, camera seizure, music sting, revive prompt or spectacle layer may leak into protected moments.

**Current P7 source boundary:**
- exactly 19 slots, 22 protected playable moments, 5 inserts and 6 slow pushes remain locked;
- Grove and Emergence use anchored camera ownership while movement remains with the player;
- Voice From Behind and Arrow are brief authored takeovers rather than generic full-scene camera seizure;
- Arrow remains exactly 1.5 seconds with static camera and non-reactive music, after which the protected thirty-second playable aftermath begins;
- Real Letter owns only the eight-second pillow push; reading remains player-paced;
- final-act slots #15-#19 remain fail-closed because the referenced Crimson Armada source identity is missing from the repository;
- no Sequencer/performance-capture/facial-animation binary set is claimed before real UE evidence exists.

See `Docs/ANIMATION_PRESENTATION_SOURCE_COMPLETION.md` for the merge boundary.

### P8 — Game Studios + NVIDIA Kimi K3 assistance

`Donchitos/Claude-Code-Game-Studios` is used as a studio-methodology source. A native `DarkArisenOps` bridge submits bounded proposal jobs to NVIDIA NIM model `moonshotai/kimi-k3` with `NVIDIA_API_KEY` injected from repository Secrets.

Kimi is advisory, never design authority. Studio jobs cover dungeon/level production, world building, technical art/animation planning, mission gap review, QA and presentation review. Generated proposals cannot auto-merge or silently become canon. Unresolved material must remain `DESIGN-GAP`.

Because the approved self-hosted runners are offline, background Kimi inference cannot honestly be reported as executed until a runner accepts a job. Source preparation continues independently.

### P9 — Audio, ambience, music and credits

- Dungeon ambience remains environmental; no ambient dungeon score.
- Current locked law: music is non-reactive globally. Older conflicting event-reactive Highmoore text does not own runtime behavior.
- Final music/SFX require actual assets, mastering/import, rights/licence/attribution evidence and runtime review.
- Final credits require complete attribution plus approved/licensed material.

### P10 — Dialogue lock, casting and final voices — LAST content-production stage

**Operator order, 2026-08-29:** final voice production happens only after world/levels, dungeons, treasures/rewards, main/side missions, bosses, animation and cutscene production are substantially resolved and dialogue text is stable.

- Build complete stable dialogue IDs, role/casting matrix, pronunciation, subtitle and scene-context packets first.
- The first five connected generic AI Voice Generator auditions were reviewed and rejected for sounding synthetic/emotionally flat. They remain negative evidence only.
- No further bulk synthesis from that generic provider for the core cast.
- Tier-1 cast requires directed human-performance-quality review; human actors are the preferred path unless a future solution demonstrably passes the same bar.
- Shipping integration requires performance acceptance, rights/contract evidence, mastering, subtitle timing, Unreal import and runtime verification.

### P11 — Save/load, accessibility, packaging and Alpha evidence

- Finish persistence wiring for authored content and existing M2-M6 systems.
- Preserve Windows x64 + Linux x86_64 Alpha parity and the six-frame deflection law across accessibility/difficulty options.
- Build immutable same-commit Shipping candidates with SHA-256 manifests.
- Runtime/playthrough/performance/controller/privacy evidence stays open until it is actually observed on approved UE 5.5 runners/hardware.
- Native PS5 remains Beta-only.

## Commit discipline

Each content family is a separate coherent commit: plan -> catalog/schema -> authored definitions -> integration -> native tests -> handover/checklist update. Re-fetch/compare before starting each new subsystem. Unavailable UE runner evidence remains explicitly deferred; no merge may be reported as runtime/Alpha acceptance.

## Current merge boundary and next execution line

PR #13 is the current authored-content source tranche. The operator has authorized merging it after the P6/P7 Animation + Presentation source boundary is coherent. Before merge: keep native validators/tests aligned, update handover/PR state and inspect reviews/status; offline runner evidence is recorded as unavailable, never as passed.

After that merge, branch again from the exact new `develop` head and continue the Rockstar path:

1. Deepen actual world/region reachability and authored placement production rather than generic filler.
2. Continue finite Treasure/Reward/Unique Item authority and physical evidence integration.
3. Continue source-backed missions/Threads and the main-story graph without fabricating the 275 missing identities.
4. Finish encounter/boss production integration and dungeon-specific encounter dependencies.
5. Move from animation/presentation source contracts into actual UE asset/capture/Sequencer production only when editor/runtime access exists.
6. Finish audio/music/credits production and rights evidence.
7. Lock full dialogue, subtitles, pronunciation, casting and final voices last.
8. Execute UE5.5 compile/Automation, performance, save/load, Windows/Linux parity, start-to-credits and private Pixel Streaming acceptance only when the approved infrastructure is actually available.
