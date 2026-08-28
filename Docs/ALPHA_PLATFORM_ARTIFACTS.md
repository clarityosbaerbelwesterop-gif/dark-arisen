# Alpha platform artifact contract

**Status:** delivery contract; no platform artifact exists yet  
**Engine:** Unreal Engine 5.5  
**Privacy:** private single-reviewer delivery only

The complete Alpha has three distinct deliverables. They are generated only from the exact accepted commit after every M0-M8 source, runtime, content and performance gate is green.

| Artifact | Required contents | Acceptance path |
|---|---|---|
| `DarkArisen-Alpha-UE55-Source.zip` | Complete private UE 5.5 project, source and authorised content; no caches, credentials or provider secrets | Clean-machine project generation, Development/Shipping compile, automation and SHA-256 verification |
| `DarkArisen-Alpha-Windows-Arcware.zip` | Reproducible Windows Shipping package with Pixel Streaming enabled and no embedded access token or public URL | Flo uploads it to his chosen private Arcware project, verifies controller/touch play and proves unauthorised access is denied |
| Native PS5 test artifact | Platform-native package created with the authorised PS5 SDK; a private ZIP wrapper is used only when PlayStation policy permits it | Build/cook/package in the approved PlayStation environment, run on a PS5 dev/test kit, complete platform tests and record a checksum |

Arcware streams an Unreal Engine application from cloud GPUs to a browser. It can validate the Windows gameplay package and browser input path; it does not convert, emulate or certify a PS5 package. Calling the Windows stream a PS5 test would leave PS5 rendering, memory, DualSense, save, entitlement and TRC behaviour untested.

Sony makes its development tools and publishing resources available after PlayStation Partner approval and the Global Developer and Publisher Agreement. PS5 packaging therefore remains blocked until Flo supplies access through an authorised environment. No SDK, confidential documentation or platform artifact may be committed to this repository.

## Release rules

- Never upload any artifact publicly or place a playable URL in the repository or PR.
- Never buy hosting, a licence, music, voice generation or platform service without Flo's explicit approval of the exact charge.
- Record the delivery commit, build configuration, toolchain version, content manifest and SHA-256 beside each artifact.
- A package that has not passed a complete start-to-credits playthrough is not an Alpha candidate.
- The Windows and PS5 candidates must expose the same approved missions, quest chains, regions, dungeons, bosses, cutscenes, dialogue, credits and save-compatible outcomes.
- Main-story completion must transition into full credits with an approved non-reactive music cue. Credits names/order and cue rights are unresolved design inputs; no music cue is invented or sourced by assumption.

## External-access blockers

- PlayStation Partner approval, GDPA, PS5 SDK and dev/test kit.
- A matching private Windows UE 5.5 build runner for reproducible packaging.
- Flo's private Arcware account/project for the Windows upload and access-control evidence.

These blockers do not prevent authorised source work in draft PR #4, but none may be marked complete by inference.

## Public references

- [PlayStation Partners registration](https://partners.playstation.net/)
- [Sony Interactive Entertainment: partner approval unlocks development tools and publishing resources](https://sonyinteractive.com/en/news/blog/showing-your-game-to-playstation/)
- [Arcware: Unreal Engine application upload and browser Pixel Streaming](https://www.arcware.com/)
