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
for mission,_ in missions:
    folded=mission.casefold()
    if any(token in folded for token in ('ethanboss','ethan.boss','ethanbetrayal')): errors.append(f'legacy mission path: {mission}')
required_files=['Story/MainStorySubsystem.cpp','Persistence/DarkArisenSaveGame.h','Opening/OpeningRuntimeComponent.cpp','Opening/StoryTriggerComponent.cpp','Combat/DamagePipeline.cpp','AI/BoardingEnemyComponent.cpp','Ship/ShipVoyageComponent.cpp','Tests/MainStoryRuntimeSpec.cpp']
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
if errors:
    print('\n'.join(f'ERROR: {e}' for e in errors));sys.exit(1)
print('Alpha runtime static verification passed (34 missions; canonical facts; native story/opening/combat/ship/test sources).')
