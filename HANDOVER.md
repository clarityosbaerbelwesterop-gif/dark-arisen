# Engineering handover

**Updated:** 2026-08-30  
**Integrated baseline:** `develop` at `04a34e37f4f0c2f982cf28b942e11e120d103d29` after PR #17  
**Current source tranche:** `feature/pre-runner-asset-production` / Draft PR #18 → `develop`  
**Runtime state:** UE 5.5 Windows/Linux compile, Unreal Automation, runtime, performance, packaging and playthrough gates remain deferred/unverified while matching self-hosted runners are offline/queued. A queued job with no assigned runner is not passed.

## Operator direction

Take the **Rockstar path**, not the shortest path. Continue building actual authored game production without manufacturing content to close counts. Work until the next genuine authoring blocker: a design fact is absent, a provider action would require new spending/plan change, or an editor/runner fact cannot be produced from source.

Executable project implementation remains native C++. Required repository/config/build formats remain in their native formats. Missing canon is represented as `DESIGN-GAP`, never generated filler.

The current production direction adds Higgsfield as a bounded external asset/previsualisation provider and keeps AI Voice Generator for the later voice stage. Neither provider is design authority. External media is not a UE production asset merely because generation succeeded.

## Integrated history

- PR #7: M3 ship + M4 systems source closure.
- PR #8: M5 Colonial War + M6 Highmoore source closure.
- PR #9: M7/M8 source contracts + native-C++ project-operations migration.
- CI/governance repair tranche integrated before later authored-content work.
- PR #15: authored dungeon/world/population/fauna production, all 17 Thread owners, fail-closed mission gaps, Tier-1 boss constraints, Animation + Presentation source boundary. Merge commit `d9f268ddf4dee30160d4f50e520fbba241d50adf`.
- PR #17: world travel, treasures and encounter integration. Merge commit `04a34e37f4f0c2f982cf28b942e11e120d103d29`.
- Detailed boundaries: `Docs/M3_M4_SOURCE_COMPLETION.md`, `Docs/M5_M6_SOURCE_COMPLETION.md`, `Docs/M7_M8_SOURCE_PLAN.md`, `Docs/ANIMATION_PRESENTATION_SOURCE_COMPLETION.md`, `Docs/WORLD_REWARDS_ENCOUNTERS_SOURCE_COMPLETION.md`.

## PR #17 integrated state

### Treasure / physical evidence

- Five treasure classes remain Coin / Material / Document / Map / Object.
- Approximate corpus mix remains 15 / 30 / 20 / 15 / 20 percent without using those percentages to fabricate missing items.
- All nine state treasures are source-grounded documents bound to exact castles:
  1. The Conquest Archives, 1651 — La Ciudadela
  2. The Labor Ledgers — Fuerte Esperanza
  3. The 1846 Patrol Reports — Fuerte San Rafael
  4. The Bribe Ledgers — Castillo Dorado
  5. Vega's Eleven Years — Puesto del Norte
  6. The Master Ledger — Coventry House
  7. Sterling's Correspondence — The Sterling Bastion
  8. The Four Thousand Contracts — Ashcroft Hall
  9. Thorne's Dispatches — Fort Resolute
- Sterling's taken-versus-voluntary-handover distinction remains explicit.
- Approximately forty community-owned artifacts remain a sell / keep / return corpus. Missing individual identities are not fabricated.
- Sixteen buried-hoard chains remain locked as 12 archipelago + 4 Highmoore; unnamed chains remain gaps.
- `UTreasureStateComponent` persists one-time sell/keep/return decisions. Returned counted artifacts expose the authored +5 network-strength delta for the real network owner to apply.
- The cumulative return count is not player-facing; only the required hidden return-bias evidence is exposed for Harbour That Was First.
- Random loot tables, rarity colours and visible treasure/return counters remain prohibited.

### Siege / reward integration

- `UCastleSiegeComponent::RecordStateTreasureRecovered` accepts only an authored state treasure at its authored castle.
- Arbitrary IDs and wrong-castle recovery fail closed.
- Canonical persistence blocks stable-ID/source-ID alias duplication.
- This does not prove physical document meshes/materials, castle placement or acquisition presentation exists.

### Travel / world reachability

- `FAuthoredTravelProductionCatalog` records six source-backed travel modes: physical archipelago sailing, feared-water guide/trust sailing, Crystal Caves first passage, Crystal Caves repeat passage, Highmoore horse-only stable relocation and within-settlement walking.
- Crystal Caves first passage remains 90–120 real minutes; repeat passage 4–6 real minutes; both remain physical traversal.
- No water instant travel, map-click movement, compass, mini-map or player dot.
- Pairwise port-route floors and actual streaming/world links remain explicit gaps rather than invented values.

### Highmoore world production

- Twelve current source-grounded anchors: Arion, Belos, Ashby Under Moor, Lynnwater, Halford Cross, Stonecroft, The Mere Villages, Coldbeck, Dunford, Wynne's Cross, Halford Cross Abbey and The Drover's Rest.
- At least six castles remain visible from the emergence shelf; switchback remains 8–10 minutes on foot; Highmoore remains roughly 25 minutes end-to-end at a canter.
- No coordinates, generic lesser-House castles, question-mark markers or fake runtime paths are invented.

## Existing authored-content state

### Dungeons

