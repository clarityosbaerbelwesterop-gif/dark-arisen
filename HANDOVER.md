# Engineering handover

**Updated:** 2026-08-29  
**Integrated baseline:** `develop` at `6e1620b70d70d2535b94e0edf59d64debfc57d6b`  
**Active source work:** authored content production toward the first private Alpha  
**Active branch / PR:** `feature/content-alpha-production` / PR #13 → `develop`  
**Runtime state:** UE 5.5 Windows/Linux compile, automation, runtime, performance, packaging and playthrough gates are deferred while matching self-hosted runners are offline. They are not waived or marked passed.

## Operator direction

Continue source/content production without pausing for unavailable runners. Work through the game toward the first Alpha. Stop only for a real design/tool decision that cannot be resolved from repository authority, or when the requested work is actually complete.

Do not invent missing canon. If a quest is not individually described by an approved source, leave it unwritten and report the exact missing slot at the end.

All executable project implementation remains native C++. Repository/build/configuration formats remain in their required native formats.

## Integrated history

- PR #7 integrated M3 ship + M4 systems source closure.
- PR #8 integrated M5 Colonial War + M6 Highmoore source closure.
- PR #9 integrated M7/M8 source contracts and the native-C++ project-operations migration.
- The follow-up CI/governance repair tranche is integrated on `develop` at `6e1620b...`.
- Detailed earlier source boundaries remain in `Docs/M3_M4_SOURCE_COMPLETION.md`, `Docs/M5_M6_SOURCE_COMPLETION.md`, `Docs/M7_M8_SOURCE_PLAN.md` and the Alpha checklist.

## Current authored-content production

### Dungeons

- The global 61-site structure is represented without procedural/radiant generation.
- Forty source-grounded named regional sites are represented in native C++ production catalogs.
- Crystal Caves remain a separate category-of-one contract.
- Twenty Minor/Tier-A regional slots are reserved according to the canonical distribution without fabricated names or stories.
- Named-site production profiles carry authored discovery/access, puzzle language, hazard, unforgettable image, boss/bottom and reward information where the source provides it.
- Shared dungeon progression enforces Threshold → Descent → Turn → Depth → Floor → Return and prevents an outside-opened return shortcut.
- Secret-site access/resolution rules and the four no-boss carve-outs are explicit.
- Cenote, Drowned Mead Hall, Obsidian Vault and Hollow City have dedicated native mechanic owners in addition to the common authored-site boundary.
- `DarkArisenOps validate-dungeons` is an additive fail-closed source gate.
- Final `.umap` geometry, art, lighting, encounter placement, navmesh, creature assets, animation, VFX, audio and runtime playtesting remain real production work.

### Quests and missions

- All 17 named Threads are source-grounded in `FAuthoredQuestCatalog`; their governing sources and activation/expiration information are retained without inventing missing journal prose.
- Phase 11 requires exactly 132 finite Turns. Only three individual Turn identities are currently authored strongly enough for native production: `Rexa.Turn.EmptyHammock`, `Rexa.Turn.ThreeCutsInStone`, `Rexa.Turn.SaltLedger`.
- Standing requires exactly 147 finite variants. Only `Rexa.Standing.Salvage.SanTelmoBell` is currently an individually complete identity.
- `FQuestCoverageRegister` exposes the exact deliberate gap: **129 unauthored Turns + 146 unauthored Standing variants = 275 identities**.
- Missing entries use neutral coverage IDs only. They have no invented title, NPC, location, dialogue, reward, premise or outcome.
- The exact grouped gap list is `Docs/QUEST_CONTENT_GAPS.md`.
- `DarkArisenOps validate-missions` requires the gap register and scans mission source for forbidden generated/radiant mission paths.

### Tier-1 bosses

