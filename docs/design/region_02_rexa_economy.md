# Region 02 — Economy & Ship Acquisition

**Status:** Design Draft v1.0  
**Related:** GDD v1.2 §20 (Ship & Crew System), region_02_rexa_overview.md  
**Critical:** This document defines the most important transition in the game — Jake going from castaway to captain.

---

## 1. Overview

The economy of Rexa exists for one reason: to make Jake earn his first ship. Every Gold Bar, every treasure, every dungeon reward in Region 2 ladders up to a single climactic purchase at the Rexa Shipwright. Once Jake walks out with the keys to La Liberación, the entire game changes shape — the open sea becomes accessible, naval combat unlocks, and the crew system replaces the lonely Souls-Like solo loop with a living, mobile base.

This is the single biggest gameplay shift in Dark Arisen. The economy must support that weight.

---

## 2. The Two Currencies

Dark Arisen uses two parallel currencies, and Rexa is where the second one is introduced:

| Currency | Symbol | Source | Use |
|----------|--------|--------|-----|
| Doubloons | D | Enemy drops, small chests, side rewards | Stat leveling, consumables, weapon repair, small purchases |
| Gold Bars | GB | Treasure caches only (3 per region) | Major purchases: ships, ship upgrades, crew hiring |

**Why two currencies:** Doubloons are everyday spending money — they flow constantly. Gold Bars are rare, dramatic, and feel valuable. You can't grind Gold Bars from enemies. You earn them by exploring, solving treasure maps, and completing dungeon climaxes. This prevents the player from buying their ship too early and breaking the story pacing.

---

## 3. The 20 Gold Bars of Rexa

| # | Gold Bars | Source | Difficulty |
|---|-----------|--------|------------|
| 1 | 7 GB | Spanish Galleon Hoard (Sunken Ship in Mire Coast) | Medium — requires Treasure Map #1 from Cliffside Tavern |
| 2 | 8 GB | Salazar's Personal Vault (hidden in Slaughterhouse dungeon) | Hard — must clear Slaughterhouse |
| 3 | 5 GB | The Old Fort Reserve (buried in Old Spanish Fort walls) | Medium — solve fort puzzle |

**Total: 20 Gold Bars** — enough for ship + 5 crew members.

---

## 4. The Ship Purchase

### 4.1 La Liberación
- **Type:** Brigantine (mid-sized, two masts, fast and maneuverable)
- **Cost:** 15 Gold Bars
- **Available at:** Rexa Shipwright (Harbor District)
- **Requirement:** Salazar must be defeated first (story-locked, prevents premature purchase)
- **Base Stats:**
  - Hull HP: 800
  - Crew Capacity: 12 (5 starting, expandable)
  - Cannons: 6 (3 per side)
  - Sail Speed: Medium
  - Cargo Hold: 50 slots

### 4.2 Why La Liberación
The name means "The Liberation" — Jake names her himself after seeing what Salazar did to enslaved people. This is the moment Jake stops being a victim of the pirate world and starts becoming someone who could actually change it. The ship's name carries thematic weight for the rest of the game.

### 4.3 Alternative Ship Choices (Branching)
The Shipwright also offers two other ships at the same price, giving the player a build choice:

| Ship | Type | Strength | Weakness |
|------|------|----------|----------|
| La Liberación | Brigantine | Balanced (default) | Master of none |
| El Tiburón | Schooner | Fastest in the seas, evasive | Weak hull, only 4 cannons |
| El Toro | Galleon | 10 cannons, 1200 hull HP, intimidating | Slow, needs 8+ crew minimum |

Player choice locks in Jake's naval playstyle for several chapters.

---

## 5. The First Crew (5 Members)

After purchasing the ship, Jake must hire his first 5 crew members. Each costs 1 Gold Bar (paid as signing bonus). The remaining 5 GB from the 20 GB total go here.

### 5.1 The 5 Recruitable Crew Members

