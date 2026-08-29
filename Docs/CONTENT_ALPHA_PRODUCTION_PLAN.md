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
- Create a native C++ authored dungeon catalog with stable IDs and exact source citations for every individually specified site.
- Represent discovery mode, cultural puzzle language, primary hazard, unforgettable image, boss/no-boss carve-out, reward/return question, inside-opened return requirement, music/marker/child-remains prohibitions and authored duration bounds.
- Do not invent names or narrative content for the 20 minor sites. Their regional slot counts are locked; individual minor definitions remain explicit authoring gaps until a governing source exists.
- Preserve the unresolved count tension: the global table says 41 named + 20 minor = 61 while Crystal Caves is separately described as a category of one. Source records the ambiguity instead of fabricating a reconciliation.
- Current source state: forty unambiguously named regional/cross-network sites are catalogued; Crystal Caves is separate; twenty minor slots are regionally reserved; the unresolved Region 06 named/Tier-E slot remains fail-closed.

### P2 — Quest / mission corpus

- Author all individually specified Threads and Turns from current quest catalogs.
- Expand Standing mission variants only where the design corpus provides an actual authored variant; the exact type totals remain acceptance requirements, not a licence to auto-generate filler.
- Bind each mission to the existing quest/journal, world-clock, economy, war, ship, settlement and progression authorities rather than creating parallel state.

### P3 — Boss corpus

- Reconcile all nine Tier-1 bosses against the canonical register and `colonial war bosses.md`.
- Build missing native C++ boss state/encounter classes from their authored mechanics.
- Dungeon bosses remain a separate Tier-4 category and obey the dungeon silence/hazard rules.

### P4 — Regions, levels and world population

- Build native C++ level/runtime descriptors and authored placement contracts for Rexa, Fjordlund, Ashenmoor, Sea, Colonial, Region 06 and Highmoore.
- Integrate already-written M2-M6 systems into reachable game paths; eliminate the current condition where major systems exist only as uninstantiated source/test classes.
- Level geometry, art, materials and Unreal assets remain authored content and must cite source/spec ownership. No procedural world filler.

### P5 — Animation / cinematic production

- Use the 19-cutscene catalog and the 22 protected playable moments as separate production lists.
- Protected playable moments may never be converted into Sequencer-owned cutscenes.
- Create animation requirements, montage/state-machine contracts, camera ownership and timing sheets before asset generation/integration.
- No unapproved slow motion, camera seizure, music sting, revive prompt or other spectacle layer may leak into locked moments.

### P6 — Game Studios + NVIDIA Kimi K3 assistance

`Donchitos/Claude-Code-Game-Studios` is a Claude Code studio template, not a Kimi runtime. Its agent frontmatter uses Claude Code model aliases such as `sonnet`; replacing that field with Kimi would not route inference to NVIDIA.

The integration therefore has two layers:

1. **Studio methodology:** selected upstream Game Studios agents/skills are used as prompt/role sources for level design, world building, technical art, audio, narrative, QA and release review. Upstream shell hooks are not imported as Dark Arisen implementation code.
2. **Native bridge:** `DarkArisenOps` owns the executable bridge that submits bounded studio jobs to NVIDIA NIM using model `moonshotai/kimi-k3`. GitHub Actions injects `NVIDIA_API_KEY` from repository secrets; the key is never read back, printed or committed.

Kimi output is advisory/authored-proposal material, not automatic canon. It must cite the supplied governing documents and explicitly list unresolved design gaps. The bridge may write proposals to a dedicated review path/branch, but no generated proposal may auto-merge or silently become gameplay authority.

Because the allowed self-hosted runners are currently offline, the bridge/workflow can be source-complete now but background inference cannot honestly be reported as executed until a runner accepts the job.

### P7 — Dialogue, casting and voices

- Build a dialogue-lock manifest, role/casting matrix, pronunciation guide and subtitle-ready list first.
- AI Voice Generator may be used only for material that has a locked dialogue ID and a known role.
- The operator has requested AI Voice Generator for voice production. Generated lines still require the dialogue-lock/casting/pronunciation/subtitle contract before being treated as production candidates.
- Shipping integration additionally requires commercial-rights and cost approval evidence. Until that is established, generated clips remain auditions/prototypes rather than accepted Alpha assets.
- No voice clone or imitation of a real performer is required by the design.

### P8 — Audio, music and credits

- Dungeon ambience remains environmental; no ambient dungeon score.
- Music remains non-reactive globally.
- Final credits require complete attribution plus an approved/licensed non-reactive cue.

### P9 — Save/load, accessibility, packaging and Alpha evidence

- Finish persistence wiring for authored content and existing M2-M6 systems.
- Preserve Windows x64 + Linux x86_64 Alpha parity and the six-frame deflection law across accessibility/difficulty options.
- Build immutable same-commit Shipping candidates with SHA-256 manifests.
- Runtime/playthrough/performance/controller/privacy evidence stays open until it is actually observed on allowed UE 5.5 runners/hardware.
- Native PS5 remains Beta-only.

## Commit discipline

Each content family is a separate coherent commit: plan -> catalog/schema -> authored definitions -> integration -> native tests -> handover/checklist update. Re-fetch/compare before starting each new subsystem. The operator has authorised merging completed source work while unavailable UE runner evidence remains explicitly deferred; no merge may be reported as runtime/Alpha acceptance.

## Current execution line

1. Finish dungeon runtime-authority wiring and hazard/system reuse on the grounded corpus.
2. Complete the authored quest/mission register and standing-slot reconciliation.
3. Complete the nine Tier-1 boss register/mechanics integration.
4. Build region/world reachability descriptors and instantiate existing source systems through real game ownership paths.
5. Build animation/cinematic production manifests and protected-playable ownership.
6. Build dialogue locks, voice audition packets, audio/credits readiness and Alpha release evidence contracts.
7. Merge only after source audit; UE compile/runtime/package evidence remains open until approved runners/editor hardware exist.