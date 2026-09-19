#!/usr/bin/env python3
"""Fail-closed source validation usable when licensed UE 5.8 is unavailable."""
from pathlib import Path
import json, re, sys

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

required_files=[
    'Story/MainStorySubsystem.cpp','Persistence/DarkArisenSaveGame.h','Opening/OpeningRuntimeComponent.cpp',
    'Opening/StoryTriggerComponent.cpp','Opening/OpeningEventTriggerComponent.cpp','Story/DarkArisenWorldDirector.cpp',
    'Story/MainStoryMapTransitionActor.cpp','Story/MainStoryMapCatalog.cpp','Story/CreditsPresentationActor.cpp',
    'Story/RacheStoryUnlockComponent.cpp','Characters/EthanHarlowCharacter.cpp','Combat/DamagePipeline.cpp',
    'AI/BoardingEnemyComponent.cpp','Ship/ShipVoyageComponent.cpp','Ship/NavalCombatComponent.cpp','Ship/HostileNavalShip.cpp',
    'Story/NavalEncounterGateActor.cpp','UI/AlphaMenuPlayerController.cpp',
    'UI/AlphaGameplayPlayerController.cpp','PostureOnlyHUD.cpp','Tests/MainStoryRuntimeSpec.cpp'
]
for relative in required_files:
    if not (source/relative).is_file(): errors.append(f'missing native runtime file: {relative}')

all_runtime='\n'.join(p.read_text(encoding='utf-8',errors='ignore') for p in source.rglob('*') if p.suffix in {'.h','.cpp'} and 'Tests' not in p.parts)
for fact in ('Story.MarcDead','Story.DeniseDead','Story.EthanAbducted','Story.EthanRecovered','World.DriftwoodBeachReached','World.MoranOpeningRouteKnown','Ship.LaLiberacionOwned','World.RexaEntered','Story.EthanAliveConfirmed','Story.EthanRouteMarksFound','Story.FirstHolderCrossed','Story.MainComplete','Story.ArmadaBreachOpen','Story.ChainBroken','Story.BlackDeckReached','Story.MainCampaignComplete'):
    if fact not in all_runtime: errors.append(f'missing runtime fact: {fact}')

# Chapters 3-10 must have one physical source contract per canonical mission and a contiguous map graph.
physical_missions=[m for m,ch in missions if int(ch)>=3]
contracts={}
map_ids=set()
for path in sorted((root/'ContentSource'/'Story').glob('Chapter*/C*.json')):
    try: data=json.loads(path.read_text(encoding='utf-8'))
    except Exception as exc:
        errors.append(f'invalid story JSON {path.relative_to(root)}: {exc}'); continue
    mission=data.get('missionId')
    map_id=data.get('mapId')
    if not mission or not map_id: continue
    if mission in contracts: errors.append(f'duplicate physical contract for {mission}')
    contracts[mission]=(path,data)
    if map_id in map_ids: errors.append(f'duplicate physical story mapId {map_id}')
    map_ids.add(map_id)
    if not data.get('entryAnchor'): errors.append(f'{mission} has no entryAnchor')
    if not data.get('actors'): errors.append(f'{mission} has no physical actors')

for index,mission in enumerate(physical_missions):
    if mission not in contracts:
        errors.append(f'missing physical story contract: {mission}'); continue
    data=contracts[mission][1]
    expected=physical_missions[index+1] if index+1<len(physical_missions) else ''
    actual=data.get('nextMission','')
    if actual!=expected: errors.append(f'{mission} nextMission={actual!r}, expected {expected!r}')

extra=set(contracts)-set(physical_missions)
for mission in sorted(extra): errors.append(f'non-canonical physical mission contract: {mission}')

# Chapter 10 naval population/completion has one authority: ContentSource/Naval + the dedicated naval materializer.
# The generic story naval actors remain available for C03 Safe Routes only; duplicating them in C10 would spawn two fleets/gates.
for mission in ('Main.C10.01.Armada','Main.C10.02.BreakTheChain'):
    if mission in contracts:
        actor_types=[a.get('type') for a in contracts[mission][1].get('actors',[]) if isinstance(a,dict)]
        duplicated=sorted(set(actor_types)&{'PlayerShip','NavalEnemy','NavalEncounterGate'})
        if duplicated: errors.append(f'{mission} duplicates dedicated naval authority with story actors: {duplicated}')
naval_contracts={}
for path in sorted((root/'ContentSource'/'Naval').glob('C10_*.naval.json')):
    try: data=json.loads(path.read_text(encoding='utf-8'))
    except Exception as exc:
        errors.append(f'invalid naval JSON {path.relative_to(root)}: {exc}'); continue
    naval_contracts[data.get('missionId')]=data
for mission in ('Main.C10.01.Armada','Main.C10.02.BreakTheChain'):
    data=naval_contracts.get(mission)
    if not data:
        errors.append(f'missing dedicated naval contract: {mission}'); continue
    if len(data.get('hostiles',[]))!=3: errors.append(f'{mission} dedicated naval contract must materialize exactly 3 hostiles')
    if data.get('completionGate',{}).get('requiredSunkShips')!=3: errors.append(f'{mission} dedicated naval gate must require 3 sunk ships')
