# Design source register

**Review date:** 2026-08-26  
**Coverage:** 185 repository documents plus two current handoff documents. Every repository text source was fetched in full; the extensionless GDD was decoded as OOXML/DOCX, read as 263 content blocks, rendered to 39 pages, and visually reviewed page by page.

## Coverage summary

| Group | Files | Characters reviewed |
|---|---:|---:|
| `docs/characters` | 3 | 9,199 |
| `docs/design` | 26 | 516,785 |
| `docs/design/bosses` | 21 | 578,898 |
| `docs/design/colonial_war` | 8 | 301,781 |
| `docs/design/fauna` | 15 | 451,580 |
| `docs/design/mechanics` | 11 | 325,872 |
| `docs/design/npcs` | 21 | 596,634 |
| `docs/design/physics` | 15 | 319,169 |
| `Repository root` | 65 | 1,095,457 |
| **Repository total** | **185** | **4,195,375** |

## Current handoff sources

- `Docs/Handoff/ENGINEERING_HANDOFF.md` — current implementation order and twelve laws; highest repository implementation authority after Flo's live direction.
- `Docs/Handoff/PIXEL_STREAMING_SETUP.md` — M0 remote-play acceptance contract.

## Exceptions found during review

- `ANIMATIONS.md` is unrelated React/mobile incremental-game material and is quarantined.
- `Dark Arisen GDD v1.2` is a valid DOCX stored without an extension; it is historical concept context because later documents supersede core story and platform decisions.
- Malformed paths, missing cross-references, and canon conflicts are recorded in `Docs/DesignAuthority.md`.

## Full repository inventory

### docs/characters

- `docs/characters/docs/characters/docs/characters/draven_voss.md` — CAPTAIN DRAVEN VOSS — Charakter-Bibel — **legacy path/canon review required**
- `docs/characters/docs/characters/ethan_harlow.md` — ETHAN HARLOW — Charakter-Bibel — **legacy path/canon review required**
- `docs/characters/jake_harlow.md` — JAKE HARLOW — Charakter-Bibel

### docs/design

- `docs/design/colonial_war_overview.md` — COLONIAL WAR OVERVIEW — THE ARCHIPELAGO AT WAR
- `docs/design/combat_system.md` — Combat System — Technical Specification
- `docs/design/controls_ps5.md` — Controls Specification — PlayStation 5
- `docs/design/fauna_overview.md` — PHASE 4 — FAUNA & FLORA MASTER DOCUMENT
- `docs/design/lore_object_catalog.md` — DARK ARISEN — LORE OBJECT CATALOG
- `docs/design/mechanics_overview.md` — MECHANICS & FUNCTIONS — MASTER OVERVIEW
- `docs/design/naval_combat_system.md` — NAVAL COMBAT SYSTEM
- `docs/design/npcs_overview.md` — PHASE 5 — NPCs MASTER DOCUMENT
- `docs/design/physics_overview.md` — PHASE 6 — PHYSICS MASTER DOCUMENT
- `docs/design/region_01_moran.md` — Region 01 — Moran (Demo Region)
- `docs/design/region_02_rexa_city.md` — Region 02 — Rexa City (Detailed)
- `docs/design/region_02_rexa_economy.md` — Region 02 — Economy & Ship Acquisition
- `docs/design/region_02_rexa_overview.md` — Region 02 — Rexa & The Mire (Overview)
- `docs/design/region_02_rexa_swamps.md` — Region 02 — The Mire (Swamplands)
- `docs/design/region_03_ashenmoor_culture.md` — Region 03 — Ashenmoor Culture & The Anak Gunung
- `docs/design/region_03_ashenmoor_overview.md` — Region 03 — Ashenmoor (Overview)
- `docs/design/region_03_ashenmoor_settlements.md` — Region 03 — Ashenmoor Settlements (Detailed)
- `docs/design/region_03_ashenmoor_volcanoes.md` — Region 03 — Ashenmoor Volcanoes & Dungeons
- `docs/design/region_04_05_concept_pitch.md` — REGION 04 & 05 — CONCEPT PITCH
- `docs/design/region_04_quiet_coast_lighthouses.md` — REGION 04 — THE QUIET COAST: THE THREE LIGHTHOUSES
- `docs/design/region_04_quiet_coast_overview.md` — REGION 04 — THE QUIET COAST (Overview)
- `docs/design/region_04_quiet_coast_villages.md` — REGION 04 — THE QUIET COAST: VILLAGES IN DETAIL
- `docs/design/region_05_fort_carrion.md` — REGION 05 — FORT CARRION (The Fortress as Dungeon)
- `docs/design/region_05_open_sea_overview.md` — REGION 05 — THE OPEN SEA & FORT CARRION (Overview)
- `docs/design/region_06_the_maw.md` — REGION 06 — THE MAW (Post-Game Overview)
- `docs/design/style_bible.md` — DARK ARISEN — STYLE BIBLE & ANTI-SLOP FILTER

