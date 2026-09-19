# Dream Ethan — Chapter 9 Boss Contract

> **Supersession notice:** This file historically described real Ethan Harlow as a corrupted boss. That concept is superseded by `Docs/MAIN_STORY_AUTHORITY_2026_09.md`.

## Non-negotiable identity boundary

Real Ethan Harlow is alive, coerced, resistant, rescued physically in Chapter 8, and friendly afterward. He is never registered as a boss and never receives hostile combat state.

The Chapter 9 encounter is **Dream Ethan**, a memory-image produced inside Jake's internal confrontation. It is a separate gameplay and presentation identity.

- Real character id: `ethan.harlow.real`
- Dream boss id: `boss.dream_ethan`
- Real Ethan state must survive the encounter unchanged.
- Dream defeat must never set a real-Ethan defeated/dead/corrupted fact.

## Combat purpose

Dream Ethan tests Jake's fear that rescue, revenge and guilt have become indistinguishable. The encounter must still be a real Souls-like fight: readable telegraphs, deflection/posture pressure, dodge windows, damage/recovery, phase escalation and a deterministic victory state.

### Phase 1 — Familiar rhythm

Dream Ethan mirrors Harlow training rhythms: measured cuts, feints and pauses that reward patience rather than aggression.

### Phase 2 — False bearings

The arena and attack cadence deliberately misdirect Jake. Telegraphs remain fair; visual deception may change spacing and angle but never hide unavoidable damage.

### Phase 3 — Let him live

Pressure rises, but the emotional resolution is not the killing of Ethan. Victory represents Jake accepting that the real brother he rescued is not the memory-image he has been fighting in his head.

## Presentation law

Dream Ethan may echo real Ethan's face/silhouette, but requires separate material treatment, lighting, animation set and actor class/identity so the player can understand that this is a memory confrontation. Avoid generic glowing hologram language; keep it grounded in the game's dream grammar.

## Runtime requirement

Implement Dream Ethan using the existing combat/damage/posture architecture. Do not create a parallel combat system. The encounter completes `Main.C09.03.DreamFight` and advances to `Main.C09.04.WakingCourse`.