naval_materializer=(root/'Source/DarkArisenEditor/Private/DarkArisenMaterializeNavalCommandlet.cpp').read_text(encoding='utf-8')
for token in ('ALaLiberacionShip','AHostileNavalShip','ANavalMissionGateActor','RequiredSunkShips'):
    if token not in naval_materializer: errors.append(f'dedicated naval materializer missing {token}')

materializer=(root/'Source/DarkArisenEditor/Private/DarkArisenMaterializeStoryCommandlet.cpp').read_text(encoding='utf-8')
for token in ('MainStoryMapTransitionActor','DuelingEnemy','PlayerShip','NavalEnemy','NavalEncounterGate','MaterializeCredits','L_Credits'):
    if token not in materializer: errors.append(f'story materializer missing {token}')
credits_contract=root/'ContentSource/Presentation/Credits/L_Credits.contract.json'
credits_authority=root/'ContentSource/Story/Credits/CreditsAuthority.json'
if not credits_contract.is_file(): errors.append('missing L_Credits presentation contract')
if not credits_authority.is_file(): errors.append('missing factual credits authority')
packaging=(root/'Config/DefaultGame.ini').read_text(encoding='utf-8')
if '/Game/Alpha/Maps/L_Credits' not in packaging: errors.append('L_Credits is not cook-listed')
render_cfg=(root/'Config/DefaultEngine.ini').read_text(encoding='utf-8')
for token in ('DefaultGraphicsRHI_DX12','PCD3D_SM6','r.RayTracing=True','r.PathTracing=True','r.Nanite.ProjectEnabled=True','r.Lumen.HardwareRayTracing=True'):
    if token not in render_cfg: errors.append(f'missing alpha rendering requirement: {token}')
user_cfg=(root/'Config/DefaultGameUserSettings.ini').read_text(encoding='utf-8')
for token in ('ResolutionSizeX=3840','ResolutionSizeY=2160','sg.GlobalIlluminationQuality=3','sg.ReflectionQuality=3'):
    if token not in user_cfg: errors.append(f'missing default 4K Epic setting: {token}')
input_cfg=(root/'Config/DefaultInput.ini').read_text(encoding='utf-8')
for action in ('PauseMenu','WorldMap','MiniMap','QuickSave','QuickLoad'):
    if f'ActionName="{action}"' not in input_cfg: errors.append(f'missing alpha shell input action: {action}')
menu=(source/'UI/AlphaMenuPlayerController.cpp').read_text(encoding='utf-8')
for token in ('StartNewGame','ContinueGame','SetQualityPreset','ToggleFullscreen'):
    if token not in menu: errors.append(f'front-end menu missing {token}')
gameplay_ui=(source/'UI/AlphaGameplayPlayerController.cpp').read_text(encoding='utf-8')
for token in ('QuickSave','QuickLoad','ToggleWorldMap','ToggleMiniMap','OpenSettings','EnablePathTracing','SetResolutionPreset'):
    if token not in gameplay_ui: errors.append(f'gameplay shell missing {token}')
hud=(source/'PostureOnlyHUD.cpp').read_text(encoding='utf-8')
for token in ('DrawMiniMap','EAlphaOverlay::Map','SAVE GAME','SETTINGS'):
    if token not in hud: errors.append(f'gameplay HUD missing {token}')

combat=(source/'Components/CombatComponent.cpp').read_text(encoding='utf-8')
for token in ('Combat.RacheUnlocked','Combat.RachePrimed','CurrentRache = MaxRache','StartRache()'):
    if token not in combat: errors.append(f'Rache runtime progression missing {token}')
if r'\\n#include' in all_runtime or r'\\n    UFUNCTION' in all_runtime:
    errors.append('literal escaped newline found in native declaration/include block')
ethan=(source/'Characters/EthanHarlowCharacter.h').read_text(encoding='utf-8')
for token in ('ethan.harlow.real','bool IsFriendly() const{return true;}','bool IsCaptive() const'):
    if token not in ethan: errors.append(f'real Ethan runtime contract missing {token}')
story_subsystem=(source/'Story/MainStorySubsystem.cpp').read_text(encoding='utf-8')
for boss_id in ('boss.dream_ethan','boss.draven_voss'):
    if boss_id not in story_subsystem: errors.append(f'finale boss registry missing {boss_id}')
contact=(source/'Story/MainStoryContactActor.cpp').read_text(encoding='utf-8')
for stale in ('IsMissionActive(','IsMissionComplete(','CompleteAuthoredMission(MissionId, CheckpointId, SpawnId)'):
    if stale in contact: errors.append(f'story contact still calls stale subsystem API: {stale}')

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
print(f'Alpha runtime static verification passed (34 canonical missions; {len(physical_missions)} physical Chapter 3-10 contracts; contiguous travel graph; native credits; naval finale; menu/map/minimap/save/settings shell).')
