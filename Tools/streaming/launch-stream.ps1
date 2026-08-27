[CmdletBinding()]
param(
    [string]$Executable,
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [string]$UserDataRoot,
    [int]$NvencTimeoutSeconds = 90,
    [switch]$Wait
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Executable)) {
    $Pointer = Join-Path $RuntimeRoot "active-release.txt"
    if (-not (Test-Path -LiteralPath $Pointer -PathType Leaf)) {
        throw "No executable or active-release pointer was supplied."
    }
    $ReleaseRoot = (Get-Content -LiteralPath $Pointer -Raw).Trim()
    $Executable = Get-ChildItem -LiteralPath $ReleaseRoot -Filter "DarkArisen.exe" -File -Recurse |
        Select-Object -First 1 -ExpandProperty FullName
}

if ([string]::IsNullOrWhiteSpace($Executable) -or -not (Test-Path -LiteralPath $Executable -PathType Leaf)) {
    throw "Packaged DarkArisen executable does not exist: $Executable"
}

$NvidiaSmi = Get-Command "nvidia-smi.exe" -ErrorAction SilentlyContinue
if (-not $NvidiaSmi) { throw "NVIDIA driver tooling is required; NVENC cannot be verified." }
$Gpu = & $NvidiaSmi.Source --query-gpu=name --format=csv,noheader
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace(($Gpu -join ""))) {
    throw "No usable NVIDIA GPU was detected."
}

New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
if ([string]::IsNullOrWhiteSpace($UserDataRoot)) {
    $UserDataRoot = Join-Path $RuntimeRoot "UserData"
}
$UserDataRoot = [System.IO.Path]::GetFullPath($UserDataRoot)
New-Item -ItemType Directory -Path $UserDataRoot -Force | Out-Null
$GameLog = Join-Path $RuntimeRoot "DarkArisen-streaming.log"
$Arguments = @(
    "-PixelStreamingURL=ws://127.0.0.1:8888",
    "-RenderOffscreen",
    "-Unattended",
    "-NoTextureStreaming",
    "-ResX=1920",
    "-ResY=1080",
    "-ForceRes",
    "-AudioMixer",
    "-UserDir=`"$UserDataRoot`"",
    "-AbsLog=$GameLog"
)

$Process = Start-Process -FilePath $Executable -ArgumentList $Arguments -PassThru
Set-Content -LiteralPath (Join-Path $RuntimeRoot "game.pid") -Value $Process.Id -Encoding ASCII

$Deadline = (Get-Date).AddSeconds($NvencTimeoutSeconds)
$Verified = $false
while ((Get-Date) -lt $Deadline -and -not $Process.HasExited) {
    if (Test-Path -LiteralPath $GameLog) {
        $Log = Get-Content -LiteralPath $GameLog -Raw
        if ($Log -match "(?i)(failed|unable).*(NVENC|hardware encoder)|(software encoder)") {
            Stop-Process -Id $Process.Id -Force
            Remove-Item -LiteralPath (Join-Path $RuntimeRoot "game.pid") -Force -ErrorAction SilentlyContinue
            throw "Pixel Streaming fell back from NVENC. See $GameLog"
        }
        if ($Log -match "(?i)(NVENC|NVIDIA Video Codec).*(initializ|created|active|selected)") {
            $Verified = $true
            break
        }
    }
    Start-Sleep -Seconds 2
    $Process.Refresh()
}

if (-not $Verified) {
    if (-not $Process.HasExited) { Stop-Process -Id $Process.Id -Force }
    Remove-Item -LiteralPath (Join-Path $RuntimeRoot "game.pid") -Force -ErrorAction SilentlyContinue
    throw "NVENC activation was not proven within $NvencTimeoutSeconds seconds. See $GameLog"
}

Write-Host "Dark Arisen is connected to the local signalling server with NVENC verified."
if ($Wait) {
    Wait-Process -Id $Process.Id
    $Process.Refresh()
    $PidPath = Join-Path $RuntimeRoot "game.pid"
    if (Test-Path -LiteralPath $PidPath) {
        $RecordedPid = (Get-Content -LiteralPath $PidPath -Raw).Trim()
        if ($RecordedPid -eq [string]$Process.Id) {
            Remove-Item -LiteralPath $PidPath -Force
        }
    }
    exit $Process.ExitCode
}