- Global structure remains exactly 61 sites: 41 named + 20 Minor/Tier-A slots.
- Forty named site identities are grounded in current source. One Region-06 named/Tier-E identity remains unresolved rather than invented.
- Twenty Minor/Tier-A regional slots are allocated but not individually named/authored where canon is absent.
- Crystal Caves remain separate as the category-of-one passage contract and the only >90-minute dungeon carve-out.
- Tier B+ Return law, no-child-remains law, marker/discovery rules, Secret Site access/resolution and the four no-boss carve-outs remain source-owned.
- Real dungeon `.umap`, geometry, art, navmesh, lighting, creatures, encounters, animation, VFX/audio and playtest evidence remain open.

### Quests / missions

- All 17 named Threads have source/runtime coverage owners.
- Exactly 132 Turns are required; only three individual Turn identities are sufficiently grounded for production source.
- Exactly 147 Standing variants are required; only `Rexa.Standing.Salvage.SanTelmoBell` is individually complete.
- Deliberate authoring gap remains **129 Turns + 146 Standing variants = 275 identities**.
- Neutral gap slots carry no invented title, NPC, location, dialogue, reward, premise or outcome.
- Assassin Network, Liberation connections, Highmoore Reconstruction, Crew/Archipelago/Named-Dead/Light-Elves Thread state owners preserve their currently authored boundaries and explicit sub-gaps.

### Tier-1 bosses

- Nine Who Hold remain Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft and Thorne.
- Non-combat resolution routes are limited to source-backed IDs.
- Vega still has no clean spare route.
- Final arenas, AI tuning, character/weapon assets, animation, audio and runtime proof remain open.

### Population / fauna

- Eight production-region identities remain source-registered with no fake final map packages.
- Highmoore keeps its separate population register and ecology authority.
- Existing fauna/flora scale locks and named ecological anchors remain authoritative.
- Random authored population/wildlife generation and generic spawn substitution remain prohibited.

### Animation / presentation

- Eleven named animation requirements remain source-grounded.
- Exact weapon timing/readability, six-frame deflection, non-cancellable recovery, wound layers, horse/carry/traversal requirements and Crystal Katana clean-set exception remain authoritative.
- Exactly 19 cutscene slots, 22 protected playable moments, 5 inserts and 6 slow pushes remain the presentation boundary.
- Final-act cutscenes #15–#19 remain unresolved because no authoritative source identity has closed them.
- External video/reference generation does not satisfy imported animation, AnimGraph/Montage, AnimNotify, Sequencer or runtime requirements.

### Voice

- Existing AI Voice Generator outputs are auditions only.
- Shipping voice remains late-stage and requires dialogue lock, casting role, pronunciation record, subtitle text and explicit cost/rights acceptance before production can be called commercially cleared.

## Game Studios / Kimi K3

- `CLAUDE.md` now exists and records the adapted `Donchitos/Claude-Code-Game-Studios` methodology.
- Installed methodology files include `.claude/agents/*`, `.claude/skills/team-level/SKILL.md` and `.github/workflows/game-studios-kimi.yml`.
- The upstream Game Studios repository is methodology, not Dark Arisen canon.
- `DarkArisenOps studio-kimi` targets NVIDIA NIM model `moonshotai/kimi-k3`; `NVIDIA_API_KEY` stays in process/GitHub secret environment and is never read back or committed.
- Kimi output is review material only and cannot create canon, auto-merge or fill the 275 mission identities.

## Current PR #18 — pre-runner asset production

Plan: `Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md`.

### Provider preflight already performed

Higgsfield read-only account/model preflight found:

- workspace plan: free;
- available balance at preflight: 10 credits;
- free-trial unlimited generations: unavailable;
- Seedance 2.0 Mini 4-second / 480p reference-video estimate: 4 credits;
- two attempted 4-second / 480p animation-previs jobs were rejected **before job creation** with `Requires basic plan or higher`;
- therefore no Higgsfield generation job was created and no credits were consumed in that attempt.

No upgrade, subscription, credit purchase or provider-plan change is authorised or attempted.

### Higgsfield capability boundary

Useful pre-runner outputs include:

- concept/reference images;
- motion/camera/facial-performance reference video;
- 3D GLB candidate meshes where an eligible model is available;
- auto-rig/canned motion candidate output where an eligible model supports it;
- video analysis and review reference.

Higgsfield does **not** replace:

- Unreal import/reimport and asset settings;
- skeleton compatibility/retarget review;
- AnimGraph or Montage construction;
- AnimNotify gameplay ownership;
- Sequencer integration;
- collision/navmesh;
- gameplay AI;
- real level placement/streaming;
- save/load validation;
- 60-fps evidence;
- packaging or start-to-credits testing.

External output must therefore carry explicit provenance and evidence state rather than being counted immediately as production-complete content.

## PR #18 work order

1. Reconcile stale governance/checklist statements to PR #17 merge `04a34e37...` and active PR #18.
2. Add native C++ external-asset production/provenance authority with fail-closed evidence states.
3. Add Higgsfield production documentation and source-backed animation/presentation/world/prop briefs.
4. Preserve provider plan/cost/rights block without buying anything.
5. Re-scan quest source at the end and stop at the exact authorship boundary rather than inventing missing Turns/Standing variants.
6. Keep UE runtime gates open until a real runner/editor provides evidence.

## Never claim without evidence

- UE compilation passed;
- Unreal Automation passed;
- an external image/video/GLB is already an accepted UE asset;
- a binary map/animation/audio/cinematic exists merely because a source brief exists;
- 60 fps passed;
- start-to-credits passed;
- voice/music/provider usage is commercially cleared;
- Windows/Linux packages are Alpha-ready;
- the Alpha is accepted.
