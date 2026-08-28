[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE55_ROOT,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")),
    [switch]$RunAutomationTests
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-NativeChecked {
    param(
        [Parameter(Mandatory = $true)][string]$FilePath,
        [Parameter(Mandatory = $true)][string[]]$Arguments
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Native command failed with exit code $LASTEXITCODE`: $FilePath"
    }
}

if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw "UE55_ROOT is required and must point to an Unreal Engine 5.5 installation."
}

$ProjectFile = Join-Path $ProjectRoot "DarkArisen.uproject"
$BuildScript = Join-Path $EngineRoot "Engine\Build\BatchFiles\Build.bat"
$EditorCommand = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

foreach ($Path in @($ProjectFile, $BuildScript)) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Required file does not exist: $Path"
    }
}

$BuildTail = @(
    "-Project=$ProjectFile",
    "-WaitMutex",
    "-NoHotReloadFromIDE",
    "-WarningsAsErrors"
)

Invoke-NativeChecked -FilePath $BuildScript -Arguments (@("DarkArisenEditor", "Win64", "Development") + $BuildTail)
Invoke-NativeChecked -FilePath $BuildScript -Arguments (@("DarkArisen", "Win64", "Development") + $BuildTail)
Invoke-NativeChecked -FilePath $BuildScript -Arguments (@("DarkArisen", "Win64", "Shipping") + $BuildTail)

if (-not $RunAutomationTests) {
    exit 0
}

if (-not (Test-Path -LiteralPath $EditorCommand -PathType Leaf)) {
    throw "UnrealEditor-Cmd.exe does not exist: $EditorCommand"
}

$LogDirectory = Join-Path $ProjectRoot "Saved\Logs"
New-Item -ItemType Directory -Path $LogDirectory -Force | Out-Null
$AutomationLog = Join-Path $LogDirectory "DarkArisenAutomation.log"

Invoke-NativeChecked -FilePath $EditorCommand -Arguments @(
    $ProjectFile,
    "-NullRHI",
    "-Unattended",
    "-NoPause",
    "-NoSplash",
    "-NoP4",
    "-ExecCmds=Automation RunTests DarkArisen.; Quit",
    "-TestExit=Automation Test Queue Empty",
    "-AbsLog=$AutomationLog"
)

$LogText = Get-Content -LiteralPath $AutomationLog -Raw
if ($LogText -match "Result=\{Fail\}|Automation Test Failed|LogAutomationController: Error") {
    throw "At least one DarkArisen automation test failed. See $AutomationLog"
}
if ($LogText -notmatch "DarkArisen\.DesignLaws\.Constants") {
    throw "The required DarkArisen design-law automation test did not run."
}
