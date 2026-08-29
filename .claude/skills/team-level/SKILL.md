---
name: team-level
description: "Dark Arisen adaptation of Claude-Code-Game-Studios team-level orchestration for source-backed dungeon, region and level production."
argument-hint: "[level or dungeon stable id]"
user-invocable: true
allowed-tools: Read, Glob, Grep, Write, Edit, Task, AskUserQuestion, TodoWrite
model: sonnet
---

# Dark Arisen /team-level

Adapted from `Donchitos/Claude-Code-Game-Studios/.claude/skills/team-level/SKILL.md` (MIT), pinned integration reference `984023ddac0d5e27624f2baacde6105e45de375f`.

The upstream workflow is specialized here to Dark Arisen's design authority, native-C++ rule and no-invented-canon policy.

## Phase 0 — authority and freshness

Before delegation:

1. Read `/CLAUDE.md`, `Docs/AGENT_RULES.md`, `SECURITY.md`, `Docs/DesignAuthority.md`, and `Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md`.
2. Re-read the current branch/PR state and all governing sources named by the requested level/dungeon.
3. Search existing `Source/DarkArisen/**`, tests and design docs for the stable ID/responsibility. Do not duplicate an existing owner.
4. If two governing sources conflict, record the conflict before design work. Do not choose a new canon value unless authority order resolves it.

## Studio team

Use these roles in sequence or parallel where appropriate:

- narrative-director: narrative function and only already-authored beats
- world-builder: historical/ecological/environmental storytelling constraints
- level-designer: spatial layout, pacing, shortcuts, traversal and encounter flow
- systems-designer: existing-system integration and authored hazards
- art-director / technical-artist: production specifications and asset requirements
- accessibility-specialist: navigation/accessibility review without adding HUD/markers
- qa-tester: sequence-break, softlock, design-law and playtest acceptance cases

For NVIDIA assistance, the orchestrator may create a bounded `DarkArisenOps studio-kimi` job using the same role brief. Kimi output is advisory proposal material only.

## Step 1 — locked brief

Compile a source-backed brief containing:

- stable ID and exact display name if authored
- governing documents and sections
- region/tier
- discovery rule
- cultural puzzle language
- primary hazard
- unforgettable image
- boss/no-boss rule
- reward/return question
- mandatory inside-opened return path where applicable
- cross-system dependencies
- every unresolved `DESIGN-GAP`

Never create a name, boss, reward, dialogue, duration, marker, music cue or mechanic simply because a field is empty.

## Step 2 — spatial design

The level-designer proposes the critical path, optional paths, loops, verticality, return shortcut and key sightlines while obeying:

- no map/objective markers except an explicitly cartographic discovery rule
- no generic filler corridors
- one primary environmental hazard
- no ambient dungeon score
- no child remains
- Tier B+ inside-opened return route
- Crystal Caves is a separate 90–120 minute passage and not precedent for ordinary dungeon length
- protected playable moments stay player-controlled

When an adjacent area or asset is not authored, mark the connection `UNRESOLVED`; never author the missing neighbor by implication.

## Step 3 — gameplay/system integration

Map the level onto existing C++ authorities. Do not create parallel state owners. Relevant owners include world/time/save, Colonial War, holdings/sieges, progression/economy, ship/voyage, Highmoore/Princess and Rexa settlement systems.

Any genuinely new executable rule must be native C++ under `Source/DarkArisen/**` with a native Automation test.

## Step 4 — production specification

Technical-art/animation/audio output must be implementation-ready specifications, not claims that assets exist. List:

- geometry/modular-kit needs
- material/VFX needs
- animation/montage/state-machine needs
- collision/nav/traversal needs
- environmental sound needs
- authored boss/image cue if explicitly allowed
- performance risks and LOD/Nanite implications

Do not introduce UMG/Slate or new combat HUD elements.

## Step 5 — QA and evidence

Write acceptance cases for:

- critical path completion
- return shortcut
- sequence breaks/softlocks
- save/load interaction
- marker/music/child-remains prohibitions
- boss/no-boss branch rules
- war/reactive variants if applicable
- 60-fps runtime evidence requirement

Source/test creation is not runtime evidence. If UE runners are offline, leave compile/automation/playtest evidence open.

## Output

Persist approved design work under the current PR using stable IDs and source citations. Keep unresolved gaps explicit for later operator decisions. Never auto-merge generated design or Kimi output.
