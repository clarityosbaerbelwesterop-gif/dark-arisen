# Rexa City - Layout Specification (Region 02)

**Status:** Draft v1.0, for review | **Author:** level-designer | **Date:** 2026-11-09
**Reports to:** game-designer | **Hand-off:** gameplay-programmer (generator), technical-artist, world-builder
**Authority:** docs/design/region_02_rexa_city.md. Checked against Tools/o3de/Ops/ArtKitSettlements.cpp, ArtKitArchipelago.cpp and ContentSource/World/{Locations,Places,Archipelago}.json.
**Not covered (roles not run):** narrative, art, systems, accessibility and QA input are limited to the notes where they shape geometry. Their sign-off is still open.

**Source tags.** `D` = the design doc names this (existence only; every size and position is mine). `G` = generator fact read from code. `M` = measured by replicating `ArchipelagoHeight` plus the site levelling on the real Archipelago.json (scratch harness, not committed). `P` = proposal, not canon. Anything untagged is `P`. Street names and sign texts are placeholders for the writer.

---

## 0. Change list for the programmer (read first)

The six districts cannot be built as specified by the current data. Sections 2.2 and 2.3 give the evidence; this is the resulting work list.

| ID | Change | Why (number) | Fallback if refused |
|---|---|---|---|
| R-01 | Locations.json: district `radius` becomes Harbor 170, Market 110, Cathedral 105, Noble 120, Slums 140, Cliffside 120. Add optional `groundZ` (levelling target) = 1.6 / 30 / 58 / 47 / 54 / 34. | At r=200 the six plateau discs overlap and the later site overwrites the earlier (Market centre ends at 44.5 m, slopes up to 60% inside its footprint). Harbor plateau at 8.8 m leaves the quays 7 m below the cobbles. | Keep radius 200; Market/Harbor/Cliffside layouts below are invalid; re-plan. |
| R-02 | Colonial grid: scan blocks in ring order (distance from centre), not row order; per-district `MaxBuildings`, `Fill`, `Footprint` disc; assert footprints disjoint. | Cap 160 vs ~527 buildings uncapped: row order fills only the band W=-158..-40 (south third); the north half stays empty (M, simulated). | Raise cap to 520 and accept 100% fill. |
| R-03 | Landmarks: per-landmark `offset(U,W)` and `yaw`; plazas as a `KeepClear` list of discs/rects replacing `Square = 0.22 R`. | Landmark is forced to centre with yaw = 12 deg (G). The Cathedral must face west, the plaza must sit off-centre. | Hard-code per-district constants. |
| R-04 | Corridor grading: new Roads.json polylines grade terrain Z along the edge profile (section 3.2), 5 m heightfield cells, shoulder blend 10 m. | Natural terrain deviates from the straight profile by up to +19.0 m (E7), +11.6 m (E4), -10.0 m (E1), -9.6 m (E9) (M). Without grading the 12% routes are 2-3 times longer (A* test: E4 500 m vs 157 m straight). | Avenues become ribbons on natural terrain, stairs wherever slope > 12%, E7 re-aligned as in 3.2. |
| R-05 | Waterfront per district: `waterfront` flag plus explicit pier list; pier origin at the root. | Any district with sea within 2.6 R gets 3 piers and brigs (G). With R=120, Cliffside's sea at 296 m NW would get a harbor. Piers are centred 3 m inland of the waterline, not rooted there (G). | Flag off for all but Harbor; place piers by hand. |
| R-06 | Cluster mode with Colonial variants (Cliffside) and shanty variants (Slums); contour-aligned (existing village loop). | Colonial branch returns before the organic loop (G). | Use Stilt/Nordic loop with Colonial sizes. |
| R-07 | New model variants (section 4): TC2-TC7, landmarks LM*, wall, arches, Faro, boats. | Current kit has 6 plain boxes + 5 civics. | Reuse TC1 everywhere; street-level reads fail. |
| R-08 | Civic V0 collision box = half 13.6 x 18.3, height 31.8. | Declared box is 16 x 36 x 14 (half 8.3) but the model has aisles at +-13 m, towers 33 m, dome 31.8 m (G). | Walk-through aisles. |
| R-09 | Props placed by rule with per-district caps (section 6), separate list as today. | Dress() only drops 0-2 barrels/crates at 30% of doors (G). | Hand-placed. |
| R-10 | Interiors: box colliders only (G), so no walkable interiors. Door nodes (section 5) are the interface for interior cells. | Tavern, Guild, shops, Cathedral sanctum, clinic, Library, Tavern rooms. | None; needs an interior-streaming decision. |
| R-11 | Plant clearing radius = footprint - 5 m; ruin variants; edge bands (section 8). | Clearing uses `radius` (G); jungle must start inside the ruin band. | Jungle stops at the edge, hard line. |
| R-12 | Places.json `L_RexaHarbor` -> (5240, 5258) (berth). L_World start (5650, 5320) is labelled "the shore" but is land 405 m from water (M). | Arrival point must be the berth (D). | Leave; arrival is 255 m inland of the quay. |

---

## 1. Frames and conventions

- World metres, X east, Y north, Z up. Yaw is degrees CCW about +Z; the model door is on its -Y face (G).
- **City frame** (G, StreetAngle 12 deg): `u = (0.97815, 0.20791)`, `w = (-0.20791, 0.97815)`. Local `(U,W)` about a district centre `C`: `world = C + U*u + W*w`.
- **Face** (door direction) to yaw: `-w` = 12, `+u` = 102, `+w` = 192, `-u` = 282. World west = 270, east = 90 (model +Y toward sea = yaw 90 for a west-facing quay, as the generator does).
- Footprints are written **F x D x H**: frontage (along the facade) x depth x wall height; roof adds extra. On `(U,W)`: face `+-w` -> U-size F, W-size D; face `+-u` -> U-size D, W-size F.
- Z is metres above sea after R-01. Heightfield resolution is 5 m (G); anything finer (cliff faces, stairs, walls) is geometry, not terrain.
- **Compass logic for the player:** the sea is west and north-west, the city climbs east, hills above 100 m begin east of x = 6350. West = water, east = Cathedral.
- **Street classes:** Avenida 14 m (9 carriageway + 2 x 2.5 portal/pavement); Calle 7 (4.5 + 2 x 1.25); Pasaje 3.5; Callejon 2.5 (min clear 1.8); Escalera 4-12. Carriage grade <= 12%. Stairs: riser 0.16, tread 0.30, flight <= 20 steps, landing >= 3 m, handrail both sides.
- Player capsule for clearance checks: r 0.4, h 1.8, step 0.45 `P`. Gate arch clear opening 5.5 x 4.5.
- Time of day (D): day, dusk, night, dawn. Wanted system (D) needs rooftop snipers at 4 stars and district lockdown at 5.

---

## 2. Ground truth and defects

### 2.1 Measured terrain (M)

Replica of `ArchipelagoHeight` (Rexa island: centre 8300,5300; rx 3100; ry 2200; rot 25; dome; peak 290) and of the `Height` levelling in BuildArchipelagoMaps: each site is flattened to `Z = max(natural centre height, 1.5)` inside 0.95 R, blended out to 1.5 R, later sites override earlier.

| District | Centre | Natural Z at centre | As-is levelled Z at centre | Nearest sea (as-is) |
|---|---|---|---|---|
| Harbor | 5440,5260 | 8.8 | 8.8 (SE part pulled up to 20 by Slums blend) | 262 m W (waterline x~5185) |
| Market | 5900,5450 | 25.6 | **44.5** (Cathedral disc overwrites) | 432 m NNW |
| Cathedral | 6050,5250 | 69.4 | 69.4 | none < 520 m |
| Noble | 6250,5550 | 47.0 | 47.0 (NW rim slopes to 20) | none < 520 m |
| Slums | 5800,5000 | 53.9 | 53.9 | none < 520 m |
| Cliffside | 6150,5850 | 20.3 | 20.3 | 296 m NW |

### 2.2 Defects found

