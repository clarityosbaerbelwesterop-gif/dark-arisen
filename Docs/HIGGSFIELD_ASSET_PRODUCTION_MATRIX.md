# Higgsfield asset-production matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level in this file:** planning / `RequirementOnly` unless an individual row is later updated with real provider evidence.  
**Rule:** count only identities already grounded by current repository authority. Missing or canon-conflicted identities stay missing/blocked.

| Family | Grounded identities | Provider brief state | External media state | Unreal state | Blocked / deliberately omitted |
| --- | ---: | --- | --- | --- | --- |
| Named animation requirements | 11 | Briefed | None claimed | None claimed | — |
| System animation requirements | 12 | Briefed | None claimed | None claimed | exact clip/blend tuning remains production work |
| Resolved cutscenes | 14 | Briefed | None claimed | None claimed | 5 final-act cutscene identities unresolved |
| Grounded named dungeons | 40 | Briefed | None claimed | None claimed | 20 minor identities + unresolved named Region 06 slot are not invented |
| World regions | 8 | Briefed | None claimed | Runtime maps remain unauthored | no coordinates/unnamed locations invented |
| Highmoore named anchors | 12 | Briefed | None claimed | Runtime placements remain unauthored | no generic lesser-House castle substitution |
| Major character source sheets | 9 | 6 provider-ready, 3 blocked | None claimed | None claimed | Elowen lacks a sufficient physical sheet; Ethan + Draven legacy character/boss branch conflicts with current Phase 11 |
| Deep-dive boss/creature visual sheets | 21 | 19 provider-ready, 2 blocked | None claimed | None claimed | legacy Ethan + Draven final-act sheets are blocked by `Docs/DesignAuthority.md`; this family does not replace the Nine Who Hold register |
| La Liberación visual spaces | 6 | 5 provider-ready, 1 blocked | None claimed | None claimed | final exterior hull class/dimensions/mast-sail plan/colors/figurehead remain unresolved |
| State Treasures | 9 | Briefed | None claimed | Physical assets/placements remain unauthored | — |
| Current grounded unique reward | 1 | Briefed | None claimed | Physical asset/import remains unauthored | — |
| Candidate 3D meshes/rigged meshes | 0 verified connected-model paths | Blocked | None claimed | None claimed | connected Higgsfield discovery returned no usable 3D model/action |
| Voice shipping assets | 0 | Late-stage blocked | auditions are not shipping evidence | None claimed | dialogue lock + cost/rights approval required |

## Current finite Higgsfield brief total

The native `FExternalAssetProductionCatalog` currently expects **137** source-derived provider briefs:

- 11 named animation requirements;
- 12 system animation requirements;
- 14 resolved cutscene identities;
- 40 grounded named dungeons;
- 8 world regions;
- 12 Highmoore named anchors;
- 6 current-authority major-character reference briefs;
- 19 non-conflicted deep-dive boss/creature visual briefs;
- 5 source-ready La Liberación deck/interior briefs;
- 9 State Treasures;
- 1 currently grounded unique reward.

This is a source-coverage count, not an asset-completion count.

## Character authority result

The first major-character pass still contains nine source records: Jake, Mira, Big Tom, Ines, Father Salvio, Esteban, Elowen, Ethan and Draven.

Provider-ready now means **current authority is non-conflicted**, not merely that an old document contains visual detail:

- Ready: Jake + the five core crew = 6.
- Blocked: Elowen, because her physical/costume sheet is insufficient for a canonical appearance.
- Blocked: Ethan and Draven, because `Docs/DesignAuthority.md` explicitly says the old character/boss branch conflicts with current Phase 11 and must be rewritten or explicitly restored before production.

## Boss / creature authority result

Twenty-one detailed Phase-1/2 visual boss sheets are indexed as historical/deep-dive visual references. Nineteen are currently eligible for bounded non-canon provider reference work where no higher authority conflicts. Ethan and Draven are excluded.

This does **not** change the authoritative boss counting/category rules:

- Tier-1 remains the Nine Who Hold from `Docs/M7_TIER1_BOSS_REGISTER.md`;
- dungeon bosses remain dungeon-category content;
- secret bosses remain secret-category content;
- creature/mythic bosses remain their own category;
- legacy visual sheets cannot silently rewrite campaign placement, outcome, boss count or final-act story authority.

## La Liberación result

Six source-backed ship visual packets exist:

- exterior hull — provider blocked;
- weather deck — provider ready;
- main deck interior — provider ready;
- lower crew deck — provider ready;
- cargo/workshop layer — provider ready;
- Jake's captain cabin — provider ready.

The ship sources lock a detailed functional interior/deck structure, but do not lock the final hull class, exact length/beam, mast count, sail plan, color scheme or figurehead identity. Higgsfield therefore may not choose those exterior canon facts.

The cabin brief also strips superseded map-marker/highlight language: current quest law forbids quest markers/objective UI.

## Explicit no-generation boundaries

These identities/facts must not be sent to a provider merely to close a count:

- 129 unauthored Turn identities;
- 146 unauthored Standing variants;
- 20 unauthored minor-dungeon identities;
- the unresolved additional named Region 06 dungeon slot;
- 5 unresolved final-act cutscene identities;
- Elowen's missing physical visual facts;
- current Phase-11 Ethan/Draven character and boss visuals until rewritten/restored;
- La Liberación's unresolved exterior silhouette facts;
- any main-story beat whose trigger/actors/outcome are not resolved by governing design.

Total deliberate Turn/Standing identity gap remains **275**.

## Provider state

Connected Higgsfield preflight on 2026-08-30:

- plan: Free;
- credits: 10;
- Unlimited: false;
- useful image/video model families are discoverable;
- Seedance 2.0 Mini test submission: `Requires basic plan or higher` before a job was created;
- two rejected test submissions consumed zero credits;
- connected 3D model discovery did not return a usable 3D generation model/action.

No purchase or plan upgrade is authorized by this matrix.

## Promotion law

A row progresses only with evidence:

`RequirementOnly` → `PrevisGenerated`/`CandidateAssetGenerated` → `ImportedForReview` → `RuntimeAccepted`.

A provider image/video cannot skip to `ImportedForReview`. A source path cannot skip to `RuntimeAccepted`. Runtime remains a later UE 5.5 gate.