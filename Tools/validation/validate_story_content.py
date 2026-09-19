#!/usr/bin/env python3
import json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
errors=[]
catalog=(ROOT/'Source/DarkArisen/Story/MainStoryMissionCatalog.cpp').read_text()
mission_ids={m for m in re.findall(r'Mission\(TEXT\("([^"]+)"\)',catalog)}
expected={
'Main.C03.01.RexaHarbor':{'Story.EthanAliveConfirmed'},
'Main.C03.02.SafeRoutes':{'Story.EthanRouteMarksFound'},
'Main.C03.03.TheFirstHolder':{'Story.FirstHolderCrossed','Chapter.03.Complete'}}
files=sorted((ROOT/'ContentSource/Story/Chapter03').glob('*.json'))
seen=set();known_types={'LocationTrigger','Evidence','Contact','RouteResolution'}
for p in files:
    try:d=json.loads(p.read_text())
    except Exception as e:errors.append(f'{p}: invalid JSON: {e}');continue
    for key in ('schemaVersion','missionId','regionId','mapId','entryAnchor','checkpointId','spawnId','anchors','actors','completionFacts','nextMission'):
        if key not in d:errors.append(f'{p}: missing {key}')
    mid=d.get('missionId');
    if mid not in mission_ids:errors.append(f'{p}: unknown missionId {mid}')
    if mid in seen:errors.append(f'{p}: duplicate mission manifest {mid}')
    seen.add(mid)
    anchors=d.get('anchors',{})
    if d.get('entryAnchor') not in anchors:errors.append(f'{p}: entryAnchor not authored')
    for aid,v in anchors.items():
        if not isinstance(v,list) or len(v)!=3 or not all(isinstance(x,(int,float)) for x in v):errors.append(f'{p}: invalid anchor {aid}')
    names=set()
    for actor in d.get('actors',[]):
        if actor.get('type') not in known_types:errors.append(f'{p}: unsupported actor type {actor.get("type")}')
        if not actor.get('name') or actor.get('name') in names:errors.append(f'{p}: actor name missing/duplicate')
        names.add(actor.get('name'))
        if actor.get('anchor') not in anchors:errors.append(f'{p}: actor {actor.get("name")} missing anchor')
    for geo in d.get('geometry',[]):
        if geo.get('anchor') not in anchors:errors.append(f'{p}: geometry {geo.get("name")} missing anchor')
        obj=geo.get('objectPath','');asset=obj.rsplit('/',1)[-1].split('.',1)[0]
        if asset and not (ROOT/'ContentSource/World/Rexa/Story'/f'{asset}.gltf').is_file():errors.append(f'{p}: source mesh missing for {asset}')
    if mid in expected and set(d.get('completionFacts',[]))!=expected[mid]:errors.append(f'{p}: completion facts drift')
if seen!=set(expected):errors.append(f'Chapter 3 manifest set mismatch: {sorted(seen)}')
holder=json.loads((ROOT/'ContentSource/Story/Chapter03/C03_03_TheFirstHolder.json').read_text())
if holder.get('bossId')!='boss.herrera':errors.append('Chapter 3 first Holder must bind boss.herrera authority')
outcomes={a.get('outcomeValue') for a in holder.get('actors',[]) if a.get('type')=='RouteResolution'}
if not {'NegotiatedPassage','AvoidedThroughOldQuarter'}<=outcomes:errors.append('TheFirstHolder must keep two physical non-final Herrera routes')
if 'StoryRoute.Main.C03.03.TheFirstHolder' not in (ROOT/'ContentSource/Story/Chapter03/C03_03_TheFirstHolder.json').read_text():errors.append('Holder route outcome persistence missing')
for forbidden in ('EthanBoss','Story.EthanBetrayal','boss.ethan'):
    if forbidden in '\n'.join(p.read_text(errors='ignore') for p in (ROOT/'ContentSource/Story').rglob('*.json')):errors.append(f'legacy Ethan token in story content: {forbidden}')
if errors:
    print('\n'.join('ERROR: '+e for e in errors));sys.exit(1)
print('Story content validation passed: Chapter 3 has 3 canonical physical materialisation contracts.')
