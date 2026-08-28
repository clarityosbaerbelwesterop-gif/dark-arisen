#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
    echo "Usage: $0 <40-character-commit> [output-directory]" >&2
    exit 2
fi

revision="${1,,}"
if [[ ! "$revision" =~ ^[0-9a-f]{40}$ ]]; then
    echo "The candidate revision must be a complete 40-character commit SHA." >&2
    exit 2
fi

engine_root="${UE55_ROOT:-}"
project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
output_root="${2:-$project_root/Artifacts/AlphaCandidates}"
stage_root="$project_root/Saved/AlphaPackaging/Linux/$revision"

if [[ "$(uname -s)" != "Linux" || "$(uname -m)" != "x86_64" ]]; then
    echo "The Linux Alpha candidate must be packaged on Linux x86_64." >&2
    exit 1
fi
if [[ -z "$engine_root" ]]; then
    echo "UE55_ROOT must point to an Unreal Engine 5.5 Linux installation." >&2
    exit 1
fi
if [[ "$output_root" == "/" ]]; then
    echo "The output directory may not be the filesystem root." >&2
    exit 1
fi

project_file="$project_root/DarkArisen.uproject"
run_uat="$engine_root/Engine/Build/BatchFiles/RunUAT.sh"
build_version="$engine_root/Engine/Build/Build.version"
for path in "$project_file" "$run_uat" "$build_version"; do
    if [[ ! -f "$path" ]]; then
        echo "Required packaging file is missing: $path" >&2
        exit 1
    fi
done
if [[ ! -x "$run_uat" ]]; then
    echo "RunUAT.sh is not executable: $run_uat" >&2
    exit 1
fi
for command_name in git python3 zip sha256sum find grep; do
    if ! command -v "$command_name" >/dev/null 2>&1; then
        echo "Required Linux packaging command is missing: $command_name" >&2
        exit 1
    fi
done

actual_revision="$(cd "$project_root" && git rev-parse HEAD)"
if [[ "$actual_revision" != "$revision" ]]; then
    echo "Requested revision $revision does not match checked-out commit $actual_revision." >&2
    exit 1
fi
if [[ -n "$(cd "$project_root" && git status --porcelain --untracked-files=no)" ]]; then
    echo "Tracked source changes are present. Package only an exact committed revision." >&2
    exit 1
fi
if [[ -e "$stage_root" ]]; then
    echo "Candidate staging path already exists; inspect or move it before retrying: $stage_root" >&2
    exit 1
fi
mkdir -p "$stage_root" "$output_root"

"$run_uat" BuildCookRun \
    "-project=$project_file" \
    -noP4 \
    -platform=Linux \
    -clientconfig=Shipping \
    -build \
    -cook \
    -allmaps \
    -stage \
    -pak \
    -archive \
    "-archivedirectory=$stage_root" \
    -nodebuginfo \
    -utf8output

executable="$(find "$stage_root" -type f -perm -111 \( -name 'DarkArisen' -o -name 'DarkArisen.sh' -o -name 'DarkArisen-Linux-Shipping' \) -print -quit)"
pak="$(find "$stage_root" -type f -name '*.pak' -print -quit)"
if [[ -z "$executable" ]]; then
    echo "Packaged DarkArisen Linux executable is missing from $stage_root" >&2
    exit 1
fi
if [[ -z "$pak" ]]; then
    echo "Packaged .pak content is missing from $stage_root" >&2
    exit 1
fi

while IFS= read -r -d '' packaged_file; do
    packaged_name="${packaged_file##*/}"
    lower_name="${packaged_name,,}"
    case "$lower_name" in
        .env|.env.*|id_rsa|credentials*|*.pem|*.pfx|*.p12|*.key|*.local.json|*.local.psd1)
            echo "Candidate contains a forbidden credential-like file: $packaged_file" >&2
            exit 1
            ;;
    esac
done < <(find "$stage_root" -type f -print0)

if grep -I -r -E -l \
    --include='*.ini' --include='*.json' --include='*.txt' --include='*.cfg' \
    --include='*.xml' --include='*.yaml' --include='*.yml' \
    'AKIA[0-9A-Z]{16}|tskey-[A-Za-z0-9_-]+|-----BEGIN (RSA |EC |OPENSSH )?PRIVATE KEY-----' \
    "$stage_root" >/dev/null; then
    echo "Candidate contains credential material in a packaged text file." >&2
    exit 1
fi

payload_file_count="$(find "$stage_root" -type f | wc -l | tr -d ' ')"
payload_bytes="$(find "$stage_root" -type f -printf '%s\n' | awk '{total += $1} END {print total + 0}')"
python3 - "$build_version" "$stage_root/DarkArisen-build-manifest.json" "$revision" "$payload_file_count" "$payload_bytes" <<'PY'
import datetime
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    engine = json.load(handle)
if (engine.get("MajorVersion"), engine.get("MinorVersion")) != (5, 5):
    raise SystemExit("Packaging engine is not Unreal Engine 5.5.")
manifest = {
    "schema_version": 1,
    "artifact_kind": "candidate",
    "alpha_status": "not-accepted",
    "project": "DarkArisen",
    "platform": "Linux",
    "configuration": "Shipping",
    "candidate_commit": sys.argv[3],
    "engine_version": (
        f"{engine.get('MajorVersion')}.{engine.get('MinorVersion')}."
        f"{engine.get('PatchVersion')}"
    ),
    "pixel_streaming": True,
    "content_file_count": int(sys.argv[4]),
    "content_bytes": int(sys.argv[5]),
    "generated_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "notice": (
        "Private single-reviewer candidate. Not an accepted Alpha until promoted "
        "with complete evidence."
    ),
}
with open(sys.argv[2], "w", encoding="utf-8") as handle:
    json.dump(manifest, handle, indent=2, sort_keys=True)
    handle.write("\n")
PY

short_revision="${revision:0:12}"
zip_name="DarkArisen-Candidate-Linux-x86_64-$short_revision.zip"
zip_path="$(cd "$(dirname "$output_root")" && pwd)/$(basename "$output_root")/$zip_name"
if [[ -e "$zip_path" ]]; then
    echo "Candidate ZIP already exists: $zip_path" >&2
    exit 1
fi
(cd "$stage_root" && zip -q -r "$zip_path" .)
hash_value="$(sha256sum "$zip_path" | awk '{print $1}')"
printf '%s  %s\n' "$hash_value" "$zip_name" >"$zip_path.sha256"

echo "Linux Alpha candidate created privately: $zip_path"
echo "SHA-256: $hash_value"
echo "It remains a Candidate until Tools/ci/promote-alpha-candidates.py verifies complete acceptance evidence."
