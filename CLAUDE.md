# Dark Arisen — Claude Code studio configuration

This repository uses an adapted installation of **Donchitos/Claude-Code-Game-Studios** for structured AI game-production roles. The upstream project is MIT-licensed and designed around Claude Code agents/skills. Dark Arisen adopts the studio methodology, not the upstream executable shell hooks.

## Mandatory authority order

Before proposing or writing anything, read in this order:

1. `Docs/AGENT_RULES.md`
2. `SECURITY.md`
3. `Docs/DesignAuthority.md`
4. `Docs/NON_AI_SLOP_STANDARD.md`
5. `HANDOVER.md`
6. task-specific design sources named by the active plan/PR
7. `Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md` and the active tranche plan where applicable

Current operator direction outranks older project documents. Locked design laws may not be changed without explicit operator approval.

The Non-AI-Slop Standard is a production rejection gate, not an optional style guide. A technically valid prompt, asset brief, level, animation, cinematic or external-media candidate must still be rejected when it is generic, decorative, physically weightless, culturally interchangeable, over-signalled or filled with provider defaults instead of source-backed Dark Arisen intent.

## Native C++ rule

All executable Dark Arisen project logic is C++.

- Gameplay/runtime: `Source/DarkArisen/**`
- Build, validation, packaging, streaming and AI-provider operations: `Source/DarkArisenOps/**`
- `.Build.cs` / `.Target.cs` are Unreal Build Tool metadata only.
- Markdown, YAML, INI, JSON/HUJSON and Unreal asset files are data/configuration, not implementation layers.
- Do not add Python, JavaScript/TypeScript, PowerShell, Bash, batch or command-script implementation under `Source/` or `Tools/`.
- Do not import upstream Game Studios hooks/statusline scripts. Their responsibilities must be implemented or retained in native C++ project tooling.

## Game Studios adaptation

Upstream: `Donchitos/Claude-Code-Game-Studios`, pinned integration reference `984023ddac0d5e27624f2baacde6105e45de375f`.

Use the following studio roles for authored-content production:

- **producer** — scope, dependencies, handoff, no duplicated work
- **creative-director** — vision/design-law consistency
- **technical-director / unreal-specialist** — UE5.5 architecture and C++ integration
- **level-designer** — dungeon/region spatial layouts, pacing, encounter flow, shortcuts
- **world-builder** — environmental storytelling, region coherence and placement
- **technical-artist** — asset/animation/VFX specifications and performance constraints
- **writer / narrative-director** — authored dialogue/narrative only where canon permits
- **sound-designer / audio-director** — environmental audio and non-reactive music law
- **qa-lead / qa-tester** — negative cases, regression contracts, runtime evidence separation
- **accessibility-specialist** — accessibility without changing locked combat timing
- **release-manager** — immutable private Alpha evidence and packaging boundaries

### Delegation rule

A role may elaborate **implementation detail** inside a locked design boundary, but may not invent missing canon merely to fill a count. If the governing source does not settle a narrative identity, number, mechanic, site, dialogue line or outcome, write a `DESIGN-GAP` and escalate it.

A role may not use visual polish as a substitute for authored design. Before accepting an important content/asset proposal, apply the review order in `Docs/NON_AI_SLOP_STANDARD.md`: canon, function, silhouette/composition, physical credibility, restraint, repetition and runtime role.

### Content workflow

For every content family:

1. Re-fetch/re-read the current branch and governing documents.
2. Build a source-backed catalog of what is actually authored.
3. Record contradictions and gaps before implementation.
4. Apply `Docs/NON_AI_SLOP_STANDARD.md` before provider submission or production acceptance.
5. Write native C++ authority/integration code.
6. Add native Unreal Automation tests and `DarkArisenOps` source validation.
7. Keep runtime/UE evidence open until actually observed.
8. Commit each coherent family separately.

## NVIDIA Kimi K3 assistant

Dark Arisen uses NVIDIA NIM model `moonshotai/kimi-k3` as a **bounded studio assistant** for level-design proposals, animation requirements, spatial breakdowns, content consistency reviews and other elaboration tasks.

- The API key is supplied only as GitHub secret/environment variable `NVIDIA_API_KEY`.
- Never read back, print, serialize or commit the key.
- NVIDIA/Kimi output is proposal material, not canon and not an automatic code/content write.
- Every Kimi job must include its governing-source list and ask the model to emit unresolved gaps instead of filling them.
- Every Kimi job that proposes content, visuals, levels, animation, cinematics or audio must also apply `Docs/NON_AI_SLOP_STANDARD.md` and explicitly reject generic filler/provider-default completion.
- No AI proposal may auto-merge.
- Provider calls are performed only by native `DarkArisenOps` C++ tooling.
- If the approved self-hosted runner is offline, jobs remain unexecuted; do not substitute an unapproved paid runner.

## Higgsfield — current approved scope

Higgsfield is currently a bounded **animation, body/performance and cinematic/cutscene previs** provider. It is never design authority.

Exactly the source-grounded motion/presentation requirements in `FExternalAssetProductionCatalog` may enter the current Higgsfield queue: 23 animation/performance requirements plus 14 resolved cinematic identities.

Current Higgsfield scope explicitly excludes:

- canonical character faces/costumes;
- boss physical-look sheets;
- dungeon/world/settlement look development;
- La Liberación look development;
- fauna/flora look development;
- props/treasures;
- general 3D meshes/GLBs/rigged-mesh production;
- dialogue writing;
- missing main-story or mission identities.

Internal visual and 3D-readiness catalogs remain valid production authorities for later art/modeling work, but they are not Higgsfield submission queues.

For every permitted Higgsfield brief:

- send only source-backed requirements whose unknowns remain explicit;
- reject mood/style-adjective-only prompts;
- reject output that fills a withheld fact or `DESIGN-GAP`, even when visually strong;
- generated video remains motion/camera/performance reference, not canon, an AnimSequence, AnimMontage, AnimNotify, retargeted clip, Sequencer asset or runtime evidence;
- provider cost/plan/rights preflight remains mandatory;
- no purchase, subscription change or credit spend is self-authorized.

The connected Free-plan preflight currently blocks the tested Seedance path before job creation with `Requires basic plan or higher`; rejected attempts consumed zero credits. Do not upgrade the provider automatically.

## Voice production

AI-generated voices are a production aid, not an authority.

- A shipping voice line requires a stable dialogue-lock ID, casting role, pronunciation record, subtitle text and rights/cost approval.
- Unlocked lines may be used only as clearly labelled auditions/prototypes and must not be committed as shipping assets.
- Do not imitate a real performer without appropriate authorization.

## Main-story stop boundary

`Docs/MAIN_QUEST_AUTHORITY_GAP.md` is the current stop boundary after the pre-main production families are reconciled. Do not implement main-mission chains from chapter summaries alone, and do not restore legacy Ethan/Draven final-act material unless current design authority explicitly resolves that conflict.

## Hard reporting rule

Source present != compiled != executed != runtime-verified != Alpha-accepted. State exactly which of those levels has evidence. Never turn unavailable runners into a green claim.
