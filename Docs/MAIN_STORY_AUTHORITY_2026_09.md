# Dark Arisen — Current Main Story Authority

**Authority date:** 2026-09-10  
**Operator direction superseded:** the Phase-11 clause that killed Ethan in Chapter 1.  
**Status:** implementation authority for the critical path.  
**Scope:** finite main-story missions, persistent facts, control ownership and chapter gates. Side content remains separate and optional.

## 1. Canon lock

The Harlow family is travelling aboard its merchant ship when Draven Voss attacks. Jake and Ethan are with their parents **Marc and Denise Harlow**. During the assault Marc and Denise are killed, **Ethan is taken alive by Draven**, and Jake is thrown or washed overboard and survives. Jake's critical-path motive is therefore not to avenge a brother already dead: it is to **find Ethan, learn why the Harlows were targeted, and stop Draven without becoming him**.

The old branch in which Ethan voluntarily joins Draven and becomes a Chapter-9 boss remains legacy material. It must not leak back into runtime behavior, dialogue, boss registration or presentation.

The ten-chapter structural shape, short critical path, no tutorial island, markerless quests, physical travel, chapter-open-not-close law, optional Colonial War/Highmoore layers, Chapter-9 Rache window and Chapter-10 convergence remain useful where they do not conflict with this authority.

## 2. Story spine

**Theme:** grief can become direction without becoming identity.  
**External question:** where is Ethan, and why did Draven keep him alive?  
**Internal question:** can Jake recover his brother without turning every relationship and every war into fuel for revenge?  
**Answer:** Ethan is not a prize at the end of the campaign. He remains a person with agency, damage and choices. Draven targeted the Harlows for their private trade-route knowledge; after Marc refused to give it up, Draven used Ethan's exceptional memory of the routes to move the Crimson Armada through waters other captains could not safely use.

No magical explanation is required. No resurrection, clone, dream reveal or fake-death retcon is permitted.

## 3. Main-story runtime laws

1. Main quests use the existing six invisible activation modes. There are no world markers, objective arrows, acceptance dialogs, countdowns or mission-failed screens.
2. Every navigation instruction must exist physically in dialogue, charts, landmarks, journals, weather, tracks or objects.
3. Failure changes the current situation where authored; it does not pop a failure screen or rewind through a mission checkpoint.
4. The player's control is removed only for explicitly authored cinematic beats. All other dramatic moments remain playable.
5. The Colonial War can alter routes, allies, resistance and the Chapter-10 battle shape, but cannot delete the critical path.
6. Highmoore stays optional. The main story never requires the Crystal Guardian, Crystal Katana or Princess line.
7. Ethan is alive from the Chapter-1 abduction until his physical recovery in Chapter 8. No older Ethan-boss content is authoritative.
8. Rache becomes available in Chapter 9, after Ethan is recovered. It is a gameplay capability, not proof that revenge is morally correct.
9. Draven Voss is the final human antagonist. The finale may change tactically with world state, but the final confrontation is authored and finite.
10. Main-story completion ends in full credits. No post-boss victory stinger or reactive music is added.

## 4. Finite mission register

Each row supplies the eight minimum production fields: stable identity, entry trigger, participants/objects, physical route, player beats, completion, persistence and control ownership.

### Chapter 1 — THE TAKING (2–3 h)

| ID | Trigger | Participants / objects | Route | Required playable beats | Completion | Persistent facts | Presentation |
|---|---|---|---|---|---|---|---|
| `Main.C01.01.HomeWater` | New game / WorldState | Jake, Ethan, Marc, Denise, family crew, cargo manifest | family merchant ship: cabin → upper deck → rigging stations | walk the ship, help crew, speak with family, handle one physical task; movement/interact learned by use | family watch begins | `Story.FamilyIntact`, `Story.EthanPresent` | playable; brief opening hold only |
| `Main.C01.02.BlackSails` | Proximity: hostile ship closes | Jake, Ethan, Marc, Denise, Draven, raiders | upper deck → damaged midships → stern | react to boarding, first real fight, lose ground, learn posture/deflection through survival, reach family | Draven reaches the Harlows | `Story.DravenIdentified` | playable combat with authored inserts |
| `Main.C01.03.TheTaking` | WorldState: family cornered | Jake, Ethan, Marc, Denise, Draven | stern / broken rail | attempt to protect family, witness Marc and Denise killed, pursue Ethan as he is seized, fail physically rather than via failure UI | Ethan leaves aboard Draven's vessel; Jake enters sea | `Story.MarcDead`, `Story.DeniseDead`, `Story.EthanAbducted`, `Story.JakeOverboard` | Cutscene #1 climax; no music until final eight seconds |
| `Main.C01.04.Undertow` | WorldState: Jake regains control in water | Jake, wreckage | open water → reef → shore | swim, manage breath/stamina, use wreckage and current, crawl ashore, find one physical sign proving the raiders carried prisoners rather than bodies | Jake reaches shelter and writes first notebook entry | `Story.JakeSurvived`, `Story.EthanAliveLead`, `Chapter.01.Complete` | fully playable after short wake hold |

