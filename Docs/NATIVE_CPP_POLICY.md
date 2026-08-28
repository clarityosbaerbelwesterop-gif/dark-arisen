# Native C++ Code Policy

## Decision

All Dark Arisen executable project logic is authored in native C++.

This includes:

- gameplay/runtime systems,
- Unreal Automation tests,
- repository/design-law validation,
- runner preflight,
- build/package/promotion orchestration,
- Pixel Streaming bootstrap, launch, deployment, service setup and evidence collection,
- cloud host shutdown adapters and private-tailnet bootstrap.

The authoritative operations executable is `DarkArisenOps`, built from `Source/DarkArisenOps/Private/*.cpp`.

## Allowed non-C++ files

The following are not alternate implementation languages and remain in the formats required by their consumers:

- `.uproject`, `.ini`, `.json`, `.yaml/.yml`, `.hujson`, Caddyfile and Markdown data/configuration/documentation,
- GitHub Actions workflow YAML,
- Unreal Build Tool metadata ending in `.Target.cs` or `.Build.cs`.

No gameplay, validator, deployment or streaming behavior may be implemented in those metadata/configuration files beyond invoking the native C++ owner and vendor tools.

## Forbidden source layers

`Source/` and `Tools/` must not contain executable Python, PowerShell, Bash, JavaScript or TypeScript source. The native validator fails on `.py`, `.ps1`, `.sh`, `.bash`, `.js`, `.jsx`, `.ts`, `.tsx`, `.cmd` or `.bat` files in those trees. C# is permitted only for Unreal Build Tool `.Target.cs` / `.Build.cs` metadata.

## Pixel Streaming frontend

Dark Arisen does not maintain a custom TypeScript frontend overlay. The UE 5.5-pinned Epic Pixel Streaming Infrastructure stock frontend is built and served. This avoids reintroducing a second application-language stack while retaining the vendor-supported player implementation.

## Safety gates retained in C++

`DarkArisenOps` keeps the existing fail-closed gates for:

- UE 5.5 association and runner architecture,
- Git LFS and free-disk preflight,
- M0-M6 design/source invariants and M7/M8 source-plan invariants,
- exact clean commit packaging,
- packaged credential/material scan,
- Windows/Linux candidate manifests and SHA-256,
- accepted-Alpha evidence and promotion gating,
- NVENC proof with software-encoder rejection,
- private Tailnet / Funnel reset and private service ports,
- provider deallocation/stop rather than guest-OS shutdown only.

## M7/M8 rule

All new M7/M8 gameplay, content-runtime, save/accessibility/release logic and its regression tests are C++. Data assets and authored content remain Unreal assets/data rather than code.