| # | Defect | Evidence |
|---|---|---|
| D1 | Plateau discs overlap at r=200 | Centre distances: Market-Cathedral 250, Noble-Cliffside 316, Cathedral-Slums 354, Cathedral-Noble 360, Market-Noble 364 (< 380 = 2 x 0.95 R). Market slope inside footprint: mean 16%, max 60%. |
| D2 | Row-order scan + cap 160 | 527 buildings per district uncapped; first 160 occupy W -158..-40 only (30%). |
| D3 | No cross-district check | Market and Cathedral grids (250 m apart, reach 180 m) overlap by 110 m and use different lattices; `Free()` is not called in the colonial branch. |
| D4 | Auto-piers | See R-05. |
| D5 | Collision boxes | Cathedral box 16 x 36 x 14 vs model 26.6 x 36, dome 31.8 (G). Mansion model carries a garden wall 8 m in front and 4 m on the sides/back, so its real lot is 30 x 28.6. |
| D6 | Harbor quay is 200 m from the Harbor centre | Waterline x 5228..5249 for y 5130..5320 (M, target terrain); the Warehouse landmark would sit 200 m inland. |
| D7 | Interiors | `Structure` carries one collision box (G). |

### 2.3 Target terraces (proposal, R-01)

Radii chosen so footprints (0.9 R) stay on flat ground. Verified on the harness: slope inside every footprint = 0.00 except Market 0.24 at the extreme SE rim (Z 30.0-30.6).

| District | R | Flat 0.95 R | **Footprint 0.9 R** | Z target | Plateau character |
|---|---|---|---|---|---|
| Harbor | 170 | 161 | 153 grid; rim and gates at 170 (+ quay band, below) | 1.6 | Matches existing pier deck 1.6 (G); quays meet cobbles. |
| Market | 110 | 104.5 | 99 | 30.0 | Natural 25.6 raised 4.4. |
| Cathedral | 105 | 99.8 | 94.5 | 58.0 | Natural 69.4 lowered so the Market stair is 28 m, not 44 m. |
| Noble | 120 | 114 | 108 | 47.0 | Natural. Walled ring at r=108. |
| Slums | 140 | 133 | 126 | 54.0 | Natural. Sits ABOVE the harbor on a shelf. |
| Cliffside | 120 | 114 | 108 | 34.0 | Natural 20.3 raised so the Faro summit tops the Governor's belvedere. |

Harbor footprint = disc r=170 (use 0.9R=153 for the grid, 170 for the edge) **union** the quay band: `x in [q(y)-30, q(y)+66], y in [5090, 5420]`, `q(y)` = waterline x (M): q(5128)=5228, q(5172)=5228, q(5216)=5237, q(5260)=5245, q(5304)=5248, q(5348)=5227, q(5392)=5215. Ground in the band is 1.6-2.4 m (blend); pier decks 1.6, so aprons ramp <= 1:10 over 6 m at pier roots.

**Footprint gaps (centre distance minus the two footprint radii; all disjoint):** H-M 228, H-S 148, M-C 56.5, M-N 157, M-S 236, M-L 265, C-S 133.5, C-N 157.5, N-L 100. Programmer: assert `gap > 40` at build time; no ownership rule is needed.

Design consequence of the real terrain: the **Slums sit on a 54 m shelf above the harbor** (a 35% scarp between them); the **Cathedral stands 28 m above the Market**; the **Cliffside lip is a 46% bluff** (34 m plateau to a 6-10 m sea shelf over ~50 m).

---

## 3. The city network

### 3.1 Schematic (north up, 1 col = 20 m, 1 row = 50 m; digits = edges E1-E9, `-` = Calle Larga, `~` = sea)

```
x 5150..6470                       y 4800..6000
5900 ~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~           *  <- Faro (6069,5918)
5850            ~~~~~~~~~~~~~~~~~~~~~~             CLIFFSIDE (6150,5850)
5750            ~~~~~~~~~~~~                       99    8   <- E8 stairs to N3
5650            ~                               99        8
5550 ~~~~~~                                  99           NOBLE (6250,5550)  G belvedere
5500 ~~~~~~~~                                  444444444   <- E4 Calle Real to N1
5450 ~~~~~~                             MARKET          77  <- E7 to N2
5400 ~~~~                         11111                77
5350 ~~~~                   111111       5   333     77    <- E3 monumental stair
5300 ~~~~~    -------------1             5
5250 ~~~~~B----  HARBOR                 5     CATHEDRAL   T towers (6065,5254)
5200 ~~~~~                              5      6
5150 ~~~~                 2222         5    666
5100 ~~~~~                   2222      5  666
5050 ~~~~~~                     2
5000 ~~~~~~~                        SLUMS X  <- chimney (5893,4997)
```
B = berth (5240,5258). Calle Larga runs due east from the berth and is the arrival vista (7.2).

### 3.2 Edges (gate = arch on each footprint rim; Z at gate; length straight line gate to gate)

| Edge | Link | From gate (x,y,Z) | To gate (x,y,Z) | Len | Rise | Grade | Type and width | Cut/fill (M) |
|---|---|---|---|---|---|---|---|---|
| E1 | Harbor-Market | 5597,5325,1.6 | 5808,5412,30 | 229 | +28.4 | 12.4% | **Avenida del Puerto** 14 m. Dogleg via 5680,5285 -> path 272 m, 10.4% | fill <= 10 |
| E2 | Harbor-Slums | 5578,5160,1.6 | 5698,5074,54 | 148 | +52.4 | 35% | **Escalera del Barrio** 4 m stairs: 328 steps, 17 flights (16 x 20 + 1 x 8), landings 3 m, 2 switchbacks, run 147 m. Foot only; horses use E5. | cut/fill <= 5.5 |
| E3 | Market-Cathedral | 5959,5371,30 | 5993,5326,58 | 57 | +28.0 | 50% | **Escalinata de la Catedral** 12 m stairs: 175 steps, 9 flights, landings 3 m + one 12 x 8 m switchback landing, run 76 m. Foot only. Axis points at the towers. | cut <= 6.3 |
| E4 | Market-Noble | 5995,5477,30 | 6146,5520,47 (N1) | 157 | +17.0 | 10.8% | **Calle Real** 14 m | cut <= 11.6 (hump), lane sunk between 4.5 m retaining walls |
| E5 | Market-Slums | 5879,5353,30 | 5827,5123,54 | 236 | +24.0 | 10.2% | Calle 9 m (carts, horses) | fill <= 7.4 |
| E6 | Cathedral-Slums | 5983,5183,58 | 5889,5089,54 | 133 | -4.0 | 3% | Calle 7 m | fill <= 5.5 |
| E7 | Cathedral-Noble | 6102,5329,58 | 6190,5460,47 (N2) | **236 path** | -11.0 | 4.7% | Calle 9 m, **re-aligned** around the ridge: 6102,5329 -> 6060,5370 -> 6080,5410 -> 6120,5450 -> 6160,5470 -> 6190,5460. Straight line needs a 19 m cut. | cut <= 6 on the re-aligned path |
| E8 | Noble-Cliffside | 6216,5652,47 (N3) | 6184,5748,34 | 100 | -13.0 | 13% | **Escaleras del Acantilado** 5 m stairs: 81 steps, 5 flights of ~16, landings 3 m, run 37 m. Beside it a carriage ramp 7 m, one hairpin, 135 m, 9.6%. | cut/fill <= 4.5 |
| E9 | Market-Cliffside | 5952,5534,30 | 6093,5758,34 | 265 | +4.0 | 1.5% | **Camino del Acantilado** 9 m, level, horse-able | fill <= 9.6 |

Within a district the radial avenue from each gate runs to the plaza (widths as above, 14 m for E1/E4, else 9 or 7).

**Connection graph:** Harbor: M(E1), S(E2 stairs). Market: H, C(E3 stairs), N(E4), S(E5), L(E9). Cathedral: M, N(E7), S(E6). Noble: M, C, L(E8 stairs). Slums: H, M, C. Cliffside: M(E9, level), N(E8, stairs). Horse route to every district exists (no district is stairs-only except via a second edge). Fastest approach to the Noble gates is E4 (main), E7 (service), E8 (back stairs, lightly guarded - infiltration).

### 3.3 Gate arches and lockdown (5 stars: district locked down, D)

