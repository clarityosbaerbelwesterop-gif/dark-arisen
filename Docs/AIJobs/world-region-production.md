# Kimi K3 job — world region production review

Produce a **review proposal only** for turning the eight source-grounded Dark Arisen region identities into UE5.5 production packets. Do not create canon, code, binary assets, maps or filler geography.

## Required output

For each region identity already present in `AuthoredWorldRegionRegistry`:

1. governing source files and exact hard laws;
2. playable traversal spine and physically reachable boundaries;
3. settlement/landmark/dungeon/harbour/castle placement requirements that are explicitly sourced;
4. environmental storytelling and physical-navigation cues with no minimap/compass/player-dot dependency;
5. streaming/World Partition considerations and 60-fps risks;
6. modular kit opportunities versus hero-asset requirements;
7. C++ owners that level scripting may call but may not replace;
8. missing authored data as `DESIGN-GAP`, never invented content;
9. editor acceptance checklist for an eventual real `.umap` package.

## Prohibitions

- no instant travel, waypoint teleport, minimap, compass heading, player dot or objective beam;
- no new towns, factions, quests, landmarks or routes merely to fill empty space;
- no claim that a `.umap`, mesh, foliage set, material, lighting pass or population pass exists;
- no executable gameplay logic in Blueprint;
- no conflict resolution by averaging documents: use `Docs/DesignAuthority.md`.

Conclude with a dependency-ordered editor build queue and a separate list of true design gaps requiring operator/canon decisions.