- The authoritative Nine Who Hold register remains exactly Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft and Thorne.
- Shared native boss encounter authority covers source-backed arena/phase/resolution requirements while retaining specific exceptions such as Vega refusing a spare route.
- Isabel Cruz retains her dedicated M2 encounter actor rather than being flattened into a generic commander.
- Final arenas, character/weapon assets, animation, AI tuning, dialogue staging and runtime evidence remain open.

### World / regions

- Native authored-region registry and level-binding contracts exist for the production regions.
- A level may not claim a canonical region identity without registering against the matching source-backed region authority.
- This does not mean the final region `.umap` worlds, terrain, settlements, vegetation, fauna, populations, navmesh, lighting and streaming cells are already authored.

### Presentation / animation

- Native presentation contracts retain exactly 19 authored cinematics and 22 moments that must remain player-controlled.
- The animation state bridge already consumes combat, weapon-weight, posture, wound and Crystal-Katana exception state.
- The authored animation production catalog records named animation requirements instead of claiming missing binary assets exist.
- Cutscene #9's local entry says P1 while the later aggregate P1 summary omits it; the narrower local entry is retained and the aggregate inconsistency remains a design-source discrepancy rather than silently rewritten.

### Voice

- Five canonical crew voice-audition texts are source-locked against existing voice bibles.
- AI Voice Generator audition previews were generated for Mira, Big Tom, Ines, Father Salvio and Esteban.
- These are auditions only. They are not Shipping voice assets and are not treated as commercially cleared until service rights/cost terms are explicitly confirmed acceptable.

## Writing quality rule

`Docs/WRITING_STYLE_GUARDRAILS.md` applies to dialogue, journal prose, quest text, lore, item text and AI-assisted writing.

Source-backed character voice beats polish. Do not homogenise people into a shared eloquent style. Avoid machine-shaped rhetorical symmetry, generic solemnity, exposition disguised as banter and invented dialect decoration. If the information itself is not authored, do not write the line.

## Game Studios / Kimi K3

- The imported Game Studios agent profiles are methodology only; Dark Arisen design authority remains above them.
- `DarkArisenOps studio-kimi` uses NVIDIA NIM model `moonshotai/kimi-k3` and reads `NVIDIA_API_KEY` only from the process environment.
- No secret is stored in source or proposal output.
- Kimi output is restricted to `Docs/AIProposals/` review material and isolated `ai/...` branches; it never becomes canon or merges automatically.
- The Game Studios workflow has bounded jobs for dungeon spatial production, world-region production, mission-authorship gap review and animation coverage.
- Mission review is explicitly not a mission generator. Writing guardrails and the quest-gap register are supplied as governing context.
- The Kimi workflow still cannot execute until the required UE 5.5 self-hosted Windows runner is online.

## Remaining large Alpha work

1. Finish source-backed Thread implementation from each cited design source; do not touch the 275 neutral quest gaps unless new approved design appears.
2. Continue dungeon mechanics/integration and real level-production requirements for every source-grounded site.
3. Finish region/world integration contracts, populations, fauna, reconstruction, assassin-network and remaining authored content.
4. Finish boss-specific mechanics and presentation requirements.
5. Finish animation/cutscene/dialogue/subtitle/audio production contracts, then create/integrate actual UE assets when editor/runtime access exists.
6. Resolve credits/music/licensing and voice commercial-use approval before Shipping promotion.
7. Bring Windows/Linux UE 5.5 runners online and execute compile + Unreal Automation.
8. Author/validate final `.umap`/`.uasset` content in UE 5.5.
9. Run save/load, performance, accessibility/controller and full start-to-credits regression.
10. Produce same-commit Windows x64 + Linux x86_64 Shipping candidates, checksums, private Pixel Streaming access test and final operator Alpha approval.

## Never claim without evidence

- UE compilation passed,
- Unreal Automation passed,
- a binary level/animation/audio asset exists merely because a C++ contract exists,
- 60 fps passed,
- start-to-credits passed,
- voice/music usage is commercially cleared,
- Windows/Linux packages are Alpha-ready,
- the Alpha is accepted.
