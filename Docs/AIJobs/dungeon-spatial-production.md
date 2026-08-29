# Kimi K3 job — Dark Arisen dungeon spatial production

## Objective

Act as the Game Studios level-design team for Dark Arisen and convert the **already-authored dungeon canon** into production-ready spatial/level/animation packets that a UE5.5 C++ team can implement.

Do **not** create new canon merely to complete the 61-site count.

## Required output

For each dungeon that the supplied governing files actually name, produce a compact production packet containing:

1. stable identity / exact authored name
2. governing source sections
3. player-experience sentence grounded in the source
4. spatial graph: threshold -> descent -> turn -> depth -> floor -> return, adjusted only where the source explicitly defines an exception
5. critical-path rooms and vertical connections
6. optional loops / observation points / physical secrets that do not require UI markers
7. cultural puzzle spatialization
8. primary hazard and how it changes traversal/encounter space
9. unforgettable-image staging requirements
10. boss/no-boss arena requirements without inventing new boss mechanics
11. mandatory inside-opened return shortcut
12. geometry/modular-kit/material/VFX requirements
13. skeletal-animation / traversal-animation / interaction-animation requirements
14. sound requirements, obeying the no-ambient-dungeon-score rule
15. existing Dark Arisen C++ authorities that should own state/integration
16. QA: softlocks, sequence breaks, visibility, return-route, marker/music/children prohibitions
17. every unresolved item under `DESIGN-GAPS`

## Priority order

Work in this order so partial output is useful even if the response limit is reached:

1. MVP: Cenote of the First Mother, Drowned Mead Hall, Obsidian Vault, Hollow City
2. remaining Rexa & Moran
3. Fjordlund
4. Ashenmoor
5. Sea
6. Colonial
7. Highmoore
8. Region 06 and cross-network sites
9. Crystal Caves only as a separate passage/category-of-one production packet

## Absolute constraints

- do not invent the unresolved second Region 06 Tier-E site
- do not invent identities for the twenty globally required unnamed Tier-A minor sites
- preserve the Fourth Fortress Tier-D/Tier-E source conflict as a DESIGN-GAP
- preserve the 41 named + 20 minor = 61 global count and report the Crystal-Caves count wording conflict rather than silently choosing a different canon total
- no dungeon map markers except physical/cartographic information already allowed by canon
- no minimap, compass or player dot
- no ambient dungeon music
- no child remains
- no generic filler corridors
- one primary environmental hazard for ordinary dungeons
- Tier B+ opens a route back from the inside
- four no-boss sites remain no-boss sites
- Fifth Connection Site has no fight in any branch
- First House gives no reward
- no new HUD or UMG/Slate design
- no Blueprint-only gameplay authority
- executable integration is native C++
- source/design proposals are not runtime evidence

## Response style

Be implementation-dense. Do not repeat the entire Bible. Prefer tables/room graphs/asset lists. Cite the governing filename and section beside every important constraint. If a value is not authored, write `DESIGN-GAP` or `TUNABLE — NOT CANON`; do not guess.
