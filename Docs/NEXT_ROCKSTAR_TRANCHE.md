# Next Rockstar-path production tranche

**Date:** 2026-08-29  
**Base:** `develop` at `d9f268ddf4dee30160d4f50e520fbba241d50adf`  
**Branch:** `feature/world-rewards-encounters-production`  
**Purpose:** continue authored game production after the Animation + Presentation source merge without treating unavailable UE runtime evidence as passed.

## Operator direction

Continue immediately on the Rockstar path. Do not optimise for the shortest route to an Alpha label. Preserve authored specificity, physical-world readability, finite content, restrained presentation, native-C++ ownership and fail-closed design gaps. Review the tranche at the end and merge it when coherent; runtime/editor gates remain open until they actually execute.

## Production line

1. **World reachability and region integration**
   - strengthen source-backed region entry/exit/traversal contracts so major M2-M6 systems are reachable through authored world routes rather than existing only as isolated components;
   - encode settlement/landmark placement requirements where governing sources give real identities;
   - never claim a `.umap`, navmesh, foliage, lighting, crowd, creature or streaming cell exists until reviewed Unreal asset evidence exists.

2. **Treasures, rewards and physical evidence**
   - reconcile finite named treasures, letters, state treasures, unique weapons/items and authored take/leave objects into one source-backed authority;
   - keep ownership, location, consequence and acquisition physical where written;
   - prohibit generic loot substitution and invented filler rewards.

3. **Mission/main-story integration**
   - continue only source-backed identities and state ownership;
   - preserve the 275 unauthored Turn/Standing identities as deliberate gaps;
   - bind authored content to existing journal, war, settlement, ship, social, progression and world-clock authorities instead of parallel systems.

4. **Encounter integration**
   - strengthen boss/dungeon/world encounter dependencies where source is concrete;
   - preserve special cases rather than flattening them into one generic boss or arena path;
   - keep final arenas/AI assets/animation/runtime evidence open until actual Unreal production exists.

5. **World population/ecology quality**
   - extend source-backed placement/readability contracts for Highmoore and archipelago settlements/ecology where the design corpus is explicit;
   - no random authored population, wildlife, generic spawn substitution or procedural mission filler.

## Non-negotiable evidence boundary

This tranche may add native source contracts, integration code, tests, validators and documentation. It may **not** claim:

- UE 5.5 compilation passed;
- Unreal Automation passed;
- a production `.umap`/`.uasset` exists merely because a C++ path/contract exists;
- 60 fps passed;
- Windows/Linux Shipping candidates exist;
- start-to-credits passed;
- Alpha acceptance passed.

The required self-hosted UE 5.5 runners are still offline/queued. Queued is not passed.