| # | Name | Role | Found In | Personal Story |
|---|------|------|----------|----------------|
| 1 | Mira | First Mate | Returns from Moran (story trigger) | Already bonded with Jake, becomes his second-in-command |
| 2 | Esteban "El Viejo" | Quartermaster | Cliffside Tavern, Rexa | Old former slave, knows Salazar's operation inside out |
| 3 | Ines la Calma | Navigator | Free Slaves Underground safehouse | Escaped slave with photographic memory of star charts |
| 4 | Big Tom | Gunner | Won in a brawl at La Sirena Negra | Former imperial cannoneer, deserter, comedy relief |
| 5 | Father Salvio | Surgeon | Cathedral District (defected priest) | Lost his faith watching Salazar's victims, finds purpose helping Jake |

### 5.2 Crew Recruitment Mechanic
Each crew member has a short recruitment quest (~15-30 min) that:
- Tests Jake's ability to perform their role's tasks
- Ends with a moral choice that locks in their loyalty starting value
- Costs 1 Gold Bar as their signing bonus
- Adds them to the ship roster permanently

### 5.3 Starting Loyalty Values
| Crew Member | Loyalty (0-100) | Notes |
|-------------|-----------------|-------|
| Mira | 80 | Highest — long history with Jake |
| Esteban | 60 | Pragmatic, will leave if Jake becomes cruel |
| Ines | 65 | Loyal but suspicious of pirates in general |
| Big Tom | 50 | Likes Jake but loves rum more |
| Father Salvio | 70 | Loyal to Jake's better nature |

---

## 6. Doubloons Economy in Rexa (Reference Values)

| Source | Doubloons |
|--------|-----------|
| Common enemy kill | 5-15 D |
| Elite enemy kill | 30-60 D |
| Commander kill | 200-400 D |
| Salazar boss kill | 1500 D |
| Small chest | 20-50 D |
| Medium chest | 100-200 D |
| Large chest | 300-500 D |
| Side quest reward | 100-300 D |
| Bounty Hunter contract | 150-500 D |
| Selling stolen goods (Hehler) | 60% of original value |

| Purchase | Cost |
|----------|------|
| Stat level up (early) | 100-300 D |
| Stat level up (mid) | 500-1500 D |
| Tincture (apothecary) | 80-150 D |
| Weapon repair | 50-200 D |
| Brown Bess Musket (after recipe) | 800 D |
| Tavern room (rest) | 20 D |
| Horse (basic) | 400 D |
| Horse (warhorse) | 1200 D |
| Bribe Wanted level (per star) | 200 D × stars |

---

## 7. Why This System Matters

The Rexa economy does five critical things:

1. **Earned Power:** Jake doesn't get his ship handed to him. He hunts for it, dungeon by dungeon, treasure by treasure. The ship feels like victory.

2. **Pacing Gate:** Gold Bars are gated behind dungeons and treasure maps. The player can't skip to ship ownership. This protects the story flow of Chapters 3-4.

3. **Choice Architecture:** Three ship options + five crew personalities = real player identity by the end of Region 2.

4. **Emotional Investment:** Crew members aren't shop items. They have stories, loyalty, and personal arcs. By the time Jake leaves Rexa, he isn't alone anymore.

5. **System Tutorial:** Every mechanic introduced in Rexa (treasure maps, gold bars, ship customization, crew loyalty) is the foundational system for the rest of the game's regions.

---

## 8. End-of-Region-2 State

When Jake sails out of Rexa Harbor, the game state should be:

- ✅ Salazar dead, his five Commanders dead
- ✅ La Liberación (or chosen alternative) purchased and named
- ✅ 5 crew members aboard, each with starting loyalty
- ✅ At least one mythical amulet (Swamp from Riesen-Alligator)
- ✅ At least 2-3 weapon recipes unlocked
- ✅ Treasure-hunting mechanic understood
- ✅ Open Sea unlocked for travel to Region 3 (Ashenmoor)

This is the largest single power-up the player will ever get in Dark Arisen. Region 2 is the metamorphosis chapter.