- 15 seam arches (every edge end except N1/N2/N3): 8 x 2.5 x 6.5 m, opening 5.5 x 4.5, two leaf grilles that close for a 5-star lockdown; district name painted on the lintel at 0.9 m letter height; guard post 3 x 3 x 3.5 beside it; fingerpost 5 m outside. Stair edges (E2, E3, E8) have the arch at the top of the stair and a low gate (grille 3 m wide) at the bottom.
- Wall gates N1 (carriage 9 m), N2 (service 5 m), N3 (stairs 6 m): see 5.4.
- The Harbor's sea frontage cannot be locked (piers); lockdown there closes the land arches and posts Rojas's guards on the pier roots.

### 3.4 Ribbon band along avenues

Between footprints, each avenue edge carries ribbon development so the 56-265 m gaps are not empty: a TC1/TC7 house or roadside shrine every 35-60 m on alternating sides, set back 6 m, jungle behind; walls and stairs are lit at every landing.

---

## 4. Typology catalogue

F x D x H; roof extra in brackets. Decay = salt/rot overlay strength 0-1 (tint, mould line at 1.2 m, peeling plaster) `P`.

| Code | Name | F x D x H | Source | Street-level features that must read |
|---|---|---|---|---|
| TC1 | Casa urbana V0-V5 | 6/7.5/9 x 8/10 x 6/8.5 (roof +2.4) | G | Plaster white 40%, ochre 35%, blue 25%; **wooden balcony** 1.1 deep x 0.6 F wide at +3.4 m on 55% of 2-storey houses; **rejas** (wrought-iron grilles 0.9 x 1.4) on ground windows, one per 3 m of facade; arched door 1.2 x 2.6; shutters green/blue/ochre; eave 0.6. |
| TC2 | Casa de portal | 10.5 x 10 x 7 | P | **Arcade** 2.6 deep, 3.0 clear, 0.5 piers every 3.5 m on the ground floor; 3 balcony doors above; used on every plaza frontage. |
| TC3 | Casa de balcones | 8 x 10 x 9.5 (3 storeys) | P | Continuous balcony at +3.6 and +6.6, 1.3 deep, wood (Harbor, Slums edge) or wrought iron (Market, Noble); hanging signs. |
| TC4 | Casa-patio (villa) | 16 x 18 x 8 | P | Zaguan 3.2 wide with iron gate showing a 6 x 6 patio (collision box only); lot 34 x 40, wall 2.8, gate 3.5; flat azotea 50%. |
| TC5a | Almacen (Civic V2) | 14 x 30 x 8 | G | Doors on both ends (+-Y); stone footing 1.2. |
| TC5b | Tinglado (open shed) | 10 x 24 x 5 | P | 4 m pillar grid, no walls. |
| TC6 | Shanty set SH0-SH5 | 3.5x4.5x2.6; 4x5x2.8; 5x6x5.4 (jetty 0.6); 4.5x7x5.8; 6x8x3.2; **8x22x6.5 tenement** | P | Salvaged ship timber, sailcloth awnings, plank-and-tar roofs, laundry lines; Stilt V0-V5 (G, 5-7 x 6-9 x 2.6 raised 3.7) on the scarp. |
| TC7 | Ruin shells R0-R2 | 6/7.5/9 x 8 x 3.6 | P | Roofless, 60% walls, vines, rubble apron. |
| LM1 | Catedral (Civic V0) | 26.6 x 36; nave 16, aisles 10, dome 31.8, twin towers 33 | G | West front door, twin bell towers, brass finials; box fix R-08. |
| LM2 | Market hall (Civic V1) | 18 x 26 x 6 (+4) | G | Open arcade, pillar grid 4.5 x 5.2. |
| LM3 | Governor's Mansion (Civic V3) | 22 x 16 x 11, lot 30 x 28.6 | G | + belvedere 5 x 5 rising to 22 m above plateau (P). |
| LM4 | Faro | plinth 9x9x6; shaft Ø7.4->5.2 to +34; gallery Ø8.4 at +34; lantern Ø4.4 to +40 | P | See 5.6. |
| LM5 | Bounty Hunters Guild | 24 x 18 x 10 + tower 4x4 to 16 | P | Fortified casa, iron-studded door, **wanted-poster wall** (generic). |
| LM6 | Tavern "La Sirena Negra" | 18 x 16 x 9 | P | Wrap balcony, **black mermaid figurehead** on a 1.2 m bracket at +4.5, 2.4 x 1.6 painted sign. |
| LM7 | Old Library | 22 x 14 x 10 | P | High barred windows, rejas on every opening. |
| LM8 | Convento | 30 x 22 x 9 (courtyard 12 x 12) | P | Arcaded cloister, grille parlour. |
| LM9 | Slaughterhouse | 50 x 30 x 12 + chimney 2.4x2.4x24 | D/P | Soot-dark brick, cattle ramp, drainage channels. |
| LM10 | Salazar's estate | 40 x 26 x 11, lot 44 x 36 | D/P | White plaster, pale gilt ironwork, azotea with 4 watch posts. |
| LM11 | Guard barracks | 20 x 14 x 7 | P | Noble gate garrison. |
| W1 | Muro noble | 12 x 1.0 x 4.5, pilaster 0.6, finial lamp every 4th | P | Pale plaster, tile coping, iron spikes. |
| W2 | Seam arch | 8 x 2.5 x 6.5 | P | Section 3.3. |
| W3 | Gatehouse | 6 x 6 x 8 | P | Noble gates. |
| Pier V0/V1, boats V2/V3/V4 | Harbour kit | 8x26x1.6; 3x30x1.5; 2.8x10; 6.4x24 (deck 2.0); 1.6x4.4 | G | Origin = pier root, +Y to sea, bow to +Y. |

District decay: Harbor 0.8, Market 0.4, Cathedral 0.6 ("gold over rot": gilt finials intact, lower 2 m black mould), Noble 0.15, Slums 0.9, Cliffside 0.5.

### 4.1 Generator parameter block (code-ready `P`; "grid" = Block 40 x Deep 26 / Street 7 at 12 deg)

| District | Mode | Footprint (grid / rim) | Fill | Cap | Variants | KeepClear (in addition to listed structures) | Waterfront |
|---|---|---|---|---|---|---|---|
| Harbor | grid + quay band | 153 / 170 + band | 0.92 | 280 | TC1 all, TC3 20%, TC5a/b 35% west of U=-40 | disc r22 at C; 16 m rect along y=5260, x 5249-5610; band x q-30..q+66; Guild rect | on, explicit P1-P7 |
| Market | grid, ring order from r=44 | 99 | 0.92 | 100 | TC1 all, TC2 on plaza ring r 36-52, TC3 side streets | disc r34 at C; stall ring; compounds M5-M7; 5 avenue corridors (half-width 9.5 for E1/E4, 6.5 others) | off |
| Cathedral | grid, ring order | 94.5 | 0.80 | 90 | TC1 V1/V3/V5, TC3 | disc r36 at (-30,0); K1-K5 rects + 4 m | off |
| Noble | fixed lots + ring wall | 108 | - | 40 | TC4, TC2, LM3/10/11 | square, forecourts, N1 approach court | off |
| Slums | cluster, contour-aligned | 126 + scarp zone | 0.75 | 450 | TC6 SH0-SH5; Stilt V0-V5 on scarp; TC7 in B1 | placita r14; S1/S1y rects; S2 alley; S6 alley | off |
| Cliffside | cluster, contour-aligned | 108 | 0.50 | 36 | TC1 V0-V5, TC7 in B1 | plazuela r14; L1/L1k/L2 rects; lip setback 18 m; E8/E9 corridors | off (explicit cove jetty) |

Placement order inside a grid district: reserved structures first, then blocks by increasing distance from the centre, then props. Seeds as today (hash of the location id) so rebuilds are byte-identical.

---

## 5. Districts

Counts are caps for R-02. All `(U,W)` are about the district centre; world coordinates rounded to 1 m. "Door node" = interior hand-off (R-10).

### 5.1 Harbor "El Puerto" (C 5440,5260; Z 1.6)

**(1) Pattern.** Two parts: a *quay band* of piers, apron and waterfront row (rules below), and an inland **colonial grid** 40 x 26 blocks, 7 m streets, 12 deg, fill 0.92, cap 280 buildings. Scan in ring order from the plaza. Storeys: warehouses 1 (8 m), TC1 2 (6-8.5), TC3 inns 3 (9.5). Cells that fall in the 12 deg wedge between the N-S quay and the grid become **cargo yards** (open, props only). Mix: 35% TC5a/TC5b west of U=-40, 20% TC3, rest TC1.

