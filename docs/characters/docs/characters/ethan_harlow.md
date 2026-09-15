# Ethan Harlow — Canon Character Sheet

> **Authority:** `Docs/MAIN_STORY_AUTHORITY_2026_09.md` supersedes older Ethan drafts.

## Identity

Ethan Harlow is Jake Harlow's younger brother and a gifted navigator. Draven Voss abducts him alive during the Harlow merchant-ship attack because his navigation skill is useful to the Crimson Armada.

Ethan is **not** a traitor, corrupted lieutenant, or real-world boss encounter. During captivity he resists Draven by introducing small false bearings, route delays, and Harlow-specific navigational errors that Jake can eventually recognize.

## Main-story state

- Opening: abducted alive by Draven.
- Chapters 3–7: remains captive while covertly corrupting Armada routes.
- Chapter 8: Jake physically reaches and rescues Ethan from the prison/navigation vessel.
- Persistent facts after rescue: `Story.EthanRecovered`, `Story.EthanNoLongerCaptive`, `Chapter.08.Complete`.
- After rescue: Ethan is friendly, injured/fatigued, and present as family/navigation support. He is never converted into a hostile gameplay actor.

## Production appearance

Young adult Harlow navigator. Lean and exhausted after captivity, but mentally sharp. He should share believable family resemblance with Jake without being a duplicate. Clothing is grounded maritime wear for the game's period: salt-stained linen, worn waistcoat/trousers/boots, captivity wear, minor healed cuts and wrist chafing. No fantasy armor and no modern objects.

The production source contract is `ContentSource/Characters/Ethan/EthanProductionManifest.json`.

## Dream Ethan separation

Chapter 9's confrontation is a **memory/dream construct**, not real Ethan. Dream Ethan must have a separate actor identity, combat registration, presentation state and boss implementation. Damage or defeat applied to Dream Ethan must never mutate the real Ethan character state.

## Superseded material

Any older document that describes real Ethan as corrupted by Draven, as a permanent Armada lieutenant, or as a physical boss is historical concept material only and must not drive runtime, content, animation, dialogue, save state, or mission logic.
