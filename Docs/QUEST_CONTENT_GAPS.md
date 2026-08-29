# Quest content gaps — do not invent

**Status:** active Alpha production register  
**Authority:** operator direction on 2026-08-29 + existing design corpus  
**Rule:** if a Turn or Standing mission is not actually described by an approved design source, implementation skips it. Kimi, Game Studios, editor tooling and gameplay code may not synthesize a story, person, place, dialogue, reward or outcome merely to satisfy a target count.

This document records **coverage slots**, not quest names. Neutral IDs exist only so missing content can be counted and located without pretending that a quest has already been designed.

## Threads are not part of this gap count

All **17/17 Threads are named and source-grounded** by `side quest catalog.md`. Their implementation may continue from the cited subsystem/character sources. Where a Thread source still omits a concrete line, journal entry, trigger detail or outcome, that specific detail remains a local `DESIGN-GAP`; it does not authorise invention.

The missing counts below concern only the individually unwritten **Turn** and **Standing** identities.

## Current authored identities

### Turns — 3 / 132

1. `Rexa.Turn.EmptyHammock` — `Rexa.Turn.01`
2. `Rexa.Turn.ThreeCutsInStone` — `Rexa.Turn.02`
3. `Rexa.Turn.SaltLedger` — `Rexa.Turn.03`

Governing implementation: `Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp`.

### Standing — 1 / 147

1. `Rexa.Standing.Salvage.SanTelmoBell` — `Rexa.Standing.Salvage.01`

Governing implementation: `Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp`.

## Missing Turn identities — 129

The Phase-11 regional totals are canonical. The following slots have **no authored quest identity yet**:

| Region bucket | Required | Authored | Missing | Neutral coverage IDs |
|---|---:|---:|---:|---|
| Rexa & Moran | 22 | 3 | 19 | `turn-gap.region.rexa-moran.001` … `.019` |
| Fjordlund | 18 | 0 | 18 | `turn-gap.region.fjordlund.001` … `.018` |
| Ashenmoor | 16 | 0 | 16 | `turn-gap.region.ashenmoor.001` … `.016` |
| Quiet Coast | 14 | 0 | 14 | `turn-gap.region.quiet-coast.001` … `.014` |
| Pale Isle / Silvera | 15 | 0 | 15 | `turn-gap.region.pale-isle-silvera.001` … `.015` |
| At Sea | 19 | 0 | 19 | `turn-gap.region.at-sea.001` … `.019` |
| Highmoore | 24 | 0 | 24 | `turn-gap.region.highmoore.001` … `.024` |
| Region 06 | 4 | 0 | 4 | `turn-gap.region.06.001` … `.004` |
| **Total** | **132** | **3** | **129** | |

Source for the quotas: `side quest catalog.md` Section 4 and the native `FAuthoredQuestCatalog` / `FMissionScaleRequirements` contracts.

`side quest catalog.md` gives the Turn composition shapes and regional weighting, but those are not individual quest identities. A shape such as “somebody is lying” is not enough to invent a person, motive, place or outcome.

## Missing Standing identities — 146

The nine finite mission-type totals are canonical. Only the San Telmo Bell Salvage variant currently has an individually authored identity.

| Standing type | Required | Authored | Missing | Neutral coverage IDs |
|---|---:|---:|---:|---|
| Escort | 21 | 0 | 21 | `standing-gap.escort.001` … `.021` |
| Convoy Raid | 18 | 0 | 18 | `standing-gap.convoy-raid.001` … `.018` |
| Recovery | 24 | 0 | 24 | `standing-gap.recovery.001` … `.024` |
| Champion | 12 | 0 | 12 | `standing-gap.champion.001` … `.012` |
| Transport | 16 | 0 | 16 | `standing-gap.transport.001` … `.016` |
| Privateer Commission | 14 | 0 | 14 | `standing-gap.privateer-commission.001` … `.014` |
| Road Work | 15 | 0 | 15 | `standing-gap.road-work.001` … `.015` |
| Hunt | 13 | 0 | 13 | `standing-gap.hunt.001` … `.013` |
| Salvage | 14 | 1 | 13 | `standing-gap.salvage.001` … `.013` |
| **Total** | **147** | **1** | **146** | |

Source for the quotas: `mission types.md` Sections 1–10 and the native `FMissionScaleRequirements` contract.

## Total deliberately unwritten quest identities

**275 = 129 Turns + 146 Standing variants.**

These 275 entries are intentional fail-closed gaps. They remain unwritten until approved design authority supplies enough concrete narrative material.

## Promotion rule

A neutral slot may move to `Authored` only when all of the following exist in an approved source:

- a concrete quest identity/premise,
- the people or entities involved,
- the actual place or route,
- the meaningful state/outcome,
- enough authored detail to implement without inventing narrative facts.

When a slot is promoted, update:

1. the relevant design source / operator-approved brief,
2. the native quest catalog,
3. `FMissionScaleRequirements`,
4. `FQuestCoverageRegister`,
5. this document,
6. native automation coverage.

**Forbidden:** using Kimi, Game Studios, procedural templates, radiant generation or generic filler to reduce the missing count without new approved design authority.
