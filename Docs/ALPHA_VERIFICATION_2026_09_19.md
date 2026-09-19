# Alpha verification — 2026-09-19

**State: source integration; Alpha NOT accepted.** Continuation PR: #52.
Reviewed baseline: `5b1f18c34fe22d03c63fab97f336e1ccf40e9efa`, then integrated concurrent
`main` changes through `7ca98058b559b730fe82140cba45a3089837d493` without discarding
the player-runtime save or Higgsfield source registrations.

## Repairs

- CI no longer references the unavailable `env` context in job conditions. Both UE jobs remain required and same-repository-only for PRs.
- Native Alpha operations now use the declared `DarkArisenOps` functions and shared platform/process helpers. The Editor build precedes all three materialisers, required assets/maps, Development/Shipping builds and Automation. Packaging retains the existing clean-commit, credential, manifest and hash gates.
- The wreckage import points at its existing source file.
- Story map changes capture live world, NPC, quest, progression, player and ship snapshots without creating an extra disk autosave. World state is restored before destination actors begin play. Player/ship coordinates only restore in their recorded source level.
- Authored facts are applied before completion listeners and legal chapter saves. Chapter saves honor the lake-to-dock suppression; rest save requests have a gameplay consumer.
- Save loading rejects corrupt mission ordering/identities, invalid current missions/chapters, invalid player vitals and corrupt NPC social graphs. Headless story tests cannot overwrite the real player slot.
- Fully decayed NPC memories no longer leave a residual reputation penalty.

## Evidence and limits

- Local actionlint 1.7.12 passes both workflows with the declared self-hosted labels.
- Existing Alpha runtime/story static validators and whitespace checks pass. These inspect source contracts, not actual playability.
- The native pipeline translation unit passes isolated process-routing tests for both platform branches. These use mocked engine utilities/process execution and are **not** a Windows build, an Unreal compile, an import test or runtime acceptance.
- Four Unreal regression tests cover corrupted saves, world transfer, ship map coordinates and NPC snapshot/decay integrity. They remain **unexecuted** until a licensed UE runner runs them.
- GitHub run `35458284445` executed the static job successfully on PR head `820f0ae3ad31412035e8c50b7b7bb36777f124b3`; both UE jobs were queued without an assigned runner at observation. Later PR revisions require their own results.

## Remaining acceptance blockers

1. There is no local licensed UE installation. Neither queued platform job supplies compile, Automation, cooking, package, frame-time or start-to-credits evidence.
2. Native `validate` still enforces the repository's C++-only/self-hosted policy, while the existing baseline contains Python/shell tools and a hosted Python static job. This pre-existing source/policy conflict has not been waived or hidden; it still needs resolution before native validation can pass.
3. Full-content validation finds no playable dungeon contracts (61 required), buried-hoard contracts (16 required) or materialised world-region contracts (7 required). One dungeon identity and 13 hoard identities remain explicitly withheld/undescribed by their authorities.
4. The checked-in hero/ship glTFs fail the visual release validator. The new Harlow/La Liberacion Higgsfield records are `SOURCE_CREATED_IMPORT_PENDING`, not evidence of accepted Unreal assets.
5. The delivery checklist has 53 unchecked Alpha items, plus three separate post-Alpha PS5 items. The current full-content script counts all 56; that total must not be interpreted as 56 Alpha-only requirements.
6. Existing PRs #11, #18 and #19 are not included in this repair. The older content/story branches overlap newer authorities and cannot be blindly merged as runtime acceptance.

No completed-Alpha download, private stream, package checksum or acceptance claim is produced by these source repairs.
