# Main-quest authority gap

**Status:** STOP boundary for PR #18.  
**Purpose:** identify exactly why main-story implementation must not be inferred from the current chapter scaffold.

## What is authoritative now

`main story structure.md` is a Phase-11 document and is the current structural spine. It locks:

- ten chapters;
- the chapter functions and approximate spine lengths;
- what systems/regions unlock per chapter;
- the short-critical-path doctrine;
- the chapter-gating laws;
- current high-level facts such as Chapter 1 ending with Ethan dead, Highmoore opening in Chapter 6, Rache opening in Chapter 9 and the final act opening in Chapter 10.

It explicitly says it defines **structure**, not the complete plot/missions.

## Why implementation stops here

A chapter heading is not enough to implement a main quest. A production-ready main mission needs at minimum:

1. stable mission identity;
2. exact entry trigger;
3. involved characters and physical objects;
4. location / physical route where relevant;
5. required player actions and playable beats;
6. completion condition / outcome;
7. persistence and world-state consequences;
8. presentation/control ownership where applicable.

Current authority does not resolve those fields consistently across the ten-chapter spine.

## Stop reason 1 — the current structure is a scaffold

`main story structure.md` says directly that the detailed plot of Chapters 1-3 and the endgame lives in other material. The chapter document mostly defines function, opening state and ending state.

Examples:

- Chapter 2 says Jake acquires La Liberación and Mira/Big Tom/Esteban join, but does not itself define the complete finite mission sequence that produces those events.
- Chapter 3 opens free sailing / first bosses and ends with an indication that Ethan's death was not simple, but the chapter summary is not a mission-by-mission authoring contract.
- Chapter 8 says the Crimson Armada becomes the proper endgame antagonist, but delegates the actual plot to a separate source.

Implementing quest chains by filling the space between those endpoints would be invention.

## Stop reason 2 — Ethan / Draven authority conflict

`Docs/DesignAuthority.md` explicitly records a superseded story branch:

- older GDD/character/boss material: Ethan is captured, joins Draven and becomes a Chapter-9 boss;
- current Phase-10/11 direction: Ethan is dead from the opening structure and Chapter 9 uses Ethan's Grove / Dream material.

The repository still contains detailed old Ethan/Draven boss and character documents. Their detail does **not** make them current canon.

Current authority requires one of two future operator/design actions before those materials can drive the main story:

- rewrite Ethan/Draven/final-act material to the Phase-11 structure; or
- explicitly restore the older betrayal branch.

Until then, current production blocks those legacy final-act identities.

## Stop reason 3 — missing Crimson Armada final-act authority

`main story structure.md` repeatedly points Chapters 8 and 10 to `bosses/crimson_armada.md` for the detailed endgame.

`PresentationProductionCatalog` independently records the same problem: its endgame slots 15-19 cite `cutscene catalog.md Section 3.4; main story structure.md; missing crimson_armada source` and mark identity/camera/music unresolved.

Repository search during PR #18 did not find a current matching authoritative `crimson_armada.md` document. Legacy individual boss files are not a safe replacement because of the Ethan/Draven conflict above.

## Stop reason 4 — five final-act cutscenes are unresolved

The presentation contract is exactly nineteen cutscene slots.

- #1-#14: identity resolved;
- #15-#19: final-act placeholder slots, deliberately unresolved.

This is not just missing cinematography. It is evidence that the final-act sequence identities themselves are not currently authored enough to lock the story implementation.

Higgsfield is therefore allowed to receive only the fourteen resolved cinematic briefs and cannot invent #15-#19.

## What is NOT a blocker here

The following work has already been reconciled before this stop:

- dungeons/world/travel source readiness;
- existing side-thread owners;
- finite Turn/Standing gap accounting;
- Nine Who Hold gameplay authority;
- treasure/reward authority;
- population/fauna/flora source readiness;
- La Liberación interior source readiness;
- cross-family 3D readiness;
- animation production requirements;
- resolved presentation identities;
- the Higgsfield motion/cinematic brief boundary.

Those systems can continue to receive later asset/runtime work without inventing main-story missions.

## Exact questions that must be answered before main-quest implementation

### A. Current Ethan structure

- Is Phase 11's Chapter-1 Ethan death definitively the current story?
- If yes, what replaces every later old Ethan-captured / Ethan-boss / Draven-adopted-son beat?
- What is the exact meaning/function of Ethan's Grove in Chapter 9?

### B. Crimson Armada / Draven final act

- What is the current Chapter-8 reveal sequence?
- What exact missions form Chapter 10?
- Who/what is the final human antagonist under the Phase-11 structure?
- What are the current allowed resolutions and their world-state consequences?

### C. Main-mission identities

For Chapters 1-10, define the finite mission list with stable IDs and, for each mission:

- trigger;
- participants;
- location/route;
- playable actions;
- completion/outcome;
- persistence consequence;
- cutscene/playable ownership.

### D. Final five cinematics

Resolve cutscene slots #15-#19 only after the final-act mission identities above exist.

## Do not do

Until these questions are resolved, do not:

- resurrect the legacy Ethan/Draven boss branch implicitly;
- invent a replacement final villain;
- create Chapter-10 missions from atmosphere alone;
- use Higgsfield/Kimi to generate missing final-act story;
- generate dialogue to make a missing mission feel finished;
- mark the five endgame cutscene slots resolved.

This is the intended stopping point: the pre-main production families are reconciled, and the next meaningful work requires an explicit story decision rather than more implementation.
