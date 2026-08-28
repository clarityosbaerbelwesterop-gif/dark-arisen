#!/usr/bin/env bash
set -euo pipefail

run_automation_tests=0
if [[ "${1:-}" == "--run-automation-tests" ]]; then
    run_automation_tests=1
elif [[ $# -gt 0 ]]; then
    echo "Usage: $0 [--run-automation-tests]" >&2
    exit 2
fi

engine_root="${UE55_ROOT:-}"
project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if [[ -z "$engine_root" ]]; then
    echo "UE55_ROOT is required and must point to an Unreal Engine 5.5 Linux installation." >&2
    exit 1
fi

project_file="$project_root/DarkArisen.uproject"
build_script="$engine_root/Engine/Build/BatchFiles/Linux/Build.sh"
editor_command="$engine_root/Engine/Binaries/Linux/UnrealEditor-Cmd"
for path in "$project_file" "$build_script"; do
    if [[ ! -f "$path" ]]; then
        echo "Required file does not exist: $path" >&2
        exit 1
    fi
done

build_tail=(
    "-Project=$project_file"
    "-WaitMutex"
    "-NoHotReloadFromIDE"
    "-WarningsAsErrors"
)
"$build_script" DarkArisenEditor Linux Development "${build_tail[@]}"
"$build_script" DarkArisen Linux Development "${build_tail[@]}"
"$build_script" DarkArisen Linux Shipping "${build_tail[@]}"

if (( run_automation_tests == 0 )); then
    exit 0
fi
if [[ ! -x "$editor_command" ]]; then
    echo "UnrealEditor-Cmd does not exist or is not executable: $editor_command" >&2
    exit 1
fi

log_directory="$project_root/Saved/Logs"
mkdir -p "$log_directory"
automation_log="$log_directory/DarkArisenAutomation-Linux.log"
"$editor_command" "$project_file" \
    -NullRHI \
    -Unattended \
    -NoPause \
    -NoSplash \
    -NoP4 \
    "-ExecCmds=Automation RunTests DarkArisen.; Quit" \
    "-TestExit=Automation Test Queue Empty" \
    "-AbsLog=$automation_log"

if grep -Eq 'Result=\{Fail\}|Automation Test Failed|LogAutomationController: Error' "$automation_log"; then
    echo "At least one DarkArisen automation test failed. See $automation_log" >&2
    exit 1
fi
if ! grep -q 'DarkArisen.DesignLaws.Constants' "$automation_log"; then
    echo "The required DarkArisen design-law automation test did not run." >&2
    exit 1
fi
