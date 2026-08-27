[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE55_ROOT,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")),
    [int]$MinimumFreeDiskGb = 120
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not $IsWindows) {
    throw "Dark Arisen Unreal CI requires a dedicated Windows x64 runner."
}
if (-not [Environment]::Is64BitOperatingSystem) {
    throw "The runner operating system must be x64."
}
if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw "UE55_ROOT must point to the private Unreal Engine 5.5 installation."
}

$RequiredFiles = @(
    (Join-Path $ProjectRoot "DarkArisen.uproject"),
    (Join-Path $EngineRoot "Engine\Build\Build.version"),
    (Join-Path $EngineRoot "Engine\Build\BatchFiles\Build.bat"),
    (Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe")
)
foreach ($Path in $RequiredFiles) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Required runner file is missing: $Path"
    }
}

$BuildVersion = Get-Content -LiteralPath (Join-Path $EngineRoot "Engine\Build\Build.version") -Raw |
    ConvertFrom-Json
if ($BuildVersion.MajorVersion -ne 5 -or $BuildVersion.MinorVersion -ne 5) {
    throw "UE55_ROOT is not Unreal Engine 5.5. Detected $($BuildVersion.MajorVersion).$($BuildVersion.MinorVersion)."
}

$Project = Get-Content -LiteralPath (Join-Path $ProjectRoot "DarkArisen.uproject") -Raw |
    ConvertFrom-Json
if ($Project.EngineAssociation -ne "5.5") {
    throw "DarkArisen.uproject must remain associated with Unreal Engine 5.5."
}

foreach ($Command in @("git.exe", "python.exe")) {
    if (-not (Get-Command $Command -ErrorAction SilentlyContinue)) {
        throw "Required runner command is missing: $Command"
    }
}
& git.exe lfs version | Out-Null
if ($LASTEXITCODE -ne 0) { throw "Git LFS is required on the runner." }

$ProjectDrive = Get-Item -LiteralPath $ProjectRoot
$Drive = Get-PSDrive -Name $ProjectDrive.PSDrive.Name
$FreeDiskGb = [Math]::Floor($Drive.Free / 1GB)
if ($FreeDiskGb -lt $MinimumFreeDiskGb) {
    throw "Runner disk has $FreeDiskGb GB free; at least $MinimumFreeDiskGb GB is required."
}

Write-Host "Runner preflight passed: Windows x64, UE 5.5, Git LFS, Python, and $FreeDiskGb GB free."
Write-Host "Required GitHub labels: self-hosted, Windows, X64, ue5.5, dark-arisen"
