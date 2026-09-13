#!/usr/bin/env python3
"""Fail-closed source validation usable when licensed UE 5.8 is unavailable."""
from pathlib import Path
import re, sys
root=Path(__file__).resolve().parents[2]
source=root/'Source'/'DarkArisen'
errors=[]
catalog=(source/'Story/MainStoryMissionCatalog.cpp').read_text(encoding='utf-8')
missions=re.findall(r'Mission\(TEXT\("([^"]+)"\),(\d+)',catalog)
if len(missions)!=34: errors.append(f'catalog has {len(missions)} missions, expected 34')
counts=[sum(int(ch)==c for _,ch in missions) for c in range(1,11)]
if counts!=[4,3,3,3,3,3,3,3,4,5]: errors.append(f'chapter distribution is {counts}')
if missions and (missions[0][0]!='Main.C01.01.HomeWater' or missions[-1][0]!='Main.C10.05.TheWakeAfter'): errors.append('catalog endpoints changed')
authority_text=(root/'Docs/MAIN_STORY_AUTHORITY_2026_09.md').read_text(encoding='utf-8')
authority_ids=[]
for mission_id in re.findall(r'`(Main\.C\d\d\.\d\d\.[A-Za-z0-9_]+)`',authority_text):
    if mission_id not in authority_ids: authority_ids.append(mission_id)
if [m for m,_ in missions] != authority_ids: errors.append('September story authority mission IDs do not exactly match runtime catalog')
for mission,_ in missions:
    folded=mission.casefold()
    if any(token in folded for token in ('ethanboss','ethan.boss','ethanbetrayal')): errors.append(f'legacy mission path: {mission}')
required_files=['Story/MainStorySubsystem.cpp','Persistence/DarkArisenSaveGame.h','Opening/OpeningRuntimeComponent.cpp','Opening/StoryTriggerComponent.cpp','Opening/OpeningEventTriggerComponent.cpp','Story/DarkArisenWorldDirector.cpp','Combat/DamagePipeline.cpp','AI/BoardingEnemyComponent.cpp','Ship/ShipVoyageComponent.cpp','Tests/MainStoryRuntimeSpec.cpp']
for relative in required_files:
    if not (source/relative).is_file(): errors.append(f'missing native runtime file: {relative}')
all_runtime='\n'.join(p.read_text(encoding='utf-8',errors='ignore') for p in source.rglob('*') if p.suffix in {'.h','.cpp'} and 'Tests' not in p.parts)
for fact in ('Story.MarcDead','Story.DeniseDead','Story.EthanAbducted','Story.EthanRecovered','World.DriftwoodBeachReached','World.MoranOpeningRouteKnown','Ship.LaLiberacionOwned','World.RexaEntered','Story.MainComplete'):
    if fact not in all_runtime: errors.append(f'missing runtime fact: {fact}')
opening=(source/'Opening/OpeningRuntimeComponent.cpp').read_text(encoding='utf-8')
for required in ('BeginBoardingEncounter','SignalDravenBoarded','SignalTakingStarted','SignalCrewRecruitmentAvailable','SignalLaLiberacionHelmSecured','SignalLaLiberacionHarborCleared','RestoreAtCheckpoint'):
    if required not in opening: errors.append(f'missing real opening gate: {required}')
if 'NewLocation==EOpeningLocation::MirasCove)S->RecruitCrew' in opening:
    errors.append('location-only Mira auto-recruitment returned')
if 'RaidState=EOpeningRaidState::Taking' in opening.split('SignalFirstBoarderDefeated',1)[-1].split('SignalDravenBoarded',1)[0]:
    errors.append('first boarder defeat still advances directly to Taking')
if 'RequiredBoarders<2' not in opening:
    errors.append('boarding runtime no longer rejects the single-boarder shortcut')

world_director=(source/'Story/DarkArisenWorldDirector.cpp').read_text(encoding='utf-8')
opening_trigger=(source/'Opening/OpeningEventTriggerComponent.cpp').read_text(encoding='utf-8')
if 'CreateDefaultSubobject<UOpeningRuntimeComponent>' not in world_director: errors.append('opening runtime is not owned by world director')
if 'ADarkArisenWorldDirector::Resolve(this)' not in opening_trigger: errors.append('opening world trigger bypasses world director')
if 'SignalBoarderDefeated(EventId)' not in opening_trigger or 'BeginBoardingEncounter(EventId,RequiredBoarders)' not in opening_trigger: errors.append('opening world trigger cannot drive boarding encounter')
authority=(root/'Docs/DesignAuthority.md').read_text(encoding='utf-8')
for phrase in ('physically rescued in Chapter 8','real Ethan remains alive, recovered, friendly and non-hostile','LEGACY / SUPERSEDED'):
    if phrase not in authority: errors.append(f'missing current authority lock: {phrase}')

if errors:
    print('\n'.join(f'ERROR: {e}' for e in errors));sys.exit(1)
print('Alpha runtime static verification passed (34 missions; canonical facts; native story/opening/combat/ship/test sources).')