### docs/design/bosses

- `docs/design/bosses/admiral_fitzmueller.md` — BOSS SPEC — Admiral Cornelius Fitzmueller "The Professional"
- `docs/design/bosses/ashen_wyrm.md` — BOSS SPEC — The Ashen Wyrm (The Dragon)
- `docs/design/bosses/brother_cleaver.md` — BOSS SPEC — Brother Cleaver "The Redeemer"
- `docs/design/bosses/caldera_titan.md` — BOSS SPEC — The Caldera Titan (Ibu's Grief)
- `docs/design/bosses/captain_corazon.md` — BOSS SPEC — Captain Beatriz Corazón "The Iron Battery"
- `docs/design/bosses/captain_rojas.md` — BOSS SPEC — Captain Ramón Rojas "The Iron Captain"
- `docs/design/bosses/don_mateo_salazar.md` — BOSS SPEC — Don Mateo Salazar "El Carnicero" (The Butcher)
- `docs/design/bosses/draven_voss.md` — BOSS SPEC — Captain Draven Voss "The Black Tide"
- `docs/design/bosses/el_medico.md` — BOSS SPEC — El Médico "The Kind Doctor"
- `docs/design/bosses/ethan_harlow.md` — BOSS SPEC — Ethan Harlow
- `docs/design/bosses/halvard_grimm.md` — BOSS SPEC — Admiral Halvard Grimm "The Last Imperial"
- `docs/design/bosses/havfrue_modor.md` — BOSS SPEC — Havfrue Modor "The Sea-Mother"
- `docs/design/bosses/high_priest_silvano.md` — BOSS SPEC — High Priest Silvano "The Tide-Speaker"
- `docs/design/bosses/jaw_of_the_mire.md` — BOSS SPEC — Jaw of the Mire "The Swallowing King"
- `docs/design/bosses/jungle_warden.md` — BOSS SPEC — The Jungle Warden
- `docs/design/bosses/la_viuda_negra.md` — BOSS SPEC — La Viuda Negra "The Black Widow"
- `docs/design/bosses/red_lieutenant_kota_api.md` — BOSS SPEC — Red Lieutenant Kota Api "The Forge-Master"
- `docs/design/bosses/sturmkralle_sky_terror.md` — BOSS SPEC — Sturmkralle, the Sky Terror
- `docs/design/bosses/the_scarred_twins.md` — BOSS SPEC — The Scarred Twins "Kira & Mira Blackthorn"
- `docs/design/bosses/the_twin_hooks.md` — BOSS SPEC — The Twin Hooks "Castor and Pollux"
- `docs/design/bosses/ulfar_stormhand.md` — BOSS SPEC — Ulfar Stormhand "The Old Wolf of the Fjords"

### docs/design/colonial_war

- `docs/design/colonial_war/castle_ownership.md` — CASTLE OWNERSHIP — WHAT JAKE BUILDS
- `docs/design/colonial_war/castle_sieges.md` — CASTLE SIEGES — TAKING AND HOLDING
- `docs/design/colonial_war/colony_system_core.md` — COLONY SYSTEM CORE — THE MOMENT-TO-MOMENT WAR
- `docs/design/colonial_war/docs/design/colonial_war/castle_catalog.md` — CASTLE CATALOG — THE FORTRESSES OF THE ARCHIPELAGO
- `docs/design/colonial_war/imperial_colonies.md` — IMPERIAL COLONIES — THE OLD POWER'S HOLDINGS
- `docs/design/colonial_war/indigenous_liberation_alliance.md` — THE LIBERATION ALLIANCE — THE THIRD POWER
- `docs/design/colonial_war/krone_colonies.md` — KRONE COLONIES — THE COMPANY'S HOLDINGS
- `docs/design/colonial_war/retaliation_system.md` — RETALIATION SYSTEM — WHEN THE EMPIRES STRIKE BACK

### docs/design/fauna

- `docs/design/fauna/beer_and_brewing_system.md` — BEER & BREWING SYSTEM
- `docs/design/fauna/birds.md` — BIRDS
- `docs/design/fauna/cannabis_system.md` — CANNABIS SYSTEM
- `docs/design/fauna/economy_and_trade.md` — ECONOMY & TRADE — FAUNA AND FLORA COMMODITIES
- `docs/design/fauna/gathering_system.md` — GATHERING SYSTEM
- `docs/design/fauna/hunting_system.md` — HUNTING SYSTEM
- `docs/design/fauna/insects_and_small_creatures.md` — INSECTS & SMALL CREATURES
- `docs/design/fauna/land_animals.md` — LAND ANIMALS
- `docs/design/fauna/legendary_creatures.md` — LEGENDARY CREATURES — MASTER REFERENCE
- `docs/design/fauna/plants_and_herbs.md` — PLANTS & HERBS
- `docs/design/fauna/processing_and_cooking.md` — PROCESSING & COOKING SYSTEM
- `docs/design/fauna/regional_ecosystems.md` — REGIONAL ECOSYSTEMS
- `docs/design/fauna/sea_animals.md` — SEA ANIMALS
- `docs/design/fauna/tobacco_system.md` — TOBACCO SYSTEM
- `docs/design/fauna/trees_and_fruits.md` — TREES & FRUITS

### docs/design/mechanics

- `docs/design/mechanics/combat_feel.md` — COMBAT FEEL DEEP DIVE
- `docs/design/mechanics/controls.md` — CONTROLS FOUNDATION
- `docs/design/mechanics/interaction.md` — INTERACTION SYSTEM
- `docs/design/mechanics/inventory_crafting.md` — INVENTORY & CRAFTING SYSTEM
- `docs/design/mechanics/item_wheel.md` — ITEM WHEEL SYSTEM
- `docs/design/mechanics/ship_interior.md` — SHIP INTERIOR SYSTEM — LA LIBERACIÓN AS MOBILE BASE
- `docs/design/mechanics/ship_management.md` — SHIP MANAGEMENT SYSTEM
- `docs/design/mechanics/ship_navigation.md` — SHIP NAVIGATION & SAILING SYSTEM
- `docs/design/mechanics/stealth.md` — STEALTH SYSTEM
- `docs/design/mechanics/traversal.md` — TRAVERSAL SYSTEM
- `docs/design/mechanics/weapon_wheel.md` — WEAPON WHEEL SYSTEM

### docs/design/npcs

- `docs/design/npcs/apothecaries_and_alchemists.md` — APOTHECARIES AND ALCHEMISTS — THE HEALING NETWORK
- `docs/design/npcs/black_market_fences.md` — BLACK MARKET FENCES — THE SHADOW NETWORK
- `docs/design/npcs/blacksmiths_and_armorers.md` — BLACKSMITHS AND ARMORERS — THE FORGE NETWORK
- `docs/design/npcs/building_evolution.md` — BUILDING EVOLUTION SYSTEM
- `docs/design/npcs/cannabis_keeper_mama_jacinta.md` — MAMA JACINTA — THE CANNABIS KEEPER
- `docs/design/npcs/children_and_families.md` — CHILDREN AND FAMILIES — THE GENERATIONS
- `docs/design/npcs/cigar_collector_don_alejandro.md` — DON ALEJANDRO DE LA VEGA — THE CIGAR COLLECTOR
- `docs/design/npcs/desert_sage_mbah_seruni.md` — MBAH SERUNI — THE DESERT SAGE
- `docs/design/npcs/dialogue_system.md` — DIALOGUE SYSTEM
- `docs/design/npcs/general_merchants.md` — GENERAL MERCHANTS — THE ECONOMIC BACKBONE
- `docs/design/npcs/healer_mormor_astrid.md` — MORMOR ASTRID — THE FJORDLUND GRANDMOTHER
- `docs/design/npcs/imperial_contacts.md` — IMPERIAL CONTACTS — THE COMPLEX NETWORK
- `docs/design/npcs/luxury_merchants.md` — LUXURY MERCHANTS — THE REFINED NETWORK
- `docs/design/npcs/named_crew_deep_dives.md` — NAMED CREW — DEEP CHARACTER DOCUMENTS
- `docs/design/npcs/npc_recurrence_system.md` — NPC RECURRENCE SYSTEM
- `docs/design/npcs/npc_system_core.md` — NPC SYSTEM CORE — MECHANICAL FOUNDATION
- `docs/design/npcs/recurring_quest_givers.md` — RECURRING QUEST GIVERS — THE LIVING WORLD ENGINE
- `docs/design/npcs/regional_populations.md` — REGIONAL POPULATIONS — THE BREATHING MASSES
- `docs/design/npcs/shippable_specialists.md` — SHIPPABLE SPECIALISTS — RECRUITMENT-ELIGIBLE NPCs
- `docs/design/npcs/tavern_keepers.md` — TAVERN KEEPERS — THE SOCIAL HEART
- `docs/design/npcs/the_named_dead.md` — THE NAMED DEAD — THE ABSENT ARCHITECTS

### docs/design/physics

- `docs/design/physics/cloth_hair_physics.md` — CLOTH & HAIR PHYSICS — THE LIVING SURFACE
- `docs/design/physics/combat_physics.md` — COMBAT PHYSICS — IMPACT, WEIGHT, AND CONSEQUENCE
- `docs/design/physics/destruction_physics.md` — DESTRUCTION PHYSICS — THE WORLD'S WOUNDS
- `docs/design/physics/fire_physics.md` — FIRE PHYSICS — THE BURNING ELEMENT
- `docs/design/physics/lava_heat_physics.md` — LAVA & HEAT PHYSICS — THE BURNING SOUTH
- `docs/design/physics/movement_physics.md` — MOVEMENT PHYSICS — JAKE'S BODY IN THE WORLD
- `docs/design/physics/sand_desert_physics.md` — SAND & DESERT PHYSICS — THE ENDLESS WASTES
- `docs/design/physics/ship_physics.md` — SHIP PHYSICS — LA LIBERACIÓN AS LIVING VESSEL
- `docs/design/physics/snow_ice_physics.md` — SNOW & ICE PHYSICS — THE FROZEN NORTH
- `docs/design/physics/sound_physics.md` — SOUND PHYSICS — THE WORLD'S VOICE
- `docs/design/physics/sun_shadow_systems.md` — SUN & SHADOW SYSTEMS — THE LIGHT THAT REVEALS
- `docs/design/physics/vegetation_physics.md` — VEGETATION PHYSICS — THE LIVING JUNGLE
- `docs/design/physics/water_physics.md` — WATER PHYSICS — THE OCEAN AS CHARACTER
- `docs/design/physics/weather_physics.md` — WEATHER PHYSICS — THE WORLD'S EMOTIONAL STATE
- `docs/design/physics/wind_systems.md` — WIND SYSTEMS — THE INVISIBLE FORCE

### Repository root

- `animation system.md` — ANIMATION SYSTEM — WEIGHT, POSTURE, DETERIORATION
- `ANIMATIONS.md` — ANIMATIONS.md — **quarantined: foreign content**
- `army system.md` — ARMY SYSTEM — WHAT IT COSTS TO HAVE MEN
- `assassin network questline.md` — THE ASSASSIN NETWORK QUESTLINE — GOING ON OFFENCE
- `assassin network.md` — THE ASSASSIN NETWORK — THE HUNTERS
- `battle catalog.md` — BATTLE CATALOG — THE NAMED ENGAGEMENTS
- `battle system.md` — BATTLE SYSTEM — ONE MAN IN A LINE
- `camera system.md` — CAMERA SYSTEM — THE HOLD AND THE REFUSAL
- `colonial war bosses.md` — COLONIAL WAR BOSSES — THE NINE WHO HOLD
- `Colonial_War_System.md` — WAR STATE SYSTEM — THE ENGINE OF THE ARCHIPELAGO
- `crew system.md` — CREW SYSTEM — THE PEOPLE ON THE SHIP
- `crystal caves.md` — THE CRYSTAL CAVES — THE PASSAGE
- `crystal guardian.md` — THE CRYSTAL GUARDIAN — WHAT WAITS PAST THE COLUMNS
- `crystal katana.md` — THE CRYSTAL KATANA — THE SWORD THAT DOES NOT TIRE
- `cutscene catalog.md` — CUTSCENE CATALOG — THE NINETEEN, AND THE ONES THAT ARE NOT
- `Dark Arisen GDD v1.2` — DARK ARISEN — MORAN DEMO GDD v1.2 — **historical concept source**
- `dungeon design philosophy.md` — DUNGEON DESIGN PHILOSOPHY — THE PLACES UNDERNEATH
- `dungeon system global.md` — DUNGEON SYSTEM — GLOBAL
- `economy core.md` — ECONOMY CORE — THREE CURRENCIES, FOUR SINKS
- `facial animation.md` — FACIAL ANIMATION — THE REGISTER IN THE FACE
- `highmoore endgame state.md` — HIGHMOORE ENDGAME STATE — WHAT THE WORLD LEARNS
- `highmoore fauna.md` — HIGHMOORE FAUNA — WHAT LIVES ON THE MOOR
- `highmoore music.md` — HIGHMOORE — THE SCORE
- `highmoore overview.md` — HIGHMOORE OVERVIEW — THE LAND BEYOND THE MOUNTAIN
- `highmoore populations.md` — HIGHMOORE POPULATIONS — THE FIFTH REGISTER
- `highmoore reconstruction.md` — HIGHMOORE RECONSTRUCTION — WHAT COMES AFTER
- `highmoore region.md` — HIGHMOORE — THE REGION
- `highmoore settlements.md` — HIGHMOORE SETTLEMENTS — THE PLACES PEOPLE LIVE
- `highmoore smiths.md` — HIGHMOORE SMITHS — IRON, BOW, AND STONE
- `highmoore weather.md` — HIGHMOORE WEATHER — THE WIND REGION
- `house arion.md` — HOUSE ARION — WHITE AND GOLD
- `house belos.md` — HOUSE BELOS — IRON AND GREEN
- `house system.md` — THE HOUSE SYSTEM — HIGHMOORE'S POLITICS
- `interaction system.md` — INTERACTION SYSTEM — REACHING FOR THINGS
- `light elf boss.md` — THE LIGHT ELF BOSS — THE SHAFT
- `light elves.md` — THE LIGHT ELVES
- `main story structure.md` — MAIN STORY STRUCTURE — THE TEN CHAPTERS
- `mission types.md` — MISSION TYPES — THE NINE STANDING STRUCTURES
- `mounted travel.md` — MOUNTED TRAVEL — THE HORSE
- `princess quest arion falls.md` — ARION FALLS — MOVEMENT SEVEN
- `princess quest belos.md` — BELOS — MOVEMENT EIGHT
- `princess quest overview.md` — THE PRINCESS QUEST — OVERVIEW
- `princess quest the ball.md` — THE BALL — MOVEMENTS ONE TO THREE
- `princess quest the lake.md` — THE LAKE — MOVEMENTS FIVE AND SIX
- `princess quest the return.md` — THE RETURN — MOVEMENT EIGHT, CONTINUED
- `princess quest the truth.md` — THE TRUTH — WHAT WAS ACTUALLY HAPPENING
- `progression overview.md` — PROGRESSION OVERVIEW — THE THREE AXES
- `quest activation.md` — QUEST ACTIVATION — HOW A QUEST STARTS WITHOUT A MARKER
- `quest design philosophy.md` — QUEST DESIGN PHILOSOPHY — NO MARKERS, NO FAILURE SCREENS
- `README.md` — dark-arisen
- `region_04_quiet_coast_overview_part2.md` — region_04_quiet_coast_overview_part2.md — **orphaned continuation**
- `regional dungeon catalog.md` — REGIONAL DUNGEON CATALOG — THE ARCHIPELAGO'S DEPTHS
- `secret bosses.md` — SECRET BOSSES — WHAT THE NETWORKS PROTECT
- `ship as home.md` — SHIP AS HOME — LA LIBERACIÓN
- `side quest catalog.md` — SIDE QUEST CATALOG — THE SEVENTEEN THREADS
- `skill tree.md` — THE SKILL TREE — MARKS, TEACHERS, AND WHAT CANNOT BE BOUGHT
- `social system.md` — SOCIAL SYSTEM — TALKING TO PEOPLE WHO WANT NOTHING
- `technical targets.md` — TECHNICAL TARGETS — RESOLUTION, FRAMERATE, BUDGETS
- `the lesser houses.md` — THE LESSER HOUSES — THE REST OF HIGHMOORE
- `the princess.md` — ELOWEN ARION — THE PRINCESS
- `the wizard.md` — THE WIZARD — THE MAN WHO HELD THE LETTER
- `time and rest.md` — TIME AND REST — THE CLOCK AND THE SAVE
- `travel system.md` — TRAVEL SYSTEM — GETTING THERE
- `treasure system.md` — TREASURE SYSTEM — WHAT IS WORTH FINDING
- `wizard questline.md` — THE WIZARD'S QUESTLINE — THE ELEVEN DAYS

## Reading rule

This file proves inventory coverage, not that every draft is simultaneously canonical. Use `Docs/DesignAuthority.md` before implementing any item whose sources disagree.
