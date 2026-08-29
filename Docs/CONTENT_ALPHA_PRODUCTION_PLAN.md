# Dark Arisen authored-content production plan

**Branch:** `feature/content-alpha-production`  
**Integrated base:** `develop` at `6e1620b70d70d2535b94e0edf59d64debfc57d6b` (replacement PR #14 for former Draft PR #12)  
**Purpose:** move from source contracts to authored game content without inventing missing canon, weakening design laws, or pretending unavailable UE runtime evidence exists.

## Authority

1. Current operator direction.
2. `Docs/AGENT_RULES.md`, `SECURITY.md`, `Docs/DesignAuthority.md`.
3. Locked design laws.
4. Newest narrow subsystem documents.
5. Phase/global documents.
6. Older non-conflicting drafts.

Executable Dark Arisen code remains native C++. Data/config/document formats remain data/config/document formats. No generated/radiant replacement is permitted for missing authored content.

## Production sequence

### P1 — Dungeon corpus

- Reconcile the authored dungeon build list against `dungeon system global.md`, `regional dungeon catalog.md`, `dungeon design philosophy.md`, `secret bosses.md`, Highmoore sources and Crystal Caves sources.
- Maintain native C++ authored dungeon identities, production profiles, shared spatial progression, access rules, no-boss carve-outs, Secret Site decisions and native tests.
- Do not invent names or narrative content for the 20 minor sites. Their regional slot counts are locked; individual definitions remain explicit authoring gaps until a governing source exists.
- Preserve the unresolved count tension: the global table says 41 named + 20 minor = 61 while Crystal Caves is separately described as a category of one. Source records ambiguity instead of fabricating reconciliation.
- Current source state: forty unambiguously named regional/cross-network sites are catalogued; Crystal Caves is separate; twenty minor slots are regionally reserved; the unresolved Region 06 named/Tier-E slot remains fail-closed.

### P2 — Regions, levels and world population

- Build native C++ level/runtime descriptors and authored placement contracts for the eight production region identities.
- Integrate already-written M2-M6 systems into reachable game paths; eliminate the condition where major systems exist only as uninstantiated source/test classes.
- Level geometry, art, materials, foliage, lighting, navigation and Unreal assets remain real authored production work and must cite source/spec ownership. No procedural world filler.
- No region is called production-complete until an actual reviewed `.umap`/asset path is bound and runtime evidence exists.

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

### P5 — Boss and encounter corpus

- Keep all nine Tier-1 commanders grounded in the canonical register and `colonial war bosses.md`.
- Build/complete native C++ encounter state only where the authored mechanics require it; special cases remain explicit.
- Dungeon bosses remain separate Tier-4 content and obey dungeon silence/hazard/spatial rules.

### P6 — Animation production

- Use `animation system.md` plus existing combat/movement/wound/horse/weapon owners to produce the finite animation requirement set before asset generation/integration.
- Preserve the six-frame deflection law, 8+ frame readable attack tells, non-cancellable recovery, wounded layers, Katana exception, mounted transitions and named hero animations.
- Executable behavior stays native C++; animation assets are presentation/data, never gameplay authority.
- No animation is called complete until a reviewed imported asset exists and is wired to the correct C++ state.

### P7 — Cutscenes and protected playable presentation

- Use the 19-cutscene catalog and 22 protected playable moments as separate production lists.
- Protected playable moments may never become Sequencer-owned cutscenes.
- Lock camera ownership, timing, animation priority, silence/music state and transitions before Sequencer/capture production.
- No unapproved slow motion, camera seizure, music sting, revive prompt or spectacle layer may leak into protected moments.

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

## Current execution line

1. Continue dungeon/world reachability and production binding.
2. Build the finite Treasure/Reward/Unique Item authority from existing source.
3. Finish main/side quest and finite mission identities without filler.
4. Finish encounter/boss production integration.
5. Build animation asset-production requirements and wiring.
6. Build the 19 cutscene packets and protect all 22 playable moments.
7. Finish audio/music/credits production.
8. Lock full dialogue and cast/record voices last.
9. Reconcile Alpha content, package only when actual assets/runtime evidence exist, then merge completed source work according to operator authorization.
