# Higgsfield asset-production matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level:** planning / `RequirementOnly` unless real provider evidence is recorded.  
**Rule:** only current-authority, individually grounded identities enter provider production.

| Family | Grounded identities | Provider state | External media | Unreal state | Blocked / omitted |
| --- | ---: | --- | --- | --- | --- |
| Named animation requirements | 11 | Briefed | None | None | — |
| System animation requirements | 12 | Briefed | None | None | exact clip/blend tuning remains production work |
| Resolved cutscenes | 14 | Briefed | None | None | 5 final-act cutscene identities unresolved |
| Grounded named dungeons | 40 | Briefed | None | None | 20 minor identities + unresolved named Region 06 slot |
| World regions | 8 | Briefed | None | Runtime maps unauthored | no invented coordinates/locations |
| Highmoore named anchors | 12 | Briefed | None | Runtime placements unauthored | no generic replacement landmarks |
| Major character source sheets | 9 | 6 ready / 3 blocked | None | None | Elowen physical sheet; Phase-11 Ethan/Draven conflict |
| Current Tier-1 bosses — Nine Who Hold | 9 | 0 full-look ready / 9 tracked gaps | None | None | current sources lock age/role/weapon/performance/arena, not final face/body/wardrobe |
| Deep-dive boss/creature visual sheets | 21 | 19 ready / 2 blocked | None | None | legacy Ethan/Draven blocked; does not replace Nine Who Hold |
| La Liberación visual spaces | 6 | 5 ready / 1 blocked | None | None | final exterior silhouette unresolved |
| Individually grounded fauna visuals | 24 | 19 ready / 5 blocked | None | None | Final Wolf save-dependent; four Highmoore species variants underspecified |
| State Treasures | 9 | Briefed | None | Physical assets unauthored | — |
| Current grounded unique reward | 1 | Briefed | None | Physical asset unauthored | — |
| Candidate 3D meshes/rigged meshes | 0 verified connected-model paths | Blocked | None | None | no usable connected 3D generation action |
| Voice shipping assets | 0 | Late-stage blocked | auditions are not shipping evidence | None | dialogue lock + rights/cost approval required |

## Current finite provider brief total

`FExternalAssetProductionCatalog` now expects **156** source-derived Higgsfield briefs:

- 23 animation/performance requirements;
- 14 resolved cutscenes;
- 40 named dungeons;
- 8 regions;
- 12 Highmoore anchors;
- 6 current-authority character references;
- 19 non-conflicted deep-dive boss/creature references;
- 5 La Liberación deck/interior references;
- 19 fauna references;
- 9 State Treasures;
- 1 grounded unique reward.

The nine current Tier-1 commanders add **zero** provider briefs today because their complete physical looks are not authored. Tracking a gap does not inflate the provider count.

This is source coverage, not asset completion.

## Character authority

Ready: Jake plus Mira, Big Tom, Ines, Father Salvio and Esteban. Elowen is blocked because her concrete physical/costume sheet is incomplete. Ethan and Draven are blocked because `Docs/DesignAuthority.md` says their older character/boss branch conflicts with current Phase 11 and must be rewritten or explicitly restored.

## Current Tier-1 boss authority

`Docs/M7_TIER1_BOSS_REGISTER.md` and `colonial war bosses.md` define exactly the Nine Who Hold: Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft and Thorne.

All nine now have visual-readiness records in `FTier1BossVisualReadinessCatalog`. Their current sources are strong on age/role, weapon or non-combat silhouette, combat/performance language and arena. They do **not** lock complete faces, bodies, hair, eyes, skin, scars, wardrobe packages and personal visual identifiers. Therefore **0/9 full looks are provider-ready**. Higgsfield must not choose those facts on behalf of design authority.

This does not block combat/arena production briefs that can use neutral placeholders later; it blocks canonical character-look generation.

## Legacy boss / creature visual authority

The 21 older deep-dive boss visual sheets remain a separate visual-reference corpus. Nineteen are non-conflicted and may drive bounded non-canon reference work. Ethan and Draven are excluded. This legacy visual corpus never changes the current Tier-1 count or substitutes a legacy face for one of the Nine Who Hold.

## La Liberación authority

Five source-ready spaces may receive visual references: weather deck, main interior, lower crew deck, cargo/workshop and Jake's cabin. Exterior generation remains blocked because hull class, exact dimensions, mast/sail plan, colors and figurehead are not locked. Current quest law also removes old map-marker/highlight UI from the cabin/map-table interpretation.

## Fauna authority

The fauna catalog indexes 19 individually described legendary animals/creatures plus five Highmoore MVP fauna identities. **19 are provider-ready.**

Important fail-closed cases:

- `Final Wolf` stays blocked because its appearance depends on the wolves killed in the individual save;
- Highmoore grouse, hare, fox and Fell Wolf stay blocked because the higher-authority source does not choose exact species/coat variants;
- Highmoore red deer is ready only as an ordinary species/ecology reference;
- `Deep One` is ready only for a partial glimpse/shadow composition; its complete anatomy, size and limb count must remain unseen;
- Phase-4 scale targets are not permission to synthesize unnamed species.

## Explicit no-generation boundaries

Do not generate simply to close counts:

- 129 unauthored Turn identities;
- 146 unauthored Standing variants;
- 20 unauthored minor-dungeon identities;
- unresolved named Region 06 dungeon slot;
- 5 unresolved final-act cutscene identities;
- Elowen's missing visual facts;
- current Phase-11 Ethan/Draven visuals until rewritten/restored;
- all nine current Tier-1 full character looks until their physical authority is written;
- La Liberación exterior silhouette;
- save-dependent/underspecified fauna;
- any unresolved main-story trigger/actor/outcome.

Turn/Standing identity gap remains **275**.

## Provider state

Connected Higgsfield preflight on 2026-08-30 remains: Free plan, 10 credits, Unlimited false. Two Seedance 2.0 Mini test submissions were rejected before job creation with `Requires basic plan or higher`; zero credits were consumed. Connected discovery returned no usable 3D-generation action. No purchase or upgrade is authorized.

## Promotion law

`RequirementOnly` → `PrevisGenerated`/`CandidateAssetGenerated` → `ImportedForReview` → `RuntimeAccepted`.

No image/video becomes a UE asset by existing, and no source path becomes runtime acceptance without UE evidence.