# Seventeen Threads — native source coverage

**Scope:** source/runtime ownership only  
**Branch:** `feature/content-alpha-production`  
**Alpha claim:** none. UE 5.5 compile, Automation, authored levels/assets, final dialogue/audio and playthrough evidence remain open.

The Phase-11 catalog contains exactly seventeen Threads. Every identity now has an explicit native C++ owner or existing subsystem owner. That does **not** mean every Thread is fully authored: when the design corpus stops, the implementation stops with it.

| # | Thread | Native owner | Source state |
|---|---|---|---|
| 1 | Ines and Esperanza | `UCrewPersonalThreadsComponent` | source-grounded state chain |
| 2 | Mira's coast | `UCrewPersonalThreadsComponent` | source-grounded state chain + Fjordlund world-state expiry |
| 3 | Big Tom's service | `UCrewPersonalThreadsComponent` | source-grounded state chain |
| 4 | Father Salvio's parish | `UCrewPersonalThreadsComponent` | activation only; later parish stages not authored strongly enough |
| 5 | Esteban's last chart | `UCrewPersonalThreadsComponent` | physical-chart activation only; destination/stage chain withheld |
| 6 | Assassin Network | `UAssassinNetworkThreadComponent` | two-organisation investigation/resolve owner |
| 7 | Liberation connections | `ULiberationAllianceSubsystem` | five authored connections; five Section-4.7 Full-Vision connections blocked |
| 8 | Kessler's investigation | `UArchipelagoThreadsComponent` | source-grounded investigation + exact expiry |
| 9 | Ledger Trilogy | `UArchipelagoThreadsComponent` | exact four-record chain |
| 10 | Doctor Schreiber's documentation | `UArchipelagoThreadsComponent` | ethical-documentation chain through Harbour That Was First |
| 11 | Captain Vasquez | `UArchipelagoThreadsComponent` | identity/expiry/evidence only; six-hour stage chain not authored |
| 12 | Old Fortresses | `UArchipelagoThreadsComponent` | exact four Alliance-gated physical reclamations |
| 13 | The Princess | `UPrincessQuestStateComponent` | existing M6 owner, including west/Belos/return/stair facts |
| 14 | The Light Elves | `ULightElvesThreadComponent` | four refusals, takings, practices, three sites, eleven-day gate, boss/letter |
| 15 | The Reconstruction | `UHighmooreReconstructionComponent` | standard path projects; turned-west replacement content remains a gap |
| 16 | The Named Dead | `UMemoryThreadsSubsystem` | open-ended accumulation by design; no ending/reward loop |
| 17 | Ethan | `UMemoryThreadsSubsystem` | traces + one Dream voice occurrence; structural Chapter-9 resolution without closure |

## Explicit unresolved Thread authorship gaps

Seven stable gaps remain across five Threads:

- `design-gap.thread.salvio-parish-stage-chain`
- `design-gap.thread.esteban-last-chart-destination`
- `design-gap.thread.esteban-last-chart-stage-chain`
- `design-gap.thread.alliance-five-full-vision-connections`
- `design-gap.thread.vasquez-six-hour-stage-chain`
- `design-gap.thread.reconstruction-turned-west-diplomatic-content`
- `design-gap.thread.reconstruction-mountain-supply-line`

These are not permission to invent content. They are blockers that stay visible until a newer approved design source supplies the missing facts.

## Separate finite-quest gap

This document does not alter the existing Turn/Standing register:

- 132 Turns required; 3 individually authored; 129 identities deliberately missing.
- 147 Standing variants required; 1 individually authored; 146 identities deliberately missing.
- 275 individual Turn/Standing identities therefore remain unwritten by design.

See `Docs/QUEST_CONTENT_GAPS.md`.

## What this closes and what it does not

Source ownership for all seventeen Thread identities is now explicit and testable. Production still needs the real world geometry, traversal, NPC staging, boss encounters, cinematics/playable moments, animation, subtitles, voice/audio, final data wiring and runtime evidence. A C++ state owner is the spine of the content, not the finished content itself.