### Chapter 2 — LA LIBERACIÓN (3–4 h)

| ID | Trigger | Participants / objects | Route | Required playable beats | Completion | Persistent facts | Presentation |
|---|---|---|---|---|---|---|---|
| `Main.C02.01.NoCoinNoFlag` | Conversation after recovery | Jake, Mira, Big Tom, Esteban | shore settlement → docks → tavern/storehouse | earn trust through physical work and local information, show Draven insignia, establish a route lead | Esteban identifies a port used by Draven's network | `Crew.MiraKnown`, `Crew.BigTomKnown`, `Crew.EstebanKnown` | playable dialogue and work |
| `Main.C02.02.AShipToTake` | Object: impound/prize papers | Jake, Mira, Big Tom, Esteban, port authority, ship | docks → impound yard → La Liberación | inspect ship, choose a non-teleport physical entry, free the vessel during a local break in authority, get the hull moving with crew rather than solo magic | La Liberación clears the harbor | `Ship.LaLiberacionOwned`, `Crew.CoreThreeJoined` | playable escape; no montage travel |
| `Main.C02.03.FirstWake` | WorldState: ship under way | Jake, core crew, charts | harbor → open sea → first regional port | crew before command, sail by wind/chart/landmark, weather one navigation problem | first Draven-linked port reached | `Chapter.02.Complete`, `Travel.FreeSailingSeeded` | playable voyage; one slow push allowed |

### Chapter 3 — THE ARCHIPELAGO (4–5 h)

| ID | Trigger | Conversation / Object | Draven broker, dock ledger, survivor | first port → warehouse quarter | locate a broker without marker UI, inspect cargo records, distinguish false rumor from physical ledger trail | a dated transfer places Ethan alive aboard an Armada tender | `Story.EthanAliveConfirmed` | playable investigation |
| `Main.C03.02.RedWake` | WorldState after ledger | Jake, crew, Armada tender | port → physical sea route → wreck/cove | sail pursuit, board or investigate aftermath, recover Ethan's own navigational shorthand | shorthand authenticated by Jake | `Story.EthanRouteMarksFound` | playable boarding/investigation |
| `Main.C03.03.TheFirstHolder` | Proximity / WorldState | Jake, region authority, first Tier-1 route gate | authored regional route | break through, negotiate around, or defeat the region's first real power obstacle; no level gate | access to wider three-region network | `Story.FirstHolderCrossed`, `Chapter.03.Complete` | boss/encounter stays gameplay-first |

### Chapter 4 — THE WAR (3–4 h spine)

| ID | Trigger | Participants / objects | Route | Required playable beats | Completion | Persistent facts | Presentation |
|---|---|---|---|---|---|---|---|
| `Main.C04.01.ClosedHarbor` | WorldState | Jake, crew, Imperial/Albion/Liberation actors | sea route → blockaded harbor | discover that the next Ethan lead is physically cut off by the colonial conflict | player reaches contested shore by authored means | `War.VisibleToJake` | playable |
| `Main.C04.02.ThreeVerbs` | Conversation | Jake, local faction actors | contested settlement / fort approaches | complete one authored Support, Break or Liberate action; abstention remains valid but harder | route changes enough to continue pursuit | `War.Playable`, selected qualitative war fact | playable systemic mission |
| `Main.C04.03.GlassCompass` | Object | Jake, confiscated Harlow chart, Draven courier remnants | fort/archive → ship | recover a damaged family chart whose missing annotations match Ethan's shorthand | next courier lane reconstructed | `Story.HarlowChartRecovered`, `Chapter.04.Complete` | physical examine; no exposition card |

### Chapter 5 — THE LETTER (3–4 h spine)

