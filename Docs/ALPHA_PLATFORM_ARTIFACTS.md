# Alpha platform artifact contract

**Status:** delivery contract; no platform artifact exists yet  
**Engine:** Unreal Engine 5.5  
**Privacy:** private single-reviewer delivery only

The complete Alpha has exactly two playable target builds, generated from the same accepted commit after every M0–M8 source, runtime, content and performance gate is green. A private source-project archive accompanies them for reproducibility but is not a third playable platform.

| Artifact | Required contents | Acceptance path |
|---|---|---|
| `DarkArisen-Alpha-Windows-Arcware.zip` | Complete Win64 Shipping game, every approved M0–M8 asset and Pixel Streaming 2, with no credential or public URL | UE 5.5 compile/automation, native Windows start-to-credits playthrough, 60 fps evidence, private Arcware/browser test and checksum |
| `DarkArisen-Alpha-Linux-x86_64.zip` | Complete Linux x86_64 Shipping game with the same missions, regions, dungeons, bosses, dialogue, cutscenes, credits and outcomes as Windows | UE 5.5 compile/automation, native Linux start-to-credits playthrough, 60 fps evidence and checksum |
| `DarkArisen-Alpha-UE55-Source.zip` | Exact accepted private UE 5.5 project, source and authorised content; no caches, credentials, provider secrets or generated packages | Clean-machine project generation, both target compiles, content-manifest reconciliation and checksum |

Pixel Streaming transports an already packaged application; it does not compile or complete the game. Epic supports Pixel Streaming applications on Windows and Linux, but the Windows ZIP remains the current Arcware-oriented browser-test artifact. No claim is made that Arcware accepts the Linux ZIP unless Flo's chosen private Arcware project verifies that separately.

## Candidate-to-Alpha rule

Packaging first creates immutable candidate files tied to a complete 40-character commit:

- `DarkArisen-Candidate-Windows-Arcware-<12-char-commit>.zip`
- `DarkArisen-Candidate-Linux-x86_64-<12-char-commit>.zip`

Each candidate contains a build manifest and receives a SHA-256 sidecar. `Tools/ci/promote-alpha-candidates.py` copies those exact tested bytes to the final Alpha names only when an evidence JSON has `status: accepted-alpha`, the same commit and candidate hashes, and every following Boolean gate set to `true`:

- `full_content_m0_m8`
- `windows_ue55_build_and_automation`
- `linux_ue55_build_and_automation`
- `windows_start_to_credits_playthrough`
- `linux_start_to_credits_playthrough`
- `content_manifest_reconciled`
- `performance_floor_60_fps`
- `privacy_access_denied`
- `credits_music_and_licences_approved`
- `flo_alpha_approval`

Promotion does not rebuild, edit or recompress either candidate. A source scaffold, greybox, vertical slice, failed build, partial playthrough or renamed candidate is not an Alpha.

## Release rules

- Never upload a candidate/final artifact through GitHub Actions, a public release, public bucket, repository commit, issue or PR.
- Never place a playable URL, password, host name, TURN secret or provider token in the repository or artifact.
- Never buy hosting, a licence, music, voice generation, platform access or another service without Flo's explicit approval of the exact charge.
- Record delivery commit, configuration, UE version, content manifest and SHA-256 for each artifact.
- Windows and Linux must expose the same approved missions, quest chains, regions, dungeons, bosses, cutscenes, dialogue, credits and save-compatible outcomes.
- Main-story completion must enter full credits with an approved non-reactive music cue. Credits names/order and cue rights are unresolved inputs; no cue is invented, sourced or purchased by assumption.

## Current external blockers

- Matching private Windows x64 and Linux x86_64 self-hosted runners with licensed UE 5.5 installations are required for compile, automation and packaging evidence.
- Flo's private Arcware account/project is required later for the Windows browser and access-denial test; repository automation will not create a public link or start a paid host.
- The current Codex Linux workspace has neither a licensed UE 5.5 installation nor a matching Windows environment, so it can author and validate source but cannot truthfully emit either packaged candidate here.

These blockers do not prevent authorised source work. PR #4/#5 source integration did not turn any missing platform evidence green.

## Post-Alpha / Beta — native PS5

Native PS5 packaging is explicitly **not an Alpha deliverable**. It begins in Beta only after authorised Sony tools, PlayStation Partner/GDPA access and PS5 development/test hardware are available. The Beta package must be created and tested in that approved environment and delivered only in a form permitted by Sony; Pixel Streaming and Arcware do not convert or emulate it. No Sony SDK, confidential documentation or platform artifact may enter this repository.

## Public references

- [Epic: Pixel Streaming Reference](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-pixel-streaming-reference)
- [Epic: Linux development requirements](https://dev.epicgames.com/documentation/unreal-engine/linux-development-requirements-for-unreal-engine)
- [PlayStation Partners registration](https://partners.playstation.net/)