**(2) Quay band** (world coordinates; sea to the west; piers run west; yaw 90 for pier and boat models; waterline `q(y)` in 2.3). Apron = strip from `q+3` to `q+21`, clear except props. Buildings door to the quay: yaw 270.

| ID | Structure | Size | Position | Src |
|---|---|---|---|---|
| P1 | Slipway + jetty (Shipwright) | slip 12 wide x 36 long at 1:10 | axis y=5128, root x=5231 | D/P |
| P2 | Jetty V1 + Shipwright display berth (La Liberacion later) | 3x30; berth water 14 x 34, y 5156-5169 | y=5172, root x=5231 | D/P |
| P3 | Fishing jetty V1 + V2 boats | 3x30 | y=5216, root x=5240 | P |
| P4 | **Main Docks pier V0** + **La Esperanza berth** | 8x26; berth water 12 x 30, ship centreline y=5248, bow west | y=5260, root x=5248; **arrival spawn (5240,5258)** | D |
| P5 | Rojas pier A V0 | 8x24 | y=5304, root x=5251 | D/P |
| P6 | Rojas pier B V1 | 3x30 | y=5348, root x=5230 | D/P |
| P7 | North jetty V1, small boats | 3x30 | y=5392, root x=5218 | P |
| A1 | **Rojas arena** (night boss, "multi-platform across moored ships", D) | bounds x 5200-5252, y 5300-5350 (52 x 50) | Three V3 brigs rafted, centres (5226, 5313.1/5320.5/5327.9), yaw 90, 1.2 m gaps, gangplanks 1.4 wide to P5 and between hulls; deck 2.0 vs pier 1.6 (0.4 step) | D/P |
| B1 | **Tavern La Sirena Negra** | 18x16x9, face W | (5275,5282); terrace 12x10 in front on the apron | D |
| B2 | Shipwright hall | 22x16x9, face W | (5257,5150); slip shed 12x36x8 open at (5236,5128); timber yard 40x30 at (5290,5150) | D/P |
| B3 | Chandlery (naval supply trade, D) | 10x12x6.5, face W | (5269,5238) | D/P |
| B4 | Harbor patrol post (Rojas's cover, D) | 14x12x7, face W | (5272,5326); 3 powder kegs, 2 shot stacks | D/P |
| B5 | Warehouses | TC5a 14x30x8, face W | A (5268,5194), B (5254,5370) | P |
| B6 | Dock-fight ring (D "dock fights") | Ø14, crate ring 1.0 high | (5317,5238), behind B3 | D/P |
| B7 | Dice alcoves (D "dice games") | 4 x 4 m, table Ø1.4, lamp | (q+23, pier_y+10) at P3, P4, P5, P6 | D/P |
| B8 | Cranes (SM_dock_crane) | 2 | (q+9, 5164), (q+9, 5180) | P |
| B9 | Fish stalls | 8 x SM_market_stall | x=q+8, y 5400-5420 | P |
| B10 | Sentry boxes | 2x2x3 | pier roots P2, P4, P6 at x=q+18 | P |

**Inland structures** (city frame):

| ID | Structure | Size | (U,W) -> world | Face | Src |
|---|---|---|---|---|---|
| H1 | Plaza del Puerto, disc r=22 | - | (0,0) -> 5440,5260 | - | P |
| H2 | **Bounty Hunters Guild** (faction, safehouse that clears stars, D) | 24x18x10; tower at (10,43) | (0,36) -> 5433,5295 | -w | D |
| H3 | Calle Larga (7 m, due east y=5260 from the plaza, ends in a 10 x 10 terrace with fingerpost at x=5600) | - | 5462,5260 -> 5600,5260 | - | P |

**(3) Typologies:** TC5a/b, TC3, TC1, Pier kit, LM5, LM6. **(4) Plazas:** Plaza del Puerto r=22; apron 18 x ~330 m; pier heads 8 x 8 m.

**(5) Props.** Lanterns on the apron edge every 12 m (L_harbor_lantern, G); bollards every 8 m on the quay edge, every 6 m on piers (V0 built-in); crate clusters of 2-4 every 20 m; barrels every 15 m; nets/drying racks (3 x 1.5 x 2.4, rows of 4) at P3 and P7 within 40 m; rope coils and anchors at pier roots; rubbish 1 per 25 m; laundry 1 per 40 m. Caps: props 900, lamps 70.

**(6) Gates.** E1 arch (5597,5325), E2 arch (5578,5160). Walls: none; the quay is the edge.

**(7) Sightlines.** From the berth the Calle Larga axis points at the Cathedral twin towers (825 m, bearing -0.3 deg): keep a 16 m wide corridor, nothing above 9.5 m on it, along y=5260 from x=5249 to x=5610. Guild tower (17.6 m ASL) is the local landmark. Faro (74 m) visible NE over the water. **(8) Edge.** West: sea (salt band: rust, barnacled piles, 1 rotted skiff hull per 60 m). South of y=5090: mangrove and reed marsh, ruined warehouses 40 m deep. North: shore curves east, fish stalls then ruins. East: hills, ruin band. **(9) Connections.** E1 (carriage), E2 (stairs); Calle Larga is a view line, not a road.

### 5.2 Market "El Mercado" (C 5900,5450; Z 30)

**(1) Pattern.** Radial plaza-centred: grid 40 x 26 / 7 m, 12 deg, fill 0.92, ring order from r=44, **cap 100** (expect 70-100; the gates, compounds and 5 avenues take about 4 of the 13 annulus blocks). Storeys: 2 (TC1, 6-8.5), TC2 on every plaza frontage (7 m, arcaded portales). Passes: a 3.5 m cut-through every 3rd block (pickpocket chases, D).

| ID | Structure | Size | (U,W) -> world | Face | Src |
|---|---|---|---|---|---|
| M1 | **Market hall** (LM2) | 18x26x6 | (0,0) -> 5900,5450 | -w | D/G |
| M2 | Fountain | Ø7 x 3.2, octagonal | (22,-20) -> 5926,5435 | - | P |
| M3 | Well | Ø2.4 | (-22,18) -> 5875,5463 | - | P |
| M4 | **Bounty board** (D) | 3.0 x 0.5 x 2.8 on 2 posts, shingle roof, 5 m clear radius | (-31,-9) -> 5872,5435 | -u (toward the Avenida arrival) | D |
| M5 | **Aceros del Sur** | 12x10x7; forge chimney 1.2x1.2 to 11; anvil sign | (-24,44) -> 5867,5488 | -w | D |
| M6 | **La Botica de Dona Lucia** | 9x10x6.5; green-cross sign; herb court 9x8 behind at (-7,54) | (-7,44) -> 5884,5492 | -w | D |
| M7 | **Stables** | compound 20x22, stall building 8x18x5.5 on its north edge, hitching posts every 6 m | (-52,48) -> 5839,5486 | -w | D |
| M8 | Performer stage | 6x4x0.6 | (24,22) -> 5919,5477 | - | P |

**(3) Typologies:** TC2, TC1, TC3 (side streets), LM2, awnings in five saturated colours (day colour, D).
**(4) Plazas:** Plaza del Mercado = disc r=34 at centre; stalls ring r 18-32 (outside the hall's 1 m apron). Stable lane 4 m wide from the plaza west entry to the compound gate.
**(5) Props.** SM_market_stall 3x3x3, pitch 5 m along the ring, 7 m aisle, max 64; stalls vanish at dusk (shutter props at night); lamps on the plaza ring every 10 m (8) and streets every 20 m per side; barrels/crates at stall backs every 8 m; rubbish 1 per 20 m (1 per 12 m at night); hitching posts 1 per 30 m on streets; fingerposts at all 5 gates. Caps: props 700, lamps 50.
**(6) Gates.** Five arches (rim r=99): W E1 (5808,5412); E E4 (5995,5477); NNE E9 (5952,5534); SE E3 stair (5959,5371); SSW E5 (5879,5353). **(7) Sightlines.** The E3 axis (bearing 307 deg) points at the Cathedral towers: keep a 12 m wide corridor from the plaza to the stair head with nothing above 6.5 m. The Cathedral scarp (28 m, retaining wall + gilt finials above) fills the SE skyline. Governor's belvedere is marginal from the plaza (clearance ~0). **(8) Edge.** N/NW: bay-side scrub and ruined fishermen's houses (sea 430 m); SE: retaining wall; elsewhere seam avenues. **(9) Connections.** Hub of the city: E1, E3, E4, E5, E9. Door nodes: M5, M6, M7 (stable yard, no interior).

### 5.3 Cathedral "La Catedral" (C 6050,5250; Z 58)

**(1) Pattern.** Dense, vertical, oppressive: grid 40x26 / 7 m street, only the 8.5 m variants TC1 V1/V3/V5 and TC3 (street H/W ~1.2), fill 0.8, ring order, **cap 90**. Blank upper walls toward the street, few balconies (20%), shutters closed 70% by day. Streets lit sparsely; candle-shrine niches instead (below).

| ID | Structure | Size | (U,W) -> world | Face | Src |
|---|---|---|---|---|---|
| K1 | **La Catedral de los Huesos** (LM1) | 26.6x36, towers 33, dome 31.8 | (30,0) -> 6079,5256; towers at 6063,5263 and 6067,5244 | -u (west front) | D/G |
| K2 | **Old Library** | 22x14x10 | (-30,50) -> 6010,5293 | -w | D |
| K3 | Convento | 30x22x9 | (-30,-52) -> 6031,5193 | +w | P |
| K3c | **Confessional booths** (exterior, D): 4 wooden booths 1.5x1.4x2.7, sliding grilles, under a 3 m arcade on the Convento plaza face | 4.5 m spacing, U -39..-25.5 at W=-39 | - | D/P |
| K4 | Camposanto (churchyard wall 3 m, ossuary niches 0.4x0.4 at 1.2 m, two rows) | 34x30 | (30,32) -> 6073,5288; gate 3.5 m iron at its west wall | -u | P |
| K4e | **Crypt stair entrance (Dungeon #2)** | 7x5x4 "Entrance" model | (44,44) -> 6084,5302, facing into the yard | -w | D |
| K5 | Clergy house | 14x14x8 | (30,-38) -> 6087,5219 | +w | P |
| K6 | **Sanctum (La Viuda Negra arena, D)**: interior cell, nave 12 x 30 clear, pews 8 rows, candle racks, smoke-friendly | inside K1 | door node: west portal (6064,5256) | - | D |
| K7 | Interior confessionals (north aisle, 3) | 1.4x1.2x2.6 | inside K1 | - | D/P |

**(3) Typologies:** LM1, LM7, LM8, TC1 V1/V3/V5, TC3. **(4) Plazas:** Plaza de la Catedral = disc r=36 at (-30,0) -> 6021,5244 (west of the front, 6 m clear of the facade); cloister yard 12 x 12; camposanto 34 x 30.
**(5) Props.** Candle shrines (0.8x0.4x1.2 at 1.4 m) every 30 m on walls; lamps every 26 m per side; fountain Ø5 in the cloister; benches 6 on the plaza; rubbish 1 per 60 m; no stalls. **Hidden lore tablets (D), 5 slots** (0.6 x 0.8 x 0.1 stone, content by narrative): LT-1 ossuary niche #7 in the camposanto wall; LT-2 Library attic stair landing; LT-3 behind the cloister fountain; LT-4 under the east apse buttress; LT-5 plaza paving slab at the well. Caps: props 300, lamps 45, shrines 30.
**(6) Gates.** Three arches at r=94.5: toward Market E3 (5993,5326), Slums E6 (5983,5183), Noble E7 (6102,5329). **(7) Sightlines.** The towers (91 m ASL) are the city's tallest object and are the only landmark visible from every district (7.1). Bells at fixed beats. **(8) Edge.** East, north-east, south-east: jungle hills; the camposanto wall and east apse are strangler-fig strangled (strongest "rot" of the six). West/north-west: seam avenues. **(9) Connections.** E3 (stairs, from the west), E6 (flat lane from the Slums), E7 (service road to the Noble Quarter).

### 5.4 Noble Quarter "Las Casas Altas" (C 6250,5550; Z 47)

**(1) Pattern.** Walled enclosure, **not a terrace grid**: large lots, wide streets (10 m), detached villas. Wall ring r=108, thickness 1.0, height 4.5 (W1, 12 m segments with 0.6 pilasters, finial lamp every 4th, ~53 segments). Buildings: mansion, estate, barracks, 6 villas, 3 portico houses, 2 gatehouses (**cap 40** with sheds). Storeys 2 (8 m). Lamps every 12 m on both sides; paving pale; no stalls, no rubbish.

| ID | Structure | Size | (U,W) -> world | Face | Src |
|---|---|---|---|---|---|
| N1 | **Governor's Mansion** (LM3) in a walled lot | 22x16x11 + belvedere 5x5 to 22 | (0,0) -> 6250,5550; yaw 282 | -u | D/G |
| N2 | **Duelist's Square** | plaza 44x44; duel platform 14x14x0.5 with brass inlay; 3 steps 0.3 on E/W sides; porticoes on N and S | (-54,0) -> 6197,5539 | - | D |
| N3 | **Salazar's public estate** (LM10) | 40x26x11, lot (U -76..-32, W 32..68), forecourt 15 m facing the square; azotea parapet 1.1 with 4 watch posts | (-54,50) -> 6187,5588 | -w | D |
| N4 | Barracks (LM11) | 20x14x7 | (-86,30) -> 6160,5561 | -w | P |
| N5 | Portico houses T1-T3 (TC2 on the square's south side) | 11x10x8 | (-66,-29) 6191,5508; (-54,-29) 6203,5510; (-42,-29) 6215,5513 | +w | P |
| N6 | Villas V1-V6 (TC4, lot 34x40) | 16x18x8 | V1 (48,52) 6286,5611; V2 (48,0) 6297,5560; V3 (48,-52) 6308,5509; V4 (10,-56) 6271,5497; V5 (-28,-58) 6235,5487; V6 (10,56) 6248,5607 | -w | P |
| N7 | Gatehouses flanking N1 (7.5 m either side of the gap along the wall) | 6x6x8 x2 | (-108,0) -> 6144,5527 and (-107,-15) -> 6148,5513 | - | P |

**(3) Typologies:** TC4, TC2, LM3, LM10, LM11, W1, W3. **(4) Plazas:** Duelist's Square 44 x 44; mansion forecourt; N1 approach court 31 m deep.
**(5) Props.** Fountain Ø9 x 4 in the mansion garden and one Ø5 at each end of the square; lamps every 12 m both sides; topiary/palms in tubs every 14 m; **no rubbish, no crates**; hitching rail at the gates only. Caps: props 350, lamps 80.
**(6) Walls and gates (controlled, dress code D).** Wall ring r=108 with three gates:

| Gate | Bearing | Position | Gap | Role |
|---|---|---|---|---|
| N1 | 196 deg | 6146,5520 | 9 m carriage | Main gate from Market (E4); dress-code check, 14x10 checkpoint volume outside, guards 4, gatehouses; aggro if the outfit fails. |
| N2 | 236 deg | 6190,5460 | 5 m | Service gate from Cathedral (E7); fewer guards, clergy and servants. |
| N3 | 108 deg | 6216,5652 | 6 m | Stair gate to Cliffside (E8); guards 2; the lightly guarded infiltration entry. |

Infiltration `P`: (a) disguise (D) through N1; (b) wall-climb section at bearing 20 deg (jungle side, vines, wall 4.5 m, garden wall behind is 3.2 m); (c) drain culvert at bearing 90 deg, 1.2 m high grate, 18 m crawl, from the gully; (d) roof route: Salazar's estate azotea is reachable from the barracks roof (1.8 m gap, plank). Rooftop watchers (D): patrol nodes every 28 m on the estate and barracks roofs; 4-star snipers: 4 nests.
**(7) Sightlines.** Entering N1, the Duelist's Square opens straight along the Mansion axis (bearing 192 deg): belvedere top 69 m ASL over the square. The belvedere is visible from the Market and Cliffside but not from the Cathedral plaza or the Slums placita (7.1). **(8) Edge.** East/north-east: managed forest park, jungle held back 30 m by the wall's gardeners; clean edge; breaches only at the climb points. **(9) Connections.** E4, E7, E8 (all via wall gates).

### 5.5 The Slums "El Barrio Bajo" (C 5800,5000; Z 54)

**(1) Pattern.** **Organic**, contour-aligned clusters, no grid. Alleys: Callejon 2.5 m (min clear 1.8), Pasaje 3.5, stair-alleys 1.2-2.0. Shanty set TC6; cap **380** on the plateau + **70** Stilt shanties on the scarp (Z 6-50 band between Harbor rim and plateau, inside x 5540-5720, y 5020-5240; long side on the contour). Fill 0.75. Storeys 1-2 (2.6-5.8 m), 6.5 m tenements. Unbuilt gaps: yards with fire barrels. Rubbish every 6 m, laundry lines every 12 m (spans 6-10 m at +4.5 m), 40% of lamps dark.

| ID | Structure | Size | (U,W) -> world | Face | Src |
|---|---|---|---|---|---|
| S0 | Placita (communal well Ø2.4, laundry, fire barrel, children's chalk grid) | disc r=14 | (0,0) -> 5800,5000 | - | P |
| S1 | **El Carnicero's Slaughterhouse (Dungeon #9)** hall | 50x30x12; chimney 2.4x2.4 to 24 at (90,-22) -> 5893,4997 | (78,-44) -> 5885,4973 | -u | D |
| S1y | Cattle yard (walled 3.5, gate 6 m west) | 40x30 | (43,-44) -> 5851,4966; gate (23,-44) 5832,4962 | - | P |
| S1e | Dungeon mouth: 27 m cattle ramp 5 m wide through the hill foot | "Entrance" 7x5x4 | (120,-44) -> 5927,4982, facing -u | - | D/P |
| S2 | **Black Market entrance (Dungeon #4)**: dead-end alley 3 x 14, iron door, red lantern visible down the alley at night | "Entrance" 6x5x4 | (-114,20) -> 5684,4996, facing +u | - | D |
| S3 | **Free Slaves safehouse (D)**: street trapdoor + rooftop hatch; cellar | 12x14x6 | (-32,62) -> 5756,5054 | -w | D |
| S4 | **El Medico's clinic (D)**: front waiting room on the placita; cellar arena 24 x 30 under the NE blocks (interior cell) | 10x14x6.5 | (0,24) -> 5795,5023 | -w | D |
| S5 | Hehler "El Trapero" (hidden, unlocked via Dungeon #4) | 6x6x3.5 | (52,58) -> 5839,5068 | -w | D |
| S6 | Slave-trade alley (visible at night, D): 3.5 x 30 dead end, 4 barred pens 2.4x2.4x2.4; shuttered by day | along u, W=-38, U -55..-25 -> 5754,4951 to 5783,4958 | - | D/P |
| S7 | Watch platforms (rooftop watchers): timber scaffolds 3x3 at +9 | 4 | at (-60,20), (20,-80), (70,30), (-20,-60) | - | D/P |
| S8 | Main alley "Callejon Mayor": stair top 5698,5074 -> placita -> yard gate 5832,4962 (2 bends) | 3.5 m | - | - | P |

**(3) Typologies:** TC6 SH0-SH5, Stilt V0-V5, TC7, LM9. **(4) Plazas:** Placita r=14; cattle yard 40 x 30 (open from the west); no other square.
**(5) Props.** Rubbish pile 1.5 m every 6 m; crates 1 per 25 m; laundry lines every 12 m; cages (SM_cage_small 2.4) 4 in S6, 6 in the clinic cellar; fire barrels every 25 m; wall lamp brackets at alley corners every 18 m; well on the placita. Caps: props 1,400, lamps 35.
**(6) Gates.** E2 arch at stair top (5698,5074); E5 arch (5827,5123); E6 arch (5889,5089). **(7) Sightlines.** The chimney (78 m ASL) and its plume are the landmark; visible from the placita and the stair top, not from the berth or Harbor plaza. The hillside gives the whole harbor as a vista from the stair landings. **(8) Edge.** South, east, south-east: hill jungle, strongest reclaim at the dungeon mouth; west: the scarp over the harbor (shanty cascade). **(9) Connections.** E2 (stairs from Harbor), E5 (ramp from Market), E6 (lane from Cathedral).

Child spawn volumes (children exist, D): placita, well, and stair landings; none within 30 m of S1, S4 cellar, or S6.

### 5.6 The Cliffside "El Acantilado" (C 6150,5850; Z 34)

**(1) Pattern.** Quiet **cluster**: Colonial variants (R-06), long side parallel to the lip, setback >= 18 m from it, 5 m between houses, fill 0.5, **cap 36**. Streets are paths. Storeys 1-2. Lamps every 30 m.

Cliff edge (`P`, from the levelled terrain): **lip = arc r=110 +- 2 m about C, bearings 100-235 deg** (N through W to SW). Below it a 6-10 m shelf, then sea to the west (waterline x~5805 at y=5850). Without R-04 terrain steepening the lip is a 46% bluff; dress it with cliff-face modules every 8 m and a parapet.

| ID | Structure | Size | Position (polar from C; world) | Src |
|---|---|---|---|---|
| L1 | **Old Spanish Lighthouse (Faro)** | plinth 9x9x6; shaft Ø7.4 -> 5.2 from +6 to +34; gallery Ø8.4 deck at +34 (68 m ASL) rail 1.1; lantern Ø4.4 +34..+40; cap to +42 | r 106, bearing 140 -> 6069,5918 | D/P |
| L1k | Keeper's house | 8x10x6 | 6088,5903, face +w | P |
| L2 | **Cliffside Tavern** (rooms, food, rumours; sells the first treasure map, D) | 16x14x8, terrace 14x8 on the lip side | r 88, bearing 175 -> 6062,5858, face +w | D |
| L3 | Plazuela (benches x6 around the Faro, 2 treasure-map seller stalls 3x3, D) | disc r=14 | (6075,5888) | D/P |
| L4 | Mirador parapet | 1.1 high x 0.5, 96 m long with 6 stone benches | along the lip, bearings 135-185 | P |
| L5 | **Smuggler's Cove access** | stair top (6039,5844) r 111 bearing 183, Z 34: 150 steps, 8 flights (7 x 20 + 10), landings 3 m, one switchback, run 66 m, cut into the bluff, down to the shelf (5988,5846, Z~10); shelf path 175 m west to the cove | D/P |
| L5c | Cove beach 40x25 at (5830,5852), timber jetty 2.5x12 (V1) at (5815,5852), 2 rowing boats (V4), 2 sea-cave mouths 6x5x4 at the bluff foot (5985,5832) and (5985,5870), blocked | | D/P |

**Faro climb (D "climbable"):** door at +0 locked/rotted, so the route is exterior: R0 plinth +0..6 (stone buttress, 45 deg); R1 +6..14 timber scaffold, ledge 0.9 m; R2 +14..22 broken exterior stair, three 1.5 m gaps; R3 +22..28 interior spiral (collapsed bay, 2 m jump); R4 +28..34 iron ladder to the gallery hatch. No timed jumps, no fall damage above R2 (safe-landing ledge every 8 m), assist grab. Summit = viewpoint that "reveals huge minimap area" (D); beacon at night, one 12 s sweep, no strobe.

**(3) Typologies:** TC1, TC7, LM4, LM6-type tavern, Stilt for fishermen's huts at the cove (3). **(4) Plazas:** Plazuela r=14; Faro court 20 x 20; mirador 96 x 4.
**(5) Props.** Lamps every 30 m; bollards on the cove jetty; nets 3 racks at the cove; benches 1 per 20 m on the mirador; rubbish 1 per 40 m. Caps: props 150, lamps 20.
**(6) Gates.** E9 arch (6093,5758); E8 arch with grille at the stair foot (6184,5748). **(7) Sightlines.** The Faro is visible from every district (terrain-only test, 14/14 observers) and from the sea on the arrival approach; from its gallery the whole city and the sea west are visible and the Cathedral towers stand 21 m above the gallery floor (68 m ASL vs 91 m): the Faro is the only climbable viewpoint, not the tallest object. **(8) Edge.** North and north-east: wind-bent scrub, no jungle wall; lip: salt band. **(9) Connections.** E9 (level road, horse-able), E8 (stairs from the Noble gate N3), cove stair.

---

## 6. Prop kit and spacing

Existing IDs (G, Materializer): SM_dock_crane, SM_cargo_crate, SM_oak_barrel, SM_black_powder_keg, SM_rope_coil, SM_anchor, SM_fishing_net, SM_cannonball_stack, SM_market_stall, SM_weapon_rack, SM_forge_anvil, SM_chart_table, L_harbor_lantern, L_forge_brazier; scanned Barrel_01-03, wooden_crate_01, wooden_lantern_01. `NEW` = to be authored.

| Prop | ID | Size (m) | Spacing rule |
|---|---|---|---|
| Gas lamp post | NEW SM_gas_lamp_post | Ø0.25 x 4.2, lantern 0.5 | Per side: Harbor quay 12, Harbor streets 18, Market ring 10 / streets 20, Cathedral 26, Noble 12, Slums 30 (40% dark), Cliffside 30; 0.6 m from kerb; stagger on avenues |
| Wall bracket lamp | NEW SM_gas_lamp_bracket | 0.4 out at 3.2 | Slums and passes: every 18 m at corners |
| Quay lantern | L_harbor_lantern | existing | 12 m on apron edge |
| Bollard | NEW SM_bollard | Ø0.35 x 0.6 | 8 m quay edge; 6 m on piers |
| Crates | SM_cargo_crate | 1.0 | Clusters 2-4 every 20 m (Harbor apron); Slums 1 per 25 m |
| Barrels | SM_oak_barrel / Barrel_01-03 | Ø0.7 x 0.9 | Every 15 m Harbor; stall backs every 8 m Market |
| Powder kegs, shot | SM_black_powder_keg, SM_cannonball_stack | - | Patrol post B4 and arena A1 only (3 + 2) |
| Rope, anchors | SM_rope_coil, SM_anchor | - | Pier roots; 1 per 30 m on apron |
| Nets and racks | SM_fishing_net | 2.5 x 2; racks 3x1.5x2.4 | P3 and P7 within 40 m, 1 per 6 m; cove 3 racks |
| Market stall | SM_market_stall | 3 x 3 x 3 | Pitch 5 m along ring r 18-32, aisle 7 m; max 64; hidden at night |
| Bounty board | NEW SM_bounty_board | 3.0 x 0.5 x 2.8 | One, M4 |
| Fountain / well | NEW SM_fountain, SM_well | Ø7 x 3.2 / Ø5 / Ø9 x 4 / Ø2.4 | Market 1, Cathedral cloister 1, Noble 3, Slums 1 well, Cliffside none |
| Rubbish pile | NEW SM_rubbish_pile | 1.5 | Harbor 25, Market 20 (night 12), Cathedral 60, Slums 6, Cliffside 40, Noble 0 (m between piles) |
| Laundry line | NEW SM_laundry_line | 6-10 span at +4.5 | Slums 12, Harbor 40 |
| Hitching post / trough | NEW | 3.0 | Stables 6; Market 30; Noble gates only |
| Fingerpost | NEW SM_fingerpost | 3.2 m, 3 arms, painted | Every avenue junction, 5 m outside each arch, Calle Larga terrace |
| Wall sign decal | decal | 2.4 x 0.8 | Every 40 m on avenues (street name + district) |
| Candle shrine niche | NEW | 0.8x0.4x1.2 at 1.4 | Cathedral 30; elsewhere 80 |
| Cage | NEW SM_cage_small | 2.4 cube | S6 x4; clinic cellar x6 |
| Pen / fence | NEW | 1.2 high | Paddock M7 |
| Dice table | NEW | Ø1.4 | Four alcoves B7 |
| Rooftop watcher node | marker volume, invisible | 1 x 1 | Every 28 m on frontage roofs >= 7 m in Harbor, Market, Slums, Noble estate (D: watchers at night) |

---

## 7. Sightlines and wayfinding (no markers, no highlights)

### 7.1 Landmark visibility, terrain only (M), eye = ground + 1.7

Landmark tops (ASL): Cathedral towers 91 (at 6063,5263 and 6067,5244); Faro lantern 74 (6069,5918); Governor's belvedere 69 (6250,5550); Slaughterhouse chimney 78 (5893,4997).

| Observer | Cath | Faro | Belv | Chim |
|---|---|---|---|---|
| Berth (5249,5262) | Y | Y | Y | n |
| Harbor plaza | Y | Y | Y | n |
| Harbor E gate (5597,5325) | Y | Y | n | n |
| Harbor S end | Y | Y | Y | n |
| Harbor N end | Y | Y | Y | Y |
| Market plaza | Y | Y | marginal | Y |
| Market W gate | Y | Y | Y | Y |
| Cathedral plaza | Y | Y | n | Y |
| Noble Duelist's Square | Y | Y | Y | n |
| Slums placita | Y | Y | n | Y |
| Slums stair top | Y | Y | Y | Y |
| Cliffside Tavern | Y | Y | Y | Y |
| Faro base | Y | Y | Y | Y |
| E9 midpoint | Y | Y | n | n |

Two global landmarks (Cathedral, Faro) are visible from all 14; the Governor's belvedere and the chimney are district-tier. Terrain-only results are the best case: buildings (6-9.5 m) occlude in streets, so the rules below keep the towers on axis.

### 7.2 Rules

- **VC-1 Arrival vista:** berth -> Calle Larga -> Cathedral towers, 16 m wide, nothing above 9.5 m (5.1 (7)).
- **VC-2 Market -> Cathedral:** E3 axis, 12 m wide, nothing above 6.5 m between the plaza and the stair head.
- **VC-3 Noble:** N1 axis through the Duelist's Square to the Mansion (192 deg).
- **VC-4 Slums:** Callejon Mayor from the stair top through the placita to the yard gate keeps the chimney in view.
- **Compass rule:** west is the sea, the towers are east, the Faro is north-west; any street junction on an avenue has a fingerpost; any gate has the district name painted on its lintel in letters >= 0.9 m, readable at 25 m.
- **Spoken directions:** one direction-giver (role: gatekeeper, porter, sailor) at every arch and every plaza (21 posts), giving bearings by landmark ("toward the bell towers"). No quest markers, no object highlights, no objective arrows (law). The map shows terrain and visited districts only.
- **Shop signs** (silhouette + icon + text, bracket-mounted, >= 1.2 m tall icon, readable at 25 m): anvil (Aceros), mortar and pestle (Botica), horseshoe (Stables), black mermaid (Tavern), shield and manacle (Guild), anchor (Shipwright).

---

## 8. Edge treatment (jungle reclaims)

Bands outward from each footprint rim `R_f` (0.9 R):

| Band | Range | Content |
|---|---|---|
| B1 | R_f - 20 .. R_f | 25% of cells become TC7 ruins; vine decals on facades; rubble aprons |
| B2 | R_f .. R_f + 25 | Ruins only at fill 0.5 continuing the lattice; trees breaking through roofs; plant clearing radius = R_f - 5 (R-11) |
| B3 | R_f + 25 .. + 60 | Full jungle density; 1 ruin per 40 m; avenue cuts show as sunken lanes |
| Salt band (sea sides) | within 40 m of waterline | No vegetation; rust on iron, barnacled piles, 1 rotted skiff per 60 m, driftwood |

| District | Sea / salt side | Jungle side | Managed / wall side | Seam |
|---|---|---|---|---|
| Harbor | W (quay) | S marsh/mangrove, E hills | - | E1, E2 |
| Market | N/NW bay scrub | NE/E hills | - | W, SE (retaining wall), S |
| Cathedral | - | E, SE, NE (strongest) | - | NW, W, SW, N |
| Noble | - | - | E, NE park, 30 m cleared | SW, S, N gates |
| Slums | W scarp over harbor | S, E, SE (strong) | - | W stairs, N, NE |
| Cliffside | NW to SW lip | N/NE wind scrub | - | S, SE |

---

## 9. Level document standard

**Estimated play time `P`:** story beats in the city 2-3 h; full city side content 7-9 h (the region total is 15-20 h, D). **Traversal:** berth to Cathedral plaza ~780 m straight, ~880 m by road (E1, Market, E3); berth to Cliffside plaza ~1.1 km by E1 + E9.

**Critical path (Chapter 3, D):** arrival at the berth (P4) -> La Sirena Negra (45 m; meets Diego Cruz, D) -> optional Guild (195 m) -> first Commander hunt, Captain Rojas, on P5/P6 (62 m from the tavern) -> Calle Larga / E1 -> Market (~700 m from the berth) -> E5 or E2 to the Slums -> the slave operation revealed (S6, S1). Remaining Commanders (La Viuda Negra at K6, El Medico at S4) in any order (D). All distances are straight line from the listed positions.

**Optional paths:** Faro climb and Cove; Noble infiltration (four routes, 5.4); rooftop network (planks between roofs every 2nd block, 6-10 m inner block gap bridged by clotheslines and planks, ladders every 60 m); Black Market entrance -> Dungeon #4; crypt stair -> Dungeon #2; dock fights; dice games; duels (platform N2); pickpocket missions in the Market crowd; bounty board.

**Encounter list `P` (difficulty 1-5):**

| ID | Encounter | Position | Diff | Notes |
|---|---|---|---|---|
| EN-01 | Bar fight tutorial (D) | La Sirena Negra interior | 1 | recruit drunk sailors (D) |
| EN-02 | Dock fights (D) | B6 (5317,5238) | 2 | night |
| EN-03 | Dice games (D) | B7 x4 | 1 | dusk onward |
| EN-04 | **Captain Rojas** (D) | A1 arena, night | 4 | Phase 1 six guards on the brigs; Phase 2 solo duel with pier cover |
| EN-05 | Pickpocket missions (D) | Market plaza ring | 1-2 | |
| EN-06 | Duels for honour and gold (D) | N2 platform | 2-3 | |
| EN-07 | Infiltration (D) | Noble wall | 3 | |
| EN-08 | **La Viuda Negra** (D) | K6 sanctum | 4 | smoke, ambush from behind |
| EN-09 | **El Medico** (D) | S4 cellar arena | 5 | two rooms then surgical theatre |
| EN-10 | Dungeon #2 / #4 / #9 entrances | K4e, S2, S1e | - | |
| EN-11 | Wanted snipers (D, 4 stars) | 6 Market, 4 Harbor, 4 Slums, 4 Noble nests | - | rooftops >= 7 m |

**Pacing (crowd + threat, 0-5) `P`:**

| District | Day | Dusk | Night | Dawn |
|---|---|---|---|---|
| Harbor | 4 | 4 | 3 | 1 |
| Market | 5 | 4 | 4 | 0.5 |
| Cathedral | 1 | 1 | 2 | 1 |
| Noble | 2 | 2 | 3 | 1 |
| Slums | 3 | 4 | 5 | 2 |
| Cliffside | 0.5 | 1 | 1 | 0.5 |

Lamps light at dusk, stalls close, gambling starts (D); night opens the slave-trade alley (S6) and Salazar's roof patrols; dawn is the quietest window for assassinations and escapes (D).

**Narrative beats as physical-object slots (lore through objects; content by narrative-director, not by me).** Theme tags are from the doc only.

| Slot | Place | Container | Theme |
|---|---|---|---|
| LS-01 | Quay P4 root | manifest under a crate | Armada / cargo |
| LS-02 | Guild wanted-poster wall | pinned papers | bounties |
| LS-03 | Tavern terrace | carved table | bounty hunters |
| LS-04 | Rojas pier A | ledger in the patrol post | Rojas, Salazar |
| LS-05 | Market stall back | pamphlet | rum, sugar |
| LS-06 | Botica herb court | pot | herbs, poison |
| LS-07 | Crypt stair | niche | Cathedral rot |
| LS-08 | Library attic | shelf | Free Slaves contact |
| LS-09 | Noble estate forecourt | gilt object | Salazar |
| LS-10 | Duelist's Square | platform inlay | duels |
| LS-11 | Slums S6 | pen floor | slave trade |
| LS-12 | Slaughterhouse yard | hook rack | Salazar lore |
| LS-13 | Clinic cellar | jars | El Medico |
| LS-14 | Safehouse cellar | chalk wall | Free Slaves |
| LS-15 | Faro gallery | etched rail | treasure map |
| LS-16 | Cove beach | boat | smugglers |

**Audio cues `P`** (cross-fade 6 s inside 20 m of each arch): Harbor - gulls, hull creaks, shouted cargo, tavern fiddle at dusk; Market - vihuela and voices by day, shutters at night; Cathedral - bell every beat, drone, footsteps echo; Noble - strings, gate drill; Slums - dogs, muted drums, whispers, cart wheels; Cliffside - wind, surf, Faro hum, bell buoy.

---

## 10. Budgets, accessibility, children, laws

| Item | Budget `P` |
|---|---|
| Buildings (caps) | Harbor 280, Market 100, Cathedral 90, Noble 40, Slums 450 (380 + 70 scarp), Cliffside 36: ~996 + wall 53 |
| Props (caps) | 900 + 700 + 300 + 350 + 1,400 + 150 = 3,800; lamps ~300 |
| Meshes | <= 30 distinct buildings, <= 45 props, all instanced, materials shared; props cull at 120 m, buildings at 400 m |
| Lights | Lamps are emissive + baked probe glow; real-time lights only on the nearest 8 lamps within 30 m, no shadows |
| 60 fps | Draw calls from instances only; Market night stalls hidden; NPC density by simulation LOD (12,000 NPC target, D) is systems' budget |

- **Children (D):** spawn volumes in the Market (fountain), Cathedral (cloister yard), Harbor (quay edge near P3), Slums (placita); none in arenas A1, S4, K6, within 30 m of S1/S6, or on Calle Larga at night.
- **Accessibility `P`:** alleys >= 1.8 m clear; handrails on all stairs; no timed climbs, no insta-kill falls on the Faro; icon + text on every sign; Faro beam < 1 Hz, no strobe; lamps give >= 5 lux in lit streets so night reads without colour; assist-grab on the climb.
- **Law compliance:** no markers/arrows (fingerposts and painted signs are diegetic), no highlights, lore via slot objects, navigation by landmarks/signage/spoken directions, no water fast travel, children present.

---

## 11. QA checks (for qa-tester)

1. Capsule walk E1-E9 end to end, stairs both directions; no step > 0.45; no gap < 1.8 m on mandatory paths.
2. Footprint disjointness assert (gap > 40). Building box vs corridor/plaza/keep-clear intersection = 0.
3. Z at every plateau equals the table (+-0.05); slope inside footprint < 0.03.
4. Counts within caps; no district empty in any quadrant (R-02 ring order).
5. VC-1..VC-4 corridors clear of obstructions above the stated height.
6. All 15 arches close in a 5-star lockdown; all three Noble gates reject a bad outfit.
7. Faro climb completable by assist grab on a low-skill profile; no un-recoverable fall.
8. Rojas arena: every platform reachable from P5 without a gap > 1.4 m or a rise > 0.5 m.
9. Wall-climb point, culvert and roof route each reach the Noble interior.
10. Cove stair, shelf path and jetty walkable; cave mouths blocked.

---

## 12. Concerns and open items

1. R-01 changes `radius` in Locations.json, which also drives plant clearing and tile planting; confirm with technical-artist before landing.
2. No walkable interiors (R-10): Tavern, Guild, shops, Convento, Library, sanctum and clinic need an interior streaming approach.
3. Design-doc conflicts for narrative-director: El Medico appears in the Slums (city doc) and "Rexa Black Market" (overview); Salazar's boss arena is a slaughterhouse in Chapter 4 prose but sits in the Mire's Plantation Master's Estate in the dungeon table; the first Treasure Map is sold at the Cliffside Tavern (city doc) but found in the Smuggler's Hideout (overview). The layout provides a Slums slaughterhouse dungeon and a Cliffside map seller either way.
4. Slums on a 54 m shelf is a consequence of the terrain; if art-director wants a literally low barrio, `groundZ` Slums must drop to ~14 and a 40 m bowl wall results.
5. Hill east of the Cathedral (> 100 m) is a ready backdrop for jungle; no authored content lives there in this spec.
6. Audio, art-bible palette and accessibility sign-offs are pending; all new sizes are proposals.