| ID | Trigger | Object: sealed letter | Jake, crew, assassin observer | La Liberación / port | first letter arrives naming Jake and referring to Ethan only as "the navigator" | letter authenticated | `Story.NavigatorAliasKnown` | authored letter insert |
| `Main.C05.02.HuntersKnow` | Proximity / Absence | Jake, assassin network, missing contact | settlement → back route → safe room | recognize surveillance, survive/evade a coordinated hunt, find the cost of being known | assassin network exposed | `Assassins.Unlocked` | gameplay; no combat stinger |
| `Main.C05.03.FourWaysToReadASea` | Conversation | Jake, four sages as independently reachable knowledge sources | player-chosen physical routes | obtain enough non-magical environmental/navigation interpretation to decode Draven's deep-water routing | Armada deep-water corridor identified | `Story.DeepRouteKnown`, `Chapter.05.Complete` | player-paced conversations |

### Chapter 6 — THE HOLDING (4–5 h spine)

| ID | Trigger | WorldState | Jake, crew, potential holding | contested holding route | secure or receive a first defensible base through current colonial-war rules | one holding becomes functional | `Holding.FirstOwned` | playable siege/negotiation |
| `Main.C06.02.SomethingThatCanBurn` | WorldState | Jake, residents, construction/records | holding interior and approaches | physically restore minimum functions, choose what to protect first, establish archive/intelligence room | base can support pursuit | `Holding.IntelligenceReady` | playable; no management overlay spectacle |
| `Main.C06.03.EthansHand` | Object | Jake, Ethan's encoded route copy | intelligence room → chart table | compare the copy with childhood/family chart habits; realize Ethan is deliberately leaving tiny errors Draven will not notice | Jake proves Ethan is resisting, not serving willingly | `Story.EthanResisting`, `Chapter.06.Complete` | intimate examine / slow push |

### Chapter 7 — THE COST (5–6 h spine)

| ID | Trigger | WorldState: retaliation | Jake, crew, holding residents, attackers | holding / nearby route | respond to attack without a failure screen; loss may persist | assault resolves | `Story.RetaliationPaid`, authored damage/loss facts | fully playable battle |
| `Main.C07.02.NoCleanLedger` | Object / Overheard | Jake, Draven quartermaster or lieutenant, prisoner ledger | hostile supply route → camp/ship | intercept convoy, infiltrate or fight, seize prisoner-transfer ledger | Ethan's current vessel and purpose confirmed | `Story.EthanUsedAsNavigator` | gameplay-first |
| `Main.C07.03.ThePriceOfFollowing` | Conversation | Jake, core crew | Great Cabin | crew confronts Jake with accumulated costs; player listens/interjects but receives no affinity meter | Jake commits to a rescue plan rather than an indiscriminate attack | `Story.RescueBeforeRevenge`, `Chapter.07.Complete` | anchored conversation; player look retained where possible |

### Chapter 8 — THE TURN (5–6 h spine)

| ID | Trigger | WorldState | Jake, crew, Crimson Armada scouts | open sea / occupied port | first undeniable Armada appearance; identify prison/navigation ship | target physically located | `Armada.Revealed` | gameplay plus authored wide |
| `Main.C08.02.ThePrisonCourse` | Object / Conversation | Jake, rescued prisoner, Ethan's false bearings | port → sea corridor | follow Ethan's intentionally imperfect route marks while avoiding the trap intended for Draven's enemies | prison ship intercepted | `Story.EthanSignalUnderstood` | playable navigation |
| `Main.C08.03.Brothers` | Proximity | Jake, Ethan, guards, captives | prison/navigation ship decks → hold → escape route | reach Ethan, free captives, fight/escape together; Ethan is capable but visibly changed; no boss fight between brothers | Ethan boards La Liberación alive | `Story.EthanRecovered`, `Story.EthanNoLongerCaptive`, `Chapter.08.Complete` | reunion is brief authored takeover then playable escape |

### Chapter 9 — RACHE (6–7 h spine)

