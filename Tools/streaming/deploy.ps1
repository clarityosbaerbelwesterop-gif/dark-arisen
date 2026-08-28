[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE55_ROOT,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")),
    [string]$DeploymentRoot = "C:\DarkArisen\Deployments",
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [string]$Revision = "manual"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($EngineRoot)) { throw "UE55_ROOT or -EngineRoot is required." }
if ($Revision -notmatch "^[A-Za-z0-9._-]+$") { throw "Revision contains unsafe path characters." }

$ProjectFile = Join-Path $ProjectRoot "DarkArisen.uproject"
$RunUat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
foreach ($Path in @($ProjectFile, $RunUat)) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Required file is missing: $Path" }
}

$DeploymentRoot = [System.IO.Path]::GetFullPath($DeploymentRoot)
$RuntimeRoot = [System.IO.Path]::GetFullPath($RuntimeRoot)
if ([System.IO.Path]::GetPathRoot($DeploymentRoot) -eq $DeploymentRoot) {
    throw "DeploymentRoot may not be a filesystem root."
}

$ReleaseRoot = Join-Path $DeploymentRoot $Revision
if (Test-Path -LiteralPath $ReleaseRoot) { throw "Release already exists: $ReleaseRoot" }
New-Item -ItemType Directory -Path $ReleaseRoot -Force | Out-Null
New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null

& $RunUat BuildCookRun `
    "-project=$ProjectFile" `
    -noP4 `
    -platform=Win64 `
    -clientconfig=Development `
    -build `
    -cook `
    "-map=/Engine/Maps/Entry" `
    -stage `
    -pak `
    -archive `
    "-archivedirectory=$ReleaseRoot" `
    -utf8output
if ($LASTEXITCODE -ne 0) { throw "Unreal packaging failed with exit code $LASTEXITCODE." }

$Executable = Get-ChildItem -LiteralPath $ReleaseRoot -Filter "DarkArisen.exe" -File -Recurse |
    Select-Object -First 1 -ExpandProperty FullName
if ([string]::IsNullOrWhiteSpace($Executable)) { throw "Packaged executable is missing from $ReleaseRoot" }

$Pointer = Join-Path $RuntimeRoot "active-release.txt"
$PreviousRelease = $null
if (Test-Path -LiteralPath $Pointer) {
    $PreviousRelease = (Get-Content -LiteralPath $Pointer -Raw).Trim()
    if (-not [string]::IsNullOrWhiteSpace($PreviousRelease)) {
        $PreviousRelease = [System.IO.Path]::GetFullPath($PreviousRelease)
        $DeploymentPrefix = $DeploymentRoot.TrimEnd('\') + '\'
        if (-not $PreviousRelease.StartsWith(
            $DeploymentPrefix,
            [System.StringComparison]::OrdinalIgnoreCase)) {
            throw "The active release pointer escapes DeploymentRoot. Refusing to continue."
        }
    }
}

$PidFile = Join-Path $RuntimeRoot "game.pid"
if (Test-Path -LiteralPath $PidFile) {
    $ExistingPid = (Get-Content -LiteralPath $PidFile -Raw).Trim()
    if ($ExistingPid -match "^\d+$") {
        $ExistingProcess = Get-Process -Id ([int]$ExistingPid) -ErrorAction SilentlyContinue
        if ($ExistingProcess) {
            $ExistingPath = $ExistingProcess.Path
            $ExpectedPrefix = if ([string]::IsNullOrWhiteSpace($PreviousRelease)) {
                $null
            } else {
                $PreviousRelease.TrimEnd('\') + '\'
            }
            if ([string]::IsNullOrWhiteSpace($ExistingPath) -or
                [string]::IsNullOrWhiteSpace($ExpectedPrefix) -or
                -not $ExistingPath.StartsWith(
                    $ExpectedPrefix,
                    [System.StringComparison]::OrdinalIgnoreCase) -or
                [System.IO.Path]::GetFileName($ExistingPath) -ne "DarkArisen.exe") {
                throw "PID $ExistingPid does not identify the active DarkArisen release. Refusing to stop it."
            }
            Stop-Process -Id ([int]$ExistingPid) -Force
        }
    }
    Remove-Item -LiteralPath $PidFile -Force
}

$PointerTemp = "$Pointer.new"
Set-Content -LiteralPath $PointerTemp -Value $ReleaseRoot -Encoding UTF8
Move-Item -LiteralPath $PointerTemp -Destination $Pointer -Force

try {
    & (Join-Path $PSScriptRoot "launch-stream.ps1") -Executable $Executable -RuntimeRoot $RuntimeRoot
} catch {
    if (-not [string]::IsNullOrWhiteSpace($PreviousRelease) -and (Test-Path -LiteralPath $PreviousRelease)) {
        Set-Content -LiteralPath $Pointer -Value $PreviousRelease -Encoding UTF8
        & (Join-Path $PSScriptRoot "launch-stream.ps1") -RuntimeRoot $RuntimeRoot
    }
    throw
}

Write-Host "Deployment $Revision is active. No previous release was deleted."
