#!/usr/bin/env python3
"""Fail-closed complete-Alpha content audit.

This deliberately distinguishes design/source authority from playable materialisation.
It must pass before the first "complete Alpha" player link is promoted.
"""
from __future__ import annotations
import json, re, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
CS=ROOT/"ContentSource"
errors=[]

def load(rel):
 p=ROOT/rel
 if not p.is_file():
  errors.append(f"MISSING {rel}"); return {}
 try:return json.loads(p.read_text(encoding="utf-8"))
 except Exception as e:errors.append(f"INVALID {rel}: {e}");return {}

scope=load("ContentSource/ContentScale/CompleteAlphaAuthority.json")
req=scope.get("required",{})
threads=load("ContentSource/SideQuests/ThreadCatalog.json")
ts=threads.get("threads",[])
if len(ts)!=17:errors.append(f"Threads: expected 17, found {len(ts)}")
if len({x.get("id") for x in ts})!=len(ts):errors.append("Threads: duplicate stable ids")
if any(x.get("generatedOrRadiant") for x in ts):errors.append("Threads: radiant/generated content forbidden")

treasure=load("ContentSource/Treasure/TreasureAuthority.json")
st=treasure.get("stateTreasures",[])
if len(st)!=9:errors.append(f"State treasures: expected 9, found {len(st)}")
if len({x.get("id") for x in st})!=len(st):errors.append("State treasures: duplicate stable ids")
if treasure.get("networkStrengthRewardWhenReturnedAll")!=5:errors.append("State treasures: all-nine Network Strength reward must be +5")
if any(not x.get("persistent") or not x.get("duplicateProtected") for x in st):errors.append("State treasures: persistence/duplicate protection missing")

story=list((CS/"Story").glob("Chapter*/*.json"))
mission_ids=set()
for p in story:
 try:
  mid=json.loads(p.read_text()).get("missionId")
  if mid:mission_ids.add(mid)
 except Exception:pass
# Ch1-2 are native opening/Moran authorities; the canonical story validator owns their 7 missions.
if len(mission_ids)!=27:errors.append(f"Physical Chapter 3-10 contracts: expected 27, found {len(mission_ids)}")

# Full-vision playable source contracts. Design-only docs do not count here.
dungeon_contracts=list((CS/"Dungeons").rglob("*.json")) if (CS/"Dungeons").exists() else []
turn_contracts=list((CS/"SideQuests"/"Turns").rglob("*.json")) if (CS/"SideQuests"/"Turns").exists() else []
standing_contracts=list((CS/"SideQuests"/"Standing").rglob("*.json")) if (CS/"SideQuests"/"Standing").exists() else []
hoard_contracts=list((CS/"Treasure"/"Hoards").rglob("*.json")) if (CS/"Treasure"/"Hoards").exists() else []
region_contracts=list((CS/"World"/"Regions").rglob("*.json")) if (CS/"World"/"Regions").exists() else []

expected_dungeons=int(req.get("dungeons",{}).get("total",61))
if len(dungeon_contracts)!=expected_dungeons:errors.append(f"Playable dungeon contracts: expected {expected_dungeons}, found {len(dungeon_contracts)}")
# Undescribed Turns/Standing identities may be neutral-registered, but cannot be silently counted as authored playable quests.
if len(turn_contracts)>int(req.get("turns",132)):errors.append("Turn contracts exceed canonical count")
if len(standing_contracts)>int(req.get("standingVariants",147)):errors.append("Standing contracts exceed canonical count")
if len(hoard_contracts)!=int(req.get("buriedHoardChains",16)):errors.append(f"Buried-hoard contracts: expected 16, found {len(hoard_contracts)}")
if len(region_contracts)<7:errors.append(f"World region materialisation contracts: expected at least 7, found {len(region_contracts)}")

# The delivery checklist remains evidence authority. Any open item means no perfect/complete link claim.
check=(ROOT/"Docs/ALPHA_DELIVERY_CHECKLIST.md").read_text(encoding="utf-8")
open_items=len(re.findall(r"^- \[ \]",check,re.M))
if open_items:errors.append(f"Alpha delivery checklist still has {open_items} open evidence/content gates")

# Exploration persistence must use the canonical SaveGame authority.
save_h=(ROOT/"Source/DarkArisen/Persistence/DarkArisenSaveGame.h").read_text(encoding="utf-8")
ops=(ROOT/"Source/DarkArisen/Story/MainStoryAuthoredMissionOps.cpp").read_text(encoding="utf-8")
for token in ("CurrentVersion = 3","DiscoveredDungeons","CompletedDungeons","RecoveredTreasures"):
 if token not in save_h:errors.append(f"Save authority missing {token}")
for token in ("DiscoverDungeon(","CompleteDungeon(","RecoverTreasure(","Network.StateTreasuresComplete"):
 if token not in ops:errors.append(f"World persistence runtime missing {token}")

# Hero visual gate is part of the complete-content gate.
visual=ROOT/"Tools/validation/validate_visual_release.py"
if not visual.is_file():errors.append("Missing visual release validator")

print(f"Complete Alpha audit: threads={len(ts)}/17 stateTreasures={len(st)}/9 dungeons={len(dungeon_contracts)}/{expected_dungeons} hoards={len(hoard_contracts)}/16 regions={len(region_contracts)}/7 checklistOpen={open_items}")
if errors:
 print("\nCOMPLETE ALPHA BLOCKERS:")
 for e in errors:print(" - "+e)
 sys.exit(1)
print("Complete Alpha content/evidence gate passed.")