| ID | Trigger | Conversation | Jake, Ethan, core crew | La Liberación / safe shore | hear Ethan's account: Marc refused Draven access to private Harlow safe-route knowledge; Draven killed the parents and kept Ethan because he remembered the routes | motive established without supernatural retcon | `Story.DravenMotiveKnown` | player-paced cabin scene |
| `Main.C09.02.EthansGrove` | Proximity / Object | Jake, Ethan, family objects | Ethan's Grove | walk the place physically, inspect memories, allow silence; no enemy wave | Jake reaches the remembered clearing | `Story.GroveVisited` | authored camera #4 boundaries, movement preserved |
| `Main.C09.03.TheDreamFight` | WorldState after rest/plant-system authored gate | Jake, memory-image Ethan | dream space grounded in existing presentation rules | fight the remembered healthy Ethan as an internal test; the point is not victory over Ethan but recognition of Jake's obsession | dream resolves without declaring real Ethan defeated | `Story.DreamResolved` | cutscene/gameplay hybrid #5 |
| `Main.C09.04.WhatRageIsFor` | WorldState | Jake, Ethan | dawn after Grove → ship | Rache capability becomes available; Jake chooses to pursue Draven with a concrete Armada objective | final route locked | `Combat.RacheUnlocked`, `Story.FinalRouteKnown`, `Chapter.09.Complete` | playable departure |

### Chapter 10 — THE ARMADA (8–10 h spine)

| ID | Trigger | WorldState | Jake, Ethan, crew, war-state allies/enemies | archipelago → Armada perimeter | translate the five colonial-war end states into different access, resistance and reinforcement conditions | Armada perimeter breached | `Finale.EntryStateLocked` | large-scale gameplay |
| `Main.C10.02.BreakTheChain` | WorldState | Jake, crew, captives, Armada logistics | outer fleet → supply ships / batteries | destroy or seize the physical systems that let Draven hold the fleet together; route depends on world state | fleet cohesion broken | `Armada.LogisticsBroken` | playable multi-segment battle |
| `Main.C10.03.TheBlackDeck` | Proximity | Jake, Ethan, Draven, Draven guard | flagship exterior → command deck | board flagship physically; Ethan helps only where his story state permits; no teleport to boss arena | Jake reaches Draven | `Finale.DravenReached` | gameplay transition into confrontation |
| `Main.C10.04.DravenVoss` | WorldState | Jake, Draven, Ethan as protected narrative participant | flagship command deck / damaged stern | authored final human duel; defeat comes through existing combat/posture laws; no QTE finisher required | Draven is decisively neutralized by an authored physical outcome | `Story.DravenDefeated` plus `Draven.Killed` or `Draven.Captured` | boss fight remains player-controlled; short ending takeover only |
| `Main.C10.05.TheWakeAfter` | WorldState: Draven resolved | Jake, Ethan, surviving crew/world-state representatives | flagship aftermath → La Liberación → open sea | leave battlefield, account for visible consequences, share final brother scene, return control for final walk/deck moment | credits trigger from physical end-state | `Story.MainComplete`, `Chapter.10.Complete` | no reactive victory music; full credits follow |

**Total finite main-story missions: 34.** No main-story mission generator exists.

## 5. Character arc locks

### Jake

Jake begins in the shadow of Ethan and ends without needing either Draven or Ethan to define him. Rache is a capability he can use, not the emotional answer to the story. The final chapter should make the player feel changed, not triumphant.

### Ethan

Ethan is abducted, coerced and used for his remembered trade-route knowledge. He does not secretly choose Draven, does not become Draven's adopted son, and is not a Chapter-9 boss. His resistance is quiet: small route errors, recoverable signs, and preserving other captives where he can. After rescue, he is not instantly restored to his pre-raid personality.

### Draven Voss

Draven is not motivated by prophecy or magic. He wanted a route network the Harlows would not sell him. Marc's refusal turns a calculated seizure into murder; keeping Ethan alive is practical exploitation. This makes the central evil intimate and systemic at once: people become routes, leverage and inventory to him.

## 6. Presentation / Higgsfield boundary

Higgsfield may create **previsualization and motion reference**, not shipping canon by itself. Current priority order:

1. Chapter-1 family-deck body performance and raid onset;
2. Marc/Denise death + Ethan abduction staging without gore spectacle;
3. Jake overboard / underwater survival motion;
4. Chapter-8 brother reunion and joint escape body language;
5. Chapter-9 Grove stillness / Dream Fight motion vocabulary;
6. Chapter-10 Draven confrontation and aftermath.

Generated clips are reference media until retargeted/rebuilt, reviewed and accepted in Unreal Sequencer/animation assets.

## 7. Definition of story-complete

The story source layer is complete when all 34 IDs exist in native runtime authority, chapter order is fail-closed, every mission has its authored activation/outcome/persistence contract, the old Ethan-boss branch is rejected by validation, the five final-act presentation slots are re-bound to the current Chapter-10 sequence, and a start-to-credits playthrough can be executed on one exact build without invented filler or a mission generator.
