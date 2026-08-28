#!/usr/bin/env bash
set -euo pipefail

engine_root="${UE55_ROOT:-}"
project_root="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
minimum_free_disk_gb="${DARKARISEN_MINIMUM_FREE_DISK_GB:-120}"

if [[ "$(uname -s)" != "Linux" ]]; then
    echo "Dark Arisen Linux CI requires a dedicated Linux x64 runner." >&2
    exit 1
fi
if [[ "$(uname -m)" != "x86_64" ]]; then
    echo "The Linux runner architecture must be x86_64." >&2
    exit 1
fi
if [[ -z "$engine_root" ]]; then
    echo "UE55_ROOT must point to the private Unreal Engine 5.5 Linux installation." >&2
    exit 1
fi

required_files=(
    "$project_root/DarkArisen.uproject"
    "$engine_root/Engine/Build/Build.version"
    "$engine_root/Engine/Build/BatchFiles/Linux/Build.sh"
    "$engine_root/Engine/Binaries/Linux/UnrealEditor-Cmd"
)
for path in "${required_files[@]}"; do
    if [[ ! -f "$path" ]]; then
        echo "Required Linux runner file is missing: $path" >&2
        exit 1
    fi
done

for command_name in git python3 clang zip; do
    if ! command -v "$command_name" >/dev/null 2>&1; then
        echo "Required Linux runner command is missing: $command_name" >&2
        exit 1
    fi
done
git lfs version >/dev/null

python3 - "$engine_root/Engine/Build/Build.version" "$project_root/DarkArisen.uproject" <<'PY'
import json
import sys

with open(sys.argv[1], encoding="utf-8") as handle:
    version = json.load(handle)
if (version.get("MajorVersion"), version.get("MinorVersion")) != (5, 5):
    raise SystemExit(
        f"UE55_ROOT is not Unreal Engine 5.5. Detected "
        f"{version.get('MajorVersion')}.{version.get('MinorVersion')}."
    )
with open(sys.argv[2], encoding="utf-8") as handle:
    project = json.load(handle)
if project.get("EngineAssociation") != "5.5":
    raise SystemExit("DarkArisen.uproject must remain associated with Unreal Engine 5.5.")
PY

clang_major="$(clang --version | sed -n '1s/.*version \([0-9][0-9]*\).*/\1/p')"
if [[ "$clang_major" != "18" ]]; then
    echo "UE 5.5 Linux requires clang 18.x; detected: $(clang --version | head -n 1)" >&2
    exit 1
fi

free_disk_gb="$(df -Pk "$project_root" | awk 'NR==2 {print int($4 / 1024 / 1024)}')"
if (( free_disk_gb < minimum_free_disk_gb )); then
    echo "Linux runner has ${free_disk_gb} GB free; at least ${minimum_free_disk_gb} GB is required." >&2
    exit 1
fi

echo "Runner preflight passed: Linux x86_64, UE 5.5, clang 18, Git LFS, Python, Zip, and ${free_disk_gb} GB free."
echo "Required GitHub labels: self-hosted, Linux, X64, ue5.5, dark-arisen"
